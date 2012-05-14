library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity hysteresis is
    Port ( input : in  STD_LOGIC;
           Clk : in  STD_LOGIC;
           output : out  STD_LOGIC);
end hysteresis;

architecture Behavioral of hysteresis is
	signal output_signal : STD_LOGIC;
begin
	output <= output_signal;
	process(input, Clk, output_signal)
		variable hysteresis : STD_LOGIC_VECTOR (4 downto 0):="00000";
		variable output_state : STD_LOGIC;
	begin
		if rising_edge(Clk) then 
			--Increase/decrease counter
			if(input = '1') and hysteresis < 20 then
				hysteresis := hysteresis +1;
			else --input = '0'
				if hysteresis > 0 then
				hysteresis := hysteresis -1;
				end if;
			end if;  
			if output_state = '1' and hysteresis < 4 then
				output_state := '0';
			end if;
			if output_state = '0' and hysteresis > 16 then
				output_state := '1';
			end if;
		end if;--clk
		output_signal <= output_state;
	end process;
end Behavioral;