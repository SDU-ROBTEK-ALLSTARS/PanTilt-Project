----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    10:12:46 05/11/2012 
-- Design Name: 
-- Module Name:    decoder_a_normalizer - Behavioral 
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

entity decoder_a_normalizer is
    Port ( value : in  STD_LOGIC_VECTOR(15 downto 0);
           clk : in  STD_LOGIC;
           index : in  STD_LOGIC;
			  reset_counter : out  STD_LOGIC;
           normalized_value : out  STD_LOGIC_VECTOR(15 downto 0));
end decoder_a_normalizer;

architecture Behavioral of decoder_a_normalizer is

begin
	--Dummy... for now
	normalized_value <= value;
	reset_counter <= '0';
end Behavioral;

