--TODO: Rename to binaryTo7Seg
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


--A simple binary decoder, outputs 7seg numbers
entity binaryToBCD is
    Port ( binary: in  STD_LOGIC_VECTOR(3 downto 0);
           output : out  STD_LOGIC_VECTOR(6 downto 0));
			  
end binaryToBCD;

architecture Behavioral of binaryToBCD is
	
begin
	with binary select
	output <= 	"1000000" When "0000",
					"1111001" When "0001",
					"0100100" When "0010",
					"0110000" When "0011",
					"0011001" When "0100",
					"0010010" When "0101",
					"0000010" When "0110",
					"1111000" When "0111",
					"0000000" When "1000",
					"0010000" When "1001",
					"0001000" When "1010",
					"0000011" When "1011",
					"1000110" When "1100",
					"0100001" When "1101",
					"0000110" When "1110",
					"0001110" When "1111",					
					"0000000" When others;
end Behavioral;

