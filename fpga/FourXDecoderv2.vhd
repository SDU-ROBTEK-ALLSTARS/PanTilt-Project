----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    18:22:07 05/08/2012 
-- Design Name: 
-- Module Name:    FourXDecoderv2 - Behavioral 
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
use IEEE.NUMERIC_STD.ALL;

--use IEEE.STD_LOGIC_ARITH.ALL;
--use IEEE.STD_LOGIC_UNSIGNED.ALL;
-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity FourXDecoderv2 is    
	Port ( a 					: in  STD_LOGIC;
          b 					: in  STD_LOGIC;
			 rst_counter   	: in 	STD_LOGIC;
			 clk 					: in  STD_LOGIC;
          pos 					: out STD_LOGIC_VECTOR( 15 downto 0) );
end FourXDecoderv2;


architecture Behavioral of FourXDecoderv2 is


	signal state : std_logic_vector(1 downto 0) := "00";
		
begin
	process(a,b,rst_counter,clk,state)
		variable input : std_logic_vector(1 downto 0);
		variable counter : std_logic_vector(19 downto 0):= "00001000000000000000";
	begin
		if rising_edge(clk) then
			input(0) := a;
			input(1) := b;
			counter(16) := '0';
			counter(17) := '0';
			counter(18) := '0';
			counter(19) := '0';
			--Position reset
			if(rst_counter = '1') then
				counter := "00001000000000000000";
			end if; --rst_counter ='1'
			
			--state transition
			case state is 
				when "00"   => 
					if input = "01" then 
						counter := std_logic_vector(unsigned(counter) +1);
						state <= "01";
					elsif input = "10" then  
						counter := std_logic_vector(unsigned(counter) -1);
						state <= "10";
					else 
						--
					end if;
				when "01"   =>
					if input = "11" then 
						counter := std_logic_vector(unsigned(counter) +1);
						state <= "11";
					elsif input = "00" then 
						counter := std_logic_vector(unsigned(counter) -1);
						state <= "00";
					else 
						--
					end if;
				when "11"   =>
					if input = "10" then 
						counter := std_logic_vector(unsigned(counter) +1);
						state <= "10";
					elsif input = "01" then 
						counter := std_logic_vector(unsigned(counter) -1);
						state <= "01";
					else 
						--
					end if;
				when "10"   =>
					if input = "00" then  
						counter := std_logic_vector(unsigned(counter) +1);
						state <= "00";
					elsif input = "11" then 
						counter := std_logic_vector(unsigned(counter) -1);
						state <= "11";
					else 
						--
					end if;
				when others =>
			end case;
			pos <= counter(15 downto 0);
		end if; --rising_edge(clk)
	end process;

end Behavioral;

