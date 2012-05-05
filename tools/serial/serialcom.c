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
void setFuelMenu(void);
int setFuelPrice(char type, int price);
void printReport(void);
void listen(void);
void listenALittle(void);
void sendCustomPacket(void);
void tester();
int longEchoTest(int num_packets);

static const char *deviceName = "/dev/ttyUSB1";
static int portId;
static struct termios origOptions;

int main()
{
  char input;
  bool run = true;
  int result;

  signal(SIGINT, sigIntHandler);

  portId = portOpen(&origOptions);

  if ((portId != -1) && (echoTest() != -1))
  {
    printf("Connection opened on %s (%d) and OK!\n", deviceName, portId);

    /* Main menu */
    while (run == true)
    {
      printf("\nSelect option:\n");
      printf(" 1)\tSet time\n");
      printf(" 2)\tSet fuel price(s)\n");
      printf(" 3)\tGet full sales report\n");
      printf(" 4)\tListen!\n");
      printf(" 5)\tSend \"custom\" packet\n");
      printf(" 6)\tTest\n");
      printf(" 7)\tLong echo test\n");
      printf(" 0)\tExit\n");

      /* Get input char from stdin (ignore newline) */
      while ((input = fgetc(stdin)) == '\n') {}

      /* Decide action from input */
      switch (input)
      {
      case '1':
        result = setTime();
        if (result == -1)
        {
          printf("Error while setting time.\n");
        }
        else
        {
          printf("Time written OK!\n");
        }
        break;

      case '2':
        setFuelMenu();
        break;

      case '3':
        printReport();
        break;

      case '4':
        printf("OK! Listening...\n");
        listen();
        break;

      case '5':
        sendCustomPacket();
        sleep(1);
        listenALittle();
        break;

      case '6':
        tester();
        listen();
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



void tester(void)
{
  xUartPacket packet;
  int i;

  packet.type = UART_PACKET_TYPE_SET;
  packet.instruction = 10;
  packet.datalength = UART_PACKET_MAX_DATA_SIZE;
  for (i=0; i<packet.datalength; i++)
  {
    packet.data[i] = 'a' + i;
  }
  sendPacket(&packet);

  sleep(2);

  packet.type = UART_PACKET_TYPE_GET;
  packet.instruction = 11;
  packet.datalength = 0;
  sendPacket(&packet);
}

void sendCustomPacket(void)
{
  xUartPacket packet;
  int inputNumber;

  printf("Enter instruction #:\n");
  scanf("%d",&inputNumber);
  packet.instruction = inputNumber;

  printf("Enter type #:\n");
  scanf("%d",&inputNumber);
  packet.type = inputNumber;

  packet.datalength = 2;

  printf("Enter data byte #1:\n");
  scanf("%x",&inputNumber);
  packet.data[0] = inputNumber;
  printf("Enter data byte #2:\n");
  scanf("%x",&inputNumber);
  packet.data[1] = inputNumber;

  sendPacket(&packet);
}

void listen(void)
{
  char readBuf[256];
  int result, i;

  while(1)
  {
    result = read(portId, readBuf, 255);
    if (result != -1)
    {
      for (i=0; i<result; i++)
      {
        printf ("%c", readBuf[i]);
      }
    }
    fflush(stdout);
    sleep(1);
  }
}

void listenALittle(void)
{
  char readBuf[256];
  int result, i, j;

  for (j=0; j<5; j++)
  {
    result = read(portId, readBuf, 255);
    if (result != -1)
    {
      for (i=0; i<result; i++)
      {
        printf ("%c", readBuf[i]);
      }
    }
    fflush(stdout);
    sleep(1);
  }
}

/**
 *
 */
void printReport(void)
{
  xUartPacket packet;
  int i = 0;
  int result = 0;
  char readBuf[256] = {0};

  packet.type = UART_PACKET_TYPE_GET;
  packet.datalength = 0;

  for (i=1; i<=4; i++)
  {
    packet.instruction = (int8_t) i;
    result = sendPacket(&packet);
    sleep(1);
    if (result != -1)
    {
      result = read(portId, readBuf, 255);
      if (result != -1)
      {
        for (i=0; i<result; i++)
        {
          printf ("%c", readBuf[i]);
        }
      }
      else
      {
        printf("Error reading report.");
      }
      usleep(100000);
    }
    else
    {
      printf("Error sending GETs report.");
    }
  }
}

/**
 *
 */
void setFuelMenu(void)
{
  bool run = true;
  char input;
  int price;
  int result;

  while (run == true)
  {
    printf("Which fuel type?\n");
    printf(" 1)\t92 octan\n");
    printf(" 2)\t95 octan\n");
    printf(" 3)\tE10\n");
    printf(" 0)\tBack\n");

    /* Get input char from stdin (ignore newline) */
    while ((input = fgetc(stdin)) == '\n') {}

    /* Decide action from input */
    switch (input)
    {
    case '1':
    case '2':
    case '3':
      printf("Enter price:\n");
      scanf("%d",&price);
      result = setFuelPrice(input, price);
      if (result == -1)
      {
        printf("Error while setting fuel price.\n");
      }
      else
      {
        printf("Fuel price written OK!\n");
      }
      /* Go to case '0' */
    case '0':
      run = false;
    default:
      break;
    }
  }
}

int setFuelPrice(char type, int price)
{
  xUartPacket packet;
  int result = -1;

  packet.type = UART_PACKET_TYPE_SET;
  packet.instruction = 0;
  packet.datalength = 1;
  packet.data[0] = (int8_t) price;

  switch (type)
  {
  case '1':
    /* 92 */
    packet.instruction = 1;
    break;
  case '2':
    /* 95 */
    packet.instruction = 2;
    break;
  case '3':
    /* E10 */
    packet.instruction = 3;
    break;
  default:
    break;
  }

  if (packet.instruction != 0)
  {
    result = sendPacket(&packet);
  }

  return result;
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
