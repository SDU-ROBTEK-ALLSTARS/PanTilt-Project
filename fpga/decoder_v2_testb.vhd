--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   20:34:00 05/08/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/Desktop/pantilt/spi_client/decoder_v2_testb.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: FourXDecoderv2
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
 
ENTITY decoder_v2_testb IS
END decoder_v2_testb;
 
ARCHITECTURE behavior OF decoder_v2_testb IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT FourXDecoderv2
    PORT(
         a : IN  std_logic;
         b : IN  std_logic;
         rst_counter : IN  std_logic;
         clk : IN  std_logic;
         pos : OUT  std_logic_vector(15 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal a : std_logic := '0';
   signal b : std_logic := '0';
   signal rst_counter : std_logic := '0';
   signal clk : std_logic := '0';

 	--Outputs
   signal pos : std_logic_vector(15 downto 0);

   -- Clock period definitions
   constant clk_period : time := 10 ns;
    constant input_period : time := 1000 ns;

BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: FourXDecoderv2 PORT MAP (
          a => a,
          b => b,
          rst_counter => rst_counter,
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
 
	input_process: process
	
	begin
		a <= '0';
		wait for input_period;
		b <= '0';
		wait for input_period;
		a <= '1';
		wait for input_period;
		b <= '1';
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
