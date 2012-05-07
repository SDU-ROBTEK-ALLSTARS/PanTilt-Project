
# UART Protocol #
The protocol provides a common way of sending commands to the MCU through the UART interface. Each command that is sent is formatted as a "UART Packet", which is described below. The protocol provides a way of registering handlers to execute as specific commands are recieved.

The types and macro-definitions mentioned here are all exposed in the header file `uart_protocol.h`.

## Packet format ##
A UART packet consists of a header, which is 4 bytes long, and data. The packet is to be sent header-first, MSB-first.

<table border="1">
	<tr>
		<td>0</td>
		<td>1</td>
		<td>2</td>
		<td>3</td>	
		<td>4 to UART_PACKET_MAX_DATA_SIZE + 4</td>
	</tr>
	<tr>
		<td>TYPE</td>		
		<td>INSTRUCTION</td>
		<td colspan="2" align="center">DATA LENGTH</td>
		<td>DATA</td>
	</tr>
</table>

## Registering a handler ##
A protocol handler is a function that takes a `uart_packet_t *` as argument and returns `void`. The function gets executed by the protocol what the correct packet is received.

If we wanted a function to add data byte 0 and 1 together, when the packet `{UART_PACKET_TYPE_SET, 13, 0, {data[0],data[1]}}` was recieved, we would do it like this:

	void add_data(uart_packet_t *p_packet)
	{
		int result = p_packet->data[0] + p_packet->data[1];
	}

	uart_protocol_register_handler(&add_data, UART_PACKET_TYPE_SET, 13);

To have the handler communicate data to other parts of the program, some sort of inter-task communication constructs must be used. The protocol handler is in fact executed by the protocol task, so returning a value from the function would be meaningless.

## Protocol command table ##
See `uart_protocol_commands.md`

