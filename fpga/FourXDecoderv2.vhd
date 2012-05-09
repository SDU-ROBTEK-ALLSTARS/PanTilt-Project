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

use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
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
	signal input   : std_logic_vector(1 downto 0);
	signal state   : std_logic_vector(1 downto 0) := "00";
	signal counter : unsigned (15 downto 0):= "1000000000000000";
	
	component hysteresis is
	    Port ( input  : in  STD_LOGIC;
              Clk    : in  STD_LOGIC;
              output : out  STD_LOGIC);
	end component;
		
begin
	hysteresis_a : hysteresis
		port map(
			input => a,
			clk => clk,
			output => input(0)
			);
			
	hysteresis_b : hysteresis
		port map(
			input => b,
			clk => clk,
			output => input(1)
			);

	pos <= std_logic_vector(counter);
	
	process(a,b,rst_counter,clk,state,input,counter)
	begin
		if rising_edge(clk) then

			--Position reset
			if(rst_counter = '1') then
				counter <="1000000000000000";
			end if; --rst_counter ='1'
			
			--state transition
			case state is 
				when "00"   => 
					if input = "01" then 
						counter <= counter +1;
						state <= "01";
					elsif input = "10" then  
						counter <= counter -1;
						state <= "10";
					else 
						--
					end if;
				when "01"   =>
					if input = "11" then 
						counter <= counter +1;
						state <= "11";
					elsif input = "00" then 
						counter <= counter -1;
						state <= "00";
					else 
						--
					end if;
				when "11"   =>
					if input = "10" then 
						counter <= counter +1;
						state <= "10";
					elsif input = "01" then 
						counter <= counter -1;
						state <= "01";
					else 
						--
					end if;
				when "10"   =>
					if input = "00" then  
						counter <= counter +1;
						state <= "00";
					elsif input = "11" then 
						counter <= counter -1;
						state <= "11";
					else 
						--
					end if;
				when others =>
			end case;
		end if; --rising_edge(clk)
	end process;

end Behavioral;

