--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   18:03:19 05/22/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/pantilt/PanTilt-Project/fpga/velocity_test_secant.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: dxdt
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
 
ENTITY velocity_test_secant IS
END velocity_test_secant;
 
ARCHITECTURE behavior OF velocity_test_secant IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT dxdt
    PORT(
         value : IN  std_logic_vector(15 downto 0);
         clk : IN  std_logic;
         recip_vel : OUT  std_logic_vector(15 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal value : std_logic_vector(15 downto 0) := (others => '0');
   signal clk : std_logic := '0';

 	--Outputs
   signal recip_vel : std_logic_vector(15 downto 0);
	signal position_inc  : std_logic_vector(15 downto 0):= (others =>'0');
	
   -- Clock period definitions
   constant clk_period : time := 10 ns;
	constant accelleration : time := 1 us;
	constant pos_time : time := 1 us;
BEGIN
	
	
	
	
	-- Instantiate the Unit Under Test (UUT)
   uut: dxdt PORT MAP (
          value => value,
          clk => clk,
          recip_vel => recip_vel
        );

	input_process : process
	begin
		wait for pos_time;
		value <= value + position_inc;
	end process;

	accel_process : process
	begin 
		wait for accelleration;
		position_inc <= position_inc +1;
	end process;

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
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
