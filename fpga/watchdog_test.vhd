--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   17:45:47 05/22/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/pantilt/PanTilt-Project/fpga/watchdog_test.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: watchdog
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
 
ENTITY watchdog_test IS
END watchdog_test;
 
ARCHITECTURE behavior OF watchdog_test IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT watchdog
    PORT(
         input : IN  std_logic_vector(1 downto 0);
         clk : IN  std_logic;
         alive : OUT  std_logic
        );
    END COMPONENT;
    

   --Inputs
   signal input : std_logic_vector(1 downto 0) := (others => '0');
   signal clk : std_logic := '0';

 	--Outputs
   signal alive : std_logic;
	signal dead  : std_logic;
	
   -- Clock period definitions
   constant clk_period : time := 10 ns;
	constant input_shift:time  := 30 ns;
	constant die_time   : time := 10 ms;
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: watchdog PORT MAP (
          input => input,
          clk => clk,
          alive => alive
        );

   -- Clock process definitions
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 
	life_process:process
	begin
		dead <= '0';
		wait for die_time;
		dead <= '1';
		wait for die_time;
	end process;

	life_process_2 : process
	begin
			input(0)<= '0';
			input(1)<= '0';
			wait for input_shift;
			
			input(0)<= '1' and (not dead);
			input(1)<= '0';
			wait for input_shift;
			
			input(0)<= '1' and (not dead);
			input(1)<= '1' and (not dead);
			wait for input_shift;
			
			input(0)<= '0';
			input(1)<= '1' and (not dead);
			wait for input_shift;
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
