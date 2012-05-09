----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:38:18 04/06/2012 
-- Design Name: 
-- Module Name:    prescaler - Behavioral 
-- Project Name: 
-- Target Devices: 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use IEEE.NUMERIC_STD.ALL;      -- for UNSIGNED

entity prescaler is
generic ( bitcount : integer := 8);
    Port ( Value  : in   STD_LOGIC_VECTOR(bitcount-1 downto 0);
           Clk    : in   STD_LOGIC;
           Output : out  STD_LOGIC);
end prescaler;

architecture Behavioral of prescaler is

begin

process (Value,Clk)
	variable counter : STD_LOGIC_VECTOR(bitcount-1 downto 0) := (others => '0');
	variable outputV : STD_LOGIC;
begin
	if rising_edge(Clk) then
		if ((counter = Value) or (counter > Value)) then
			counter := (others => '0');
		else
			counter := counter +1;
		end if; -- counter >= value
		if (counter = 0)then
			outputV := '1';
		else
			outputV := '0';
		end if;
	end if; -- Clk
	Output <= outputV;
end process;

end Behavioral;

