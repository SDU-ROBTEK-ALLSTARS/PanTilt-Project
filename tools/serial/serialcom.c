#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>


#define UART_PACKET_HEADER_SIZE      4
#define UART_PACKET_MAX_DATA_SIZE    32
#define UART_PACKET_TYPE_SET         0
#define UART_PACKET_TYPE_GET         1

typedef struct
{
  int8_t type;
  int8_t instruction;
  int16_t datalength;
  int8_t data[UART_PACKET_MAX_DATA_SIZE];
} xUartPacket;

void sigIntHandler(int sig);
int portOpen(struct termios *pOrigOptions);
void portClose(void);
int sendPacket(xUartPacket *pPacket);
int echoTest(void);
int setTime(void);
void listen(unsigned int seconds, char *format);
void triggerSPITest(int8_t num_loops);
int longEchoTest(int num_packets);
void requestRuntimeStats(void);
void sendToSPI(void);

static const char *deviceName;
static int portId;
static struct termios origOptions;

int main(int argc, char *argv[])
{
  char input;
  bool run = true;
  int result;

  if( argc == 2 )
  {
    deviceName = argv[1];
  }
  else if( argc > 2 )
  {
    printf("Too many arguments supplied.\n");
  }
  else
  {
    printf("One argument expected (device name).\n");
  }

  signal(SIGINT, sigIntHandler);

  portId = portOpen(&origOptions);

  if ((portId != -1) && (echoTest() != -1))
  {
    printf("Connection opened on %s (%d) and OK!\n", deviceName, portId);

    /* Main menu */
    while (run == true)
    {
      printf("\nSelect option:\n");
      printf(" 1)\tPrint run-time stats\n");
      printf(" 2)\tSend to SPI\n");
      printf(" 4)\tListen!\n");
      printf(" 6)\tRun SPI test\n");
      printf(" 7)\tLong echo test\n");
      printf(" 0)\tExit\n");

      /* Get input char from stdin (ignore newline) */
      while ((input = fgetc(stdin)) == '\n') {}

      /* Decide action from input */
      switch (input)
      {
      case '1':
        requestRuntimeStats();
        listen(3, "%c");
        break;

      case '2':
        sendToSPI();
        listen(2, "%x");
        break;

      case '3':
        break;

      case '4':
        printf("OK! Listening...\n");
        listen(10, "%c");
        break;

      case '5':
        break;

      case '6':
        triggerSPITest(64);
        listen(10, "%c");
        break;

      case '7':
        result = longEchoTest(32);
        if (result == -1)
        {
          printf("Fail.\n");
        }
        else
        {
          printf("Pass ALL.\n");
        }
        break;

      case '0':
        run = false;
        break;

      default:
        break;
      }
    }
  }
  else
  {
    printf("Port could not be opened.\n");
  }

  printf("Closing connection and exiting.");
  portClose();

  return 0;
}

void sendToSPI(void)
{
  xUartPacket packet;
  unsigned int i;

  packet.type = UART_PACKET_TYPE_SET;
  packet.instruction = 2;
  packet.datalength = 1;

  /* Promt for packet data */
  printf("Input packet data (1 byte as hex). End with <Enter>.\n");
  scanf("%x",&i);
  packet.data[0] = (int8_t) i;

  sendPacket(&packet);
}

void requestRuntimeStats(void)
{
  xUartPacket packet;

  packet.type = UART_PACKET_TYPE_GET;
  packet.instruction = 1;
  packet.datalength = 0;

  sendPacket(&packet);
}

void triggerSPITest(int8_t num_loops)
{
  xUartPacket packet;

  packet.type = UART_PACKET_TYPE_SET;
  packet.instruction = 1;
  packet.datalength = 1;
  packet.data[0] = num_loops;
  sendPacket(&packet);
}

void listen(unsigned int seconds, char *format)
{
  char readBuf[256];
  int result, i, j;

  for (j=0; j<seconds; j++)
  {
    result = read(portId, readBuf, 255);
    if (result != -1)
    {
      for (i=0; i<result; i++)
      {
        printf (format, readBuf[i]);
      }
    }
    fflush(stdout);
    sleep(1);
  }
}

/**
 *
 */
int setTime(void)
{
  int res;
  xUartPacket packet;
  int32_t seconds = (int32_t) time(NULL);

  packet.type = UART_PACKET_TYPE_SET;
  packet.instruction = 4; /* set time instr is #4 */
  packet.datalength = 4;
  packet.data[0] = (int8_t) seconds;
  packet.data[1] = (int8_t) (seconds >> 8);
  packet.data[2] = (int8_t) (seconds >> 16);
  packet.data[3] = (int8_t) (seconds >> 24);

  res = sendPacket(&packet);

  return res;
}

/**
 *
 */
int longEchoTest(int num_packets)
{
  xUartPacket packet;
  int i, j, result;
  char readBuf[255];

  for (j=0; j<num_packets; j++)
  {
    packet.type = UART_PACKET_TYPE_GET;
    packet.instruction = 5; /* 5 is the echo instr */
    packet.datalength = UART_PACKET_MAX_DATA_SIZE;
    for (i=0; i<packet.datalength; i++)
    {
      packet.data[i] = 'a' + i;
    }

    /* Send packet */
    result = sendPacket(&packet);
    if (result != (packet.datalength + UART_PACKET_HEADER_SIZE))
    {
      return -1;
    }

    sleep(1);

    /* Read incoming */
    result = read(portId, readBuf, packet.datalength); /* only .data is echoed */
    if (result != packet.datalength)
    {
      return -1;
    }

    /* Check OK-ness */
    for (i=0; i<result; i++)
    {
      if (readBuf[i] != packet.data[i])
      {
        printf("Character mismatch error.\n");
        return -1;
      }
    }

    if (result != -1)
    {
      printf("Pass %d\n", j);
    }

    sleep(1);
  }

  return result;
}

/**
 *
 */
int echoTest(void)
{
  xUartPacket packet;
  int i, result;
  char readBuf[255];

  packet.type = UART_PACKET_TYPE_GET;
  packet.instruction = 5; /* 5 is the echo instr */
  packet.datalength = UART_PACKET_MAX_DATA_SIZE;
  for (i=0; i<packet.datalength; i++)
  {
    packet.data[i] = 'a' + i;
  }

  /* Send packet */
  result = sendPacket(&packet);
  if (result != (packet.datalength + UART_PACKET_HEADER_SIZE))
  {
    return -1;
  }

  sleep(1);

  /* Read incoming */
  result = read(portId, readBuf, packet.datalength); /* only .data is echoed */
  if (result != packet.datalength)
  {
    return -1;
  }

  /* Check OK-ness */
  for (i=0; i<result; i++)
  {
    if (readBuf[i] != packet.data[i])
    {
      printf("Character mismatch error.\n");
      return -1;
    }
  }

  return result;
}

/**
 *
 */
int sendPacket(xUartPacket *pPacket)
{
  int result[UART_PACKET_HEADER_SIZE + 1] = {0};
  int sum = 0;
  int i = 0;
  int8_t dataLengthSplit;

  /* First send the header */
  for (i=0; i<UART_PACKET_HEADER_SIZE; i++)
  {
    switch (i)
    {
    case 0:
      result[i] = write(portId, &pPacket->type, 1);
      break;
    case 1:
      result[i] = write(portId, &pPacket->instruction, 1);
      break;
    case 2:
      dataLengthSplit = (int8_t) pPacket->datalength;
      result[i] = write(portId, &dataLengthSplit, 1);
      break;
    case 3:
      dataLengthSplit = (int8_t) (pPacket->datalength >> 8);
      result[i] = write(portId, &dataLengthSplit, 1);
    default:
      break;
    }
  }

  /* Then data, if any */
  result[UART_PACKET_HEADER_SIZE] = write(portId, pPacket->data, pPacket->datalength);

  /* check OK-ness */
  for(i=0; i<(sizeof(result)/sizeof(int)); i++)
  {
    if (result[i] == -1)
    {
      return -1;
    }
    else
    {
      sum += result[i];
    }
  }

  return sum;
}

/**
 *
 */
int portOpen(struct termios *pOrigOptions)
{
  struct termios newOptions;

  int fd = open(deviceName, O_RDWR | O_NOCTTY | O_NDELAY);

  if (fd != -1)
  {
    tcgetattr(fd, pOrigOptions);
    tcgetattr(fd, &newOptions);

    newOptions.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);
    newOptions.c_oflag = 0;
    newOptions.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    newOptions.c_cflag &= ~(CSIZE | PARENB);
    newOptions.c_cflag |= CS8;
    newOptions.c_cc[VMIN]  = 1;
    newOptions.c_cc[VTIME] = 0;

    tcflush(fd, TCIFLUSH);

    cfsetispeed(&newOptions, B115200);
    cfsetospeed(&newOptions, B115200);
    tcsetattr(fd, TCSANOW, &newOptions);
  }
  return fd;
}

/**
 *
 */
void portClose(void)
{
  tcflush(portId, TCIFLUSH);
  tcsetattr(portId, TCSANOW, &origOptions);
  close(portId);
}

/**
 *
 */
void sigIntHandler(int sig)
{
  printf("\nSIGINT caught. Closing port.\n");
  portClose();
  exit(sig);
}
