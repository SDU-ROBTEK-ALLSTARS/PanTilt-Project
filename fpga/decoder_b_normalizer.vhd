----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    09:58:37 05/11/2012 
-- Design Name: 
-- Module Name:    decoder_b_normalizer - Behavioral 
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

entity decoder_b_normalizer is
    Port ( value : in  STD_LOGIC_VECTOR(15 downto 0);
           clk : in  STD_LOGIC;
           index : in  STD_LOGIC;
			  reset_counter : out  STD_LOGIC;
           normalized_value : out  STD_LOGIC_VECTOR(15 downto 0));
end decoder_b_normalizer;

architecture Behavioral of decoder_b_normalizer is
	constant bits_pr_rev : integer := 1076;
	constant zero_position : STD_LOGIC_VECTOR(15 downto 0) := "1000000000000000";
begin
	process (clk,value,index)
	begin
		if rising_edge(clk) then
			if	(value = bits_pr_rev+zero_position) then
				reset_counter <= '1';
			elsif (value = zero_position - bits_pr_rev) then
				reset_counter <= '1';
			else 
				reset_counter <= '0';
			end if;
		end if;
		if value >= zero_position then
			normalized_value <= value;
		else
			normalized_value <= value + bits_pr_rev;
		end if;
	end process;
	
end Behavioral;

