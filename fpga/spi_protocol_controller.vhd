----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    19:59:04 05/03/2012 
-- Design Name: 
-- Module Name:    spi_protocol_controller - Behavioral 
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
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;


--

entity spi_protocol_controller is
	Generic (
			RCV_DATA_LENGTH : integer := 8
	);
	Port (
		clk: in std_logic;
		--spi_clk: in std_logic;
		
		mem_en : out STD_LOGIC;
		mem_addr : out STD_LOGIC_VECTOR(4 DOWNTO 0);
		mem_we : out STD_LOGIC_VECTOR(0 DOWNTO 0);
		--mem_di : out STD_LOGIC_VECTOR(7 DOWNTO 0);
		--mem_do : in STD_LOGIC_VECTOR(7 DOWNTO 0);
		
		
		-- Parallel data out
		from_spi: in std_logic_vector(RCV_DATA_LENGTH-1 downto 0);
		from_spi_valid: in std_logic := '0';
		--from_spi_collision: in std_logic := '0'; -- You know... PANIC!
		from_spi_ack: out std_logic := '0'; -- ack that the data is read
		
		-- Parallel data in
		--to_spi: out std_logic_vector(RCV_DATA_LENGTH-1 downto 0);
		to_spi_valid: out std_logic := '0';
		to_spi_ack: in std_logic := '0' -- ack that the data is read
	);
end spi_protocol_controller;

architecture Behavioral of spi_protocol_controller is
	type STATE_TYPE is (INIT, WAITCMD, READMEM_INIT, READMEM_TO_SPI, READMEM_WAIT_SPI, WRITEMEM_INIT, WRITEMEM_WAITBYTE, WRITEMEM_WAITMEM, CLEANUP);
	signal state : STATE_TYPE := INIT;
begin

	control : process (clk)
	begin
		if rising_edge(clk) then
			case state is
			-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- INIT/WAIT -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
			when INIT =>
				-- Make sure signals are corectly set up
				from_spi_ack <= '0';
				mem_addr <= (others => '0');
				mem_en <= '0';
				state <= WAITCMD;
			when WAITCMD =>
				-- In this state we are waiting for the spi to get valid to be able to read the command.
				if from_spi_valid = '1' then
					mem_addr (4 DOWNTO 0) <= from_spi(4 downto 0);
					from_spi_ack <= '1';
					if from_spi(RCV_DATA_LENGTH-1) = '1' then
						state <= WRITEMEM_INIT;
					else
						state <= READMEM_INIT;
					end if;
				end if;
			-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- READ MEM -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
			when READMEM_INIT =>
				-- Set up memory
				mem_we <= "0";
				mem_en <= '1';
				-- Wait one clock - go to next state
				state <= READMEM_TO_SPI;
			when READMEM_TO_SPI =>
				-- Mem is now valid
				-- Enable input on SPI
				to_spi_valid <= '1';
				state <= READMEM_WAIT_SPI;
			when READMEM_WAIT_SPI =>
				-- Wait for the SPI to ack the data
				if to_spi_ack = '1' then
					mem_en <= '0';
					to_spi_valid <= '0';
					-- Data should be on its way. Clean up
					state <= CLEANUP;
				end if;
			-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- WRITE MEM -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
			when WRITEMEM_INIT =>
				-- Remove ACK flag to enable next transmission
				if from_spi_valid = '0' then
					from_spi_ack <= '0';
					mem_en <= '0';
					mem_we <= "1";
					state <= WRITEMEM_WAITBYTE;
				end if;
			when WRITEMEM_WAITBYTE =>
				if from_spi_valid = '1' then
					-- Data byte received
					-- Write to mem
					mem_en <= '1';
					state <= WRITEMEM_WAITMEM;
				end if;
			when WRITEMEM_WAITMEM =>
				-- Done writing to mem. Disable mem and ack that SPI data has been received.
				mem_en <= '0';
				from_spi_ack <= '1';
				state <= CLEANUP;
			-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- CLEANUP -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
			when CLEANUP =>
				-- Remove ACK flag to enable next transmission
				if from_spi_valid = '0' then
					from_spi_ack <= '0';
					state <= WAITCMD;
				end if;
			when others =>
				state <= INIT;
			end case;
		end if;
	end process;

end Behavioral;

