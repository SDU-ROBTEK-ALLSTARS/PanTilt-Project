library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

--A simple 4 bit multiplexer, with an extra enable switch. When not enabled all output is low.
entity bit4mux is
    Port ( input  : in  STD_LOGIC_VECTOR(1 downto 0);
			  enable : in 	STD_LOGIC;
           output : out  STD_LOGIC_VECTOR(3 downto 0));
end bit4mux;

architecture Behavioral of bit4mux is
	 signal temp : STD_LOGIC_VECTOR(3 downto 0);
	 
begin
	output(0) <= temp(0) and enable;
	output(1) <= temp(1) and enable;
	output(2) <= temp(2) and enable;
	output(3) <= temp(3) and enable;
	
	with input select
		temp <= "0001" when "00",
					 "0010" when "01",
					 "0100" when "10",
					 "1000" when "11",
					 "0000" when others;
end Behavioral;

