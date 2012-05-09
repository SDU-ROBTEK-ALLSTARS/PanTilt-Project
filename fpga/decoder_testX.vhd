--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   17:10:00 05/08/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/Desktop/pantilt/spi_client/decoder_testX.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: FourXDecoder
-- 
-- Dependencies:
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
--
-- Notes: 
-- This testbench has been automatically generated using types std_logic and
-- std_logic_vector for the ports of the unit under test.  Xilinx recommends
-- that these types always be used for the top-level I/O of a design in order
-- to guarantee that the testbench will bind correctly to the post-implementation 
-- simulation model.
--------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY decoder_testX IS
END decoder_testX;
 
ARCHITECTURE behavior OF decoder_testX IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT FourXDecoder
    PORT(
         a : IN  std_logic;
         b : IN  std_logic;
         rst_bar : IN  std_logic;
         clk : IN  std_logic;
         pos : OUT signed( 15 downto 0) 
        );
    END COMPONENT;
    

   --Inputs
   signal a : std_logic := '0';
   signal b : std_logic := '0';
   signal rst_bar : std_logic := '1';
   signal clk : std_logic := '0';

 	--Outputs
   signal pos : signed( 15 downto 0) ;
	  -- Clock period definitions
   constant clk_period : time := 10 ns;
   constant turn_period : time := 1 us;
	
BEGIN
	-- Instantiate the Unit Under Test (UUT)
   uut: FourXDecoder PORT MAP (
          a => a,
          b => b,
          rst_bar => rst_bar,
          clk => clk,
          pos => pos
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
	
	turn_process:process
	begin
		a <= '0';
		wait for turn_period/2;
		b<= '0';
		wait for turn_period/2;
		a<= '1';
		wait for turn_period/2;
		b<= '1';
		wait for turn_period/2;
	end process;
   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for clk_period*10;

      -- insert stimulus here 

      wait;
   end process;

END;
