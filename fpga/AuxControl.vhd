----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    17:59:11 05/05/2012 
-- Design Name: 
-- Module Name:    AuxControl - Behavioral 
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
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity AuxControl is
    Port ( Direction : in  STD_LOGIC;
           Freerun 	: in  STD_LOGIC;
           Pwm_in 	: in  STD_LOGIC;
			  Pin2 		: out  STD_LOGIC;
           Pin1 		: out  STD_LOGIC;
           Enable 	: out  STD_LOGIC);
end AuxControl;

architecture Behavioral of AuxControl is

begin
	Pin1   <= (Pwm_in and Direction) and (not Freerun);
	Pin2   <= (Pwm_in and (not Direction)) and (not Freerun);
	Enable <= (not Freerun);
	
	
	
	
	--MOCKUP IMPLEMENTATION REPLACE LATER
--	Enable <= '1';
--	Pin2 <= '0';
--	Pin1 <= Pwm_in;
end Behavioral;

