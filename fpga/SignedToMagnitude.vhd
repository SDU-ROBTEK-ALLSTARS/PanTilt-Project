----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    18:22:17 05/05/2012 
-- Design Name: 
-- Module Name:    SignedToMagnitude - Behavioral 
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

entity SignedToMagnitude is
    Port ( Inputnumber 	: in  STD_LOGIC_VECTOR(15 downto 0);
			  Clk				: in STD_LOGIC;
           sign 			: out  STD_LOGIC;
           magnitude 	: out  STD_LOGIC_VECTOR(14 downto 0));
end SignedToMagnitude;
-----------NOTE MIMIMUM VALUE IS CLAMPED TO ONE LOWER!!!!
architecture Behavioral of SignedToMagnitude is
	signal positive_mag : STD_LOGIC_VECTOR(14 downto 0);
	signal negative_mag : STD_LOGIC_VECTOR(14 downto 0);
	signal absolute_neg : STD_LOGIC_VECTOR(14 downto 0);
	signal all_zero	  : STD_LOGIC;
begin
  sign<=Inputnumber(15);
  positive_mag <= Inputnumber(14 downto 0) and  (14 downto 0 => (not inputnumber(15)));   
  negative_mag <= ((not Inputnumber(14 downto 0))+1) and (14 downto 0 => inputnumber(15));
  absolute_neg <= (14 downto 0 =>(Inputnumber(15) and all_zero));
  all_zero <= (not(  Inputnumber(0) or
							Inputnumber(1) or 	
							Inputnumber(2) or 
							Inputnumber(3) or 
							Inputnumber(4) or 
							Inputnumber(5) or 
							Inputnumber(6) or 
							Inputnumber(7) or 
							Inputnumber(8) or 
							Inputnumber(9) or 
							Inputnumber(10) or 
							Inputnumber(11) or 
							Inputnumber(12) or 
							Inputnumber(13) or 
							Inputnumber(14)));
										  
   magnitude <= negative_mag or positive_mag or absolute_neg;

--MOCKUP IMPLEMENTATION, DOES NOT WORK WHEN GIVEN A NEGATIVE NUMBER REPLACE LATER
	--sign <= Inputnumber(15);
	--magnitude <= Inputnumber(14 downto 0);
end Behavioral;

