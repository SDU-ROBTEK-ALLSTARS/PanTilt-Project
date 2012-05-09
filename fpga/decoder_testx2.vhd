--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   17:46:11 05/08/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/Desktop/pantilt/spi_client/decoder_testx2.vhd
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
LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
 
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--USE ieee.numeric_std.ALL;
 
ENTITY decoder_testx2 IS
END decoder_testx2;
 
ARCHITECTURE behavior OF decoder_testx2 IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT FourXDecoder
    PORT(
         a : IN  std_logic;
         b : IN  std_logic;
         rst_bar : IN  std_logic;
         clk : IN  std_logic;
         pos : OUT  std_logic_vector(15 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal a : std_logic := '0';
   signal b : std_logic := '0';
   signal rst_bar : std_logic := '0';
   signal clk : std_logic := '0';

 	--Outputs
   signal pos : std_logic_vector(15 downto 0);

   -- Clock period definitions
   constant clk_period : time := 10 ns;
   constant input_period : time := 1000 ns;
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
	input_process : process
	begin
		b <= '0';
		wait for input_period;
		a <= '0';
		wait for input_period;
		b <= '1';
		wait for input_period;
		a <= '1';
		wait for input_period;
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
