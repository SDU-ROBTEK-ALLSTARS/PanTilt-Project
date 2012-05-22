--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   18:47:18 05/22/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/pantilt/PanTilt-Project/fpga/velocity_test_avg.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: dvdtv2
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
 
ENTITY velocity_test_avg IS
END velocity_test_avg;
 
ARCHITECTURE behavior OF velocity_test_avg IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT dvdtv2
    PORT(
         position : IN  std_logic_vector(15 downto 0);
         clk : IN  std_logic;
         velocity : OUT  std_logic_vector(15 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal position : std_logic_vector(15 downto 0) := (others => '0');
   signal clk : std_logic := '0';

 	--Outputs
   signal velocity : std_logic_vector(15 downto 0);
	signal position_inc  : std_logic_vector(15 downto 0):= (others =>'0');
	
   -- Clock period definitions
   constant clk_period : time := 10 ns;
	constant accelleration : time := 100 ms;
	constant pos_time : time :=  100 ms;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: dvdtv2 PORT MAP (
          position => position,
          clk => clk,
          velocity => velocity
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
		wait for pos_time;
		position <= position + position_inc;
	end process;
	accel_process : process
	begin 
		wait for accelleration;
		position_inc <= position_inc +1;
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
