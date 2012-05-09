----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:42:09 05/01/2012 
-- Design Name: 
-- Module Name:    spi_serializer_in - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------

library IEEE;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity spi_serializer is
	Generic (
			RCV_DATA_LENGTH : integer := 8
	);
	Port (
			spi_clk: in std_logic;
			spi_mosi: in std_logic;
			spi_ss: in std_logic; -- !(Slave select)
			spi_miso: out std_logic := '0';
			reset: in std_logic := '0';
			clk: in std_logic;
			
			-- Parallel data out
			parallel_data_out: out std_logic_vector(RCV_DATA_LENGTH-1 downto 0);
			parallel_data_out_valid: out std_logic := '0';
			parallel_data_out_collision: out std_logic := '0'; -- If the does not get emptied there will be a collision
			parallel_data_out_ack: in std_logic := '0'; -- ack that the data is read
			
			-- Parallel data in
			parallel_data_in: in std_logic_vector(RCV_DATA_LENGTH-1 downto 0);
			parallel_data_in_valid: in std_logic := '0';
			parallel_data_in_ack: out std_logic := '0' -- ack that the data is read
	);
end spi_serializer;

architecture Behavioral of spi_serializer is
	signal serial_buffer_in: std_logic_vector (RCV_DATA_LENGTH-1 downto 0) := (others => 'X'); -- data container
	signal serial_buffer_in_valid: std_logic := '0';
	signal serial_buffer_in_onport: std_logic := '0'; -- Used for collision detection
	signal serial_buffer_out: std_logic_vector (RCV_DATA_LENGTH-1 downto 0) := (others => 'X'); -- data container
	signal serial_buffer_out_valid: std_logic := '0';
	signal serial_buffer_out_in_progress: std_logic := '0';
	signal serial_buffer_out_ready: std_logic := '1'; -- used to replace faling_edge on in_progress (ready .. to receive data)
	-- When new data are received on the parallel side, ready and valid flag is set. When the serial sending begins, the in_progress is set and triggers ready to be unset. When sending is done, in_progess is unset, and triggers valid to be unset also. 
	signal serial_buffer_ptr: integer range (RCV_DATA_LENGTH-1) downto 0 := RCV_DATA_LENGTH-1; -- position in data
	
	signal parallel_data_out_ready: std_logic := '1'; -- Port data is read and ready to receive new data.
	
	signal mosi_valid: std_logic := '0'; -- used to dertermine if data on mosi is valid
--	signal main_state: main_state_type := RESET;
begin
	-- Debounce SPI clock (CPHA and CPOL logic here!)
	debounce : process ( spi_clk, spi_mosi, clk, reset )
		variable debounce_count: integer range 0 to 255 := 2;
	begin
		-- Use the internal clock to wait a few clock before the data is assumed valid
		if reset = '0' then
			if rising_edge(clk) then
				if spi_clk = '1' then
					if debounce_count = 0 then
						mosi_valid <= '1';
					else
						debounce_count := debounce_count - 1;
						mosi_valid <= '0';
					end if;
				else
					debounce_count := 2;
					mosi_valid <= '0';
				end if;
			end if;
		else
			-- RESET
			debounce_count := 2;
			mosi_valid <= '0';
		end if;
	end process;
	
	-- SPI Clock period messurement
	
	-- Shift data in
	serial_in : process ( mosi_valid, spi_mosi, reset,spi_ss )
	begin
		if reset = '0' then -- Check for master reset
			if spi_ss = '0' then -- Is the data for me?
				if rising_edge(mosi_valid) then
					serial_buffer_in(serial_buffer_ptr) <= spi_mosi;
					if serial_buffer_ptr = 0 then
						-- Data recieved serial, go to parallel mode
						serial_buffer_ptr <= RCV_DATA_LENGTH-1;
						serial_buffer_in_valid <= '1';
					else
						serial_buffer_ptr <= serial_buffer_ptr - 1;
						serial_buffer_in_valid <= '0';
					end if;
				end if;
			else
				-- Data is not for me. (Can be used to soft-reset transmission)
				-- If we were transmitting by mistake, reset the pointer to the front byte so we will be ready for transmitting again.
				serial_buffer_ptr <= RCV_DATA_LENGTH-1;
			end if;
		else
			-- RESET
			serial_buffer_in <= (others => '0');
			serial_buffer_ptr <= RCV_DATA_LENGTH-1;
			serial_buffer_in_valid <= '0';
		end if;
	end process;
	
	serial_out : process ( clk, serial_buffer_out_valid, serial_buffer_ptr, serial_buffer_out_in_progress, reset )
	begin
		if reset = '0' then
			-- Set up next data bit on faling edge
			--if falling_edge(mosi_valid) or rising_edge(serial_buffer_out_valid) then
			if rising_edge(clk) then
				if mosi_valid = '0' then
					-- Only make changes when debounced spi clk (mosi_valid) is low
					-- Made to ensure that a package always starts from the front.
					if (serial_buffer_out_in_progress = '1') or (serial_buffer_out_valid = '1' and serial_buffer_ptr = RCV_DATA_LENGTH-1) then
						spi_miso <= serial_buffer_out(serial_buffer_ptr);
						serial_buffer_out_in_progress <= '1';
					elsif serial_buffer_ptr = RCV_DATA_LENGTH-1 then
						-- If we are setting up without data. Set MISO to zero, to avoid sending garbage.
						spi_miso <= '0';
					end if;
					-- End of stream (pointer have been reset)
					if serial_buffer_ptr = 0 then
						serial_buffer_out_in_progress <= '0'; 
						-- A process will trigger on this falling edge and set buffer_out_valid to false.
					end if;
				end if;
			end if;
		else
			-- RESET
			serial_buffer_out_in_progress <= '0';
		end if;
	end process;



	-- Data out control
	data_out : process (clk, serial_buffer_in_valid,parallel_data_out_ack, reset)
	begin
		if reset = '0' then
			if rising_edge(clk) then
				if serial_buffer_in_valid = '1' then
					-- Data ready from SPI
					if serial_buffer_in_onport = '0' then
						-- Update the output port to the newest value
						if parallel_data_out_ready = '1' then
							-- Port is ready, so we can update
							parallel_data_out <= serial_buffer_in;
							parallel_data_out_valid <= '1';
							parallel_data_out_ready <= '0'; 
							serial_buffer_in_onport <= '1';
						else
							-- Data port not ready - Collision detected!
							parallel_data_out_collision <= '1';
						end if;
					end if;
				else
					serial_buffer_in_onport <= '0';
				end if;
				
				if parallel_data_out_ack = '1' then
					-- Data has been read from port
					parallel_data_out_valid <= '0';
					parallel_data_out_ready <= '1'; 
				end if;
			end if;
		else
			-- RESET
			parallel_data_out <= (others => '0');
			parallel_data_out_valid <= '0';
			parallel_data_out_ready <= '1'; 
			serial_buffer_in_onport <= '0';
			parallel_data_out_collision <= '0';
		end if;

	end process;
	
	data_in : process (clk,parallel_data_in_valid, serial_buffer_out_in_progress, reset)
	begin
		-- Control pins for incomming parallel data
		if reset = '0' then
			if rising_edge(clk) then
				if parallel_data_in_valid = '1' and serial_buffer_out_ready = '1' then
					serial_buffer_out <= parallel_data_in;
					parallel_data_in_ack <= '1';
					serial_buffer_out_valid <= '1';
					--serial_buffer_out_ready <= '1';
				else
					parallel_data_in_ack <= '0';
				end if;
				
				-- Data is beeing send. Set ready to false
				if serial_buffer_out_in_progress = '1' then
					serial_buffer_out_ready <= '0';
				end if;
				
				-- If ready and in_progress is false the sending is done. 
				if serial_buffer_out_ready = '0' and serial_buffer_out_in_progress = '0' then
					serial_buffer_out_ready <= '1';
					serial_buffer_out_valid <= '0';
				end if;
			end if;
		else
			-- RESET
			serial_buffer_out <= (others => '0');
			parallel_data_in_ack <= '0';
			serial_buffer_out_valid <= '0';
			serial_buffer_out_ready <= '1';
		end if;
	end process;

end Behavioral;

