--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   16:03:11 05/22/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/pantilt/PanTilt-Project/fpga/Pwmtester.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: PWMv1
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
 
ENTITY Pwmtester IS
END Pwmtester;
 
ARCHITECTURE behavior OF Pwmtester IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT PWMv1
    PORT(
         Clk : IN  std_logic;
         duty_cycle : IN  std_logic_vector(14 downto 0);
         output : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal Clk : std_logic := '0';
   signal duty_cycle : std_logic_vector(14 downto 0) := (others => '0');

 	--Outputs
   signal output : std_logic;

   -- Clock period definitions
   constant Clk_period : time := 10 ns;
	constant duty_increment : time := 20 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: PWMv1 PORT MAP (
          Clk => Clk,
          duty_cycle => duty_cycle,
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
 
	test_process :process
	begin
		duty_cycle <= duty_cycle +1;
		wait for duty_increment;
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
