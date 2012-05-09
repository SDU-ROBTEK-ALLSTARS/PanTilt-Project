--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   13:43:13 05/08/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/Desktop/pantilt/spi_client/motor_control_block_test.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: ControlBlockTop
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
 
ENTITY motor_control_block_test IS
END motor_control_block_test;
 
ARCHITECTURE behavior OF motor_control_block_test IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT ControlBlockTop
    PORT(
         Clk : IN  std_logic;
         Input_number : IN  std_logic_vector(15 downto 0);
         Freerun : IN  std_logic;
         Pin1 : OUT  std_logic;
         Pin2 : OUT  std_logic;
         Enable : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal Clk : std_logic := '0';
   signal Input_number : std_logic_vector(15 downto 0) := (15 =>'1',others => '0');
   signal Freerun : std_logic := '0';

 	--Outputs
   signal Pin1 : std_logic;
   signal Pin2 : std_logic;
   signal Enable : std_logic;

   -- Clock period definitions
   constant Clk_period : time := 10 ns;
   constant input_increment_period: time := 3 us;
	constant freerun_period : time :=196605000 ns;

BEGIN
	-- Instantiate the Unit Under Test (UUT)
   uut: ControlBlockTop PORT MAP (
          Clk => Clk,
          Input_number => Input_number,
          Freerun => Freerun,
          Pin1 => Pin1,
          Pin2 => Pin2,
          Enable => Enable);

   -- Clock process definitions
   Clk_process :process
   begin
		Clk <= '0';
		wait for Clk_period/2;
		Clk <= '1';
		wait for Clk_period/2;
   end process;
	
	inputprocess : process
	begin
		input_number <= input_number +1;
		wait for input_increment_period;
	end process;
	
	freerun_swap : process
	begin
		freerun <= '0';
		wait for freerun_period;
		freerun <= '1';
		wait for freerun_period;
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
