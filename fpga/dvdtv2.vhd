----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:04:45 05/20/2012 
-- Design Name: 
-- Module Name:    dvdtv2 - Behavioral 
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

entity dvdtv2 is
    Port ( position : in  STD_LOGIC_VECTOR(15 downto 0);
           clk : in  STD_LOGIC;
           velocity : out  STD_LOGIC_VECTOR(15 downto 0));
end dvdtv2;

architecture Behavioral of dvdtv2 is
	constant counter_max : STD_LOGIC_VECTOR(31 downto 0):= "00000010111110101111000010000000";
	
begin
	process(clk,position)
		variable time_counter : STD_lOGIC_VECTOR(31 downto 0):=(others=> '0');
		variable v_output : STD_LOGIC_VECTOR(15 downto 0)    :=(others=> '0');
		variable v_last_input : STD_LOGIC_VECTOR(15 downto 0):=(others=> '0');
	begin
		if rising_edge(clk) then 
			if(time_counter >= counter_max) then
				v_output:= v_last_input - position;
				v_last_input := position;
			else
				time_counter := time_counter +1;
			end if; -- time_counter >= counter_max
			velocity <= v_output;
		end if; -- clk
	end process;

end Behavioral;

