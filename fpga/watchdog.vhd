----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    15:29:50 05/16/2012 
-- Design Name: 
-- Module Name:    watchdog - Behavioral 
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

entity watchdog is
    Port ( input : in  STD_LOGIC_VECTOR(1 downto 0);
           clk : in  STD_LOGIC;
           alive : out  STD_LOGIC);
end watchdog;

architecture Behavioral of watchdog is
	constant bitsize  : integer := 15;
	constant time_out : STD_LOGIC_VECTOR(bitsize downto 0) := (others=>'1');
begin

	process(clk,input)
		variable time_counter : STD_LOGIC_VECTOR(bitsize downto 0):=time_out;
		variable last_state   : STD_LOGIC_VECTOR(1 downto 0);
		variable v_alive      : STD_LOGIC;
	begin
		if rising_edge(clk) then
			if (time_counter >= time_out) then
				v_alive := '0';
			else
				v_alive := '1';
				time_counter := time_counter +1;
			end if;
		
			if(last_state /= input) then
				time_counter := (others=> '0');
				last_state := input;
			end if;
		
		alive <= v_alive;	
		end if; --rising edge clk
	end process;

end Behavioral;

