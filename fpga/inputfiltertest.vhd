--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   17:14:44 05/22/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/pantilt/PanTilt-Project/fpga/inputfiltertest.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: hysteresis
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
 
ENTITY inputfiltertest IS
END inputfiltertest;
 
ARCHITECTURE behavior OF inputfiltertest IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT hysteresis
    PORT(
         input : IN  std_logic;
         Clk : IN  std_logic;
         output : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal input : std_logic := '0';
   signal Clk : std_logic := '0';

 	--Outputs
   signal output : std_logic;
	signal noise :  std_logic;
   -- Clock period definitions
   constant Clk_period : time := 10 ns;
	constant inputchange: time := 10 us;
	constant noise_frequency : time := 20 ns;
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: hysteresis PORT MAP (
          input => input,
          Clk => Clk,
          output => output
        );

   -- Clock process definitions
   Clk_process :process
   begin
		Clk <= '0';
		wait for Clk_period/2;
		Clk <= '1';
		wait for Clk_period/2;
   end process;
	
	noise_process :process
   begin
		noise <= '0';
		wait for noise_frequency;
		noise <= '1';
		wait for noise_frequency;
   end process;
	
 
	input_process : process
	begin
		wait for inputchange;
		input <= '1' xor noise;
		wait for inputchange;
		input <= '0' xor noise;
	end process;

   -- Stimulus process
   stim_proc: process
   begin		
      -- hold reset state for 100 ns.
      wait for 100 ns;	

      wait for Clk_period*10;

      -- insert stimulus here 

      wait;
   end process;

END;
