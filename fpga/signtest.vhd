--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   12:34:27 05/08/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/Desktop/pantilt/spi_client/signtest.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: SignedToMagnitude
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
 
ENTITY signtest IS
END signtest;
 
ARCHITECTURE behavior OF signtest IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT SignedToMagnitude
    PORT(
         Inputnumber : IN  std_logic_vector(15 downto 0);
         Clk : IN  std_logic;
         sign : OUT  std_logic;
         magnitude : OUT  std_logic_vector(14 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal Inputnumber : std_logic_vector(15 downto 0) := (others => '0');
   signal Clk : std_logic := '0';

 	--Outputs
   signal sign : std_logic;
   signal magnitude : std_logic_vector(14 downto 0);

   -- Clock period definitions
   constant Clk_period : time := 10 ns;
	constant input_period:time := 50 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: SignedToMagnitude PORT MAP (
          Inputnumber => Inputnumber,
          Clk => Clk,
          sign => sign,
          magnitude => magnitude
        );

   -- Clock process definitions
   Clk_process :process
   begin
		Clk <= '0';
		wait for Clk_period/2;
		Clk <= '1';
		wait for Clk_period/2;
   end process;
	
	input_proc : process
	begin
		wait for input_period;
		inputnumber <= inputnumber+1;
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
