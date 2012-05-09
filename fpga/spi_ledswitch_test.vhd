----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    10:40:38 05/03/2012 
-- Design Name: 
-- Module Name:    spi_ledswitch_test - Behavioral 
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

entity spi_ledswitch_test is
	Port (
		clk: in std_logic;
		spi_data_in: in std_logic_vector (7 downto 0);
		spi_data_in_ready: in std_logic;
		spi_data_in_ack: out std_logic := '0';
		spi_data_out: out std_logic_vector (7 downto 0) := "00000000";
		spi_data_out_ready: out std_logic := '0';
		spi_data_out_ack: in std_logic;
		leds: out std_logic_vector (7 downto 0) := "00000000";
		switches: in std_logic_vector (7 downto 0)
	);
end spi_ledswitch_test;

architecture Behavioral of spi_ledswitch_test is
begin
	data_mover : process(clk,spi_data_in_ready)
	begin
--		if rising_edge(spi_data_in_ready) then
--			-- Input related
--			leds <= spi_data_in; -- Visualize output on the leds
--			spi_data_in_ack <= '1';
--			
--			-- Output related
--			spi_data_out <= switches;
--			spi_data_out_ready <= '1';
--		end if;
--		if falling_edge(spi_data_in_ready) then
--			spi_data_in_ack <= '0';
--		end if;
--		if rising_edge(spi_data_out_ack) then
--			spi_data_out_ready <= '0';
--		end if;
		
		if rising_edge(clk) then
			if spi_data_in_ready = '1' then
			
				-- Input related
				leds <= spi_data_in; -- Visualize output on the leds
				spi_data_in_ack <= '1';
				
			else
				spi_data_in_ack <= '0';
			end if;
			
			if spi_data_out_ack = '1' then
				spi_data_out_ready <= '0';
			else
				-- Output related
				spi_data_out <= switches;
				spi_data_out_ready <= '1';
			end if;
		end if;
	end process;

end Behavioral;

