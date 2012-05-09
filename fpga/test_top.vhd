--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   11:21:48 05/03/2012
-- Design Name:   
-- Module Name:   C:/Users/RHA/xilinxworkspace/spi_client/test_top.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: top
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
 
ENTITY test_top IS
END test_top;
 
ARCHITECTURE behavior OF test_top IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT top
    Port (
		leds: out std_logic_vector(7 downto 0);
		board_clk: in std_logic;
		spi_clk: in std_logic;
		spi_ss: in std_logic;
		spi_mosi: in std_logic;
		spi_miso: out std_logic
	);
    END COMPONENT;
    

   --Inputs
   signal board_clk : std_logic := '0';
   signal spi_clk : std_logic := '0';
	signal spi_ss : std_logic := '0';
   signal spi_mosi : std_logic := '0';

 	--Outputs
   signal spi_miso : std_logic;

   -- Clock period definitions
   constant board_clk_period : time := 0.002 ns;
   constant spi_clk_period : time := 10 ns;
 
BEGIN
 
	-- Instantiate the Unit Under Test (UUT)
   uut: top PORT MAP (
          board_clk => board_clk,
          spi_clk => spi_clk,
			 spi_ss => spi_ss,
          spi_mosi => spi_mosi,
          spi_miso => spi_miso
        );

   -- Clock process definitions
   board_clk_process :process
   begin
		board_clk <= '0';
		wait for board_clk_period/2;
		board_clk <= '1';
		wait for board_clk_period/2;
   end process;
 
   spi_clk_process :process
   begin
		spi_clk <= '0';
		wait for spi_clk_period/2;
		spi_clk <= '1';
		wait for spi_clk_period/2;
   end process;
	
	spi_ss_process :process
	begin
		spi_ss <= '1';
		wait for spi_clk_period*10;
		spi_ss <= '0';
		wait for spi_clk_period*1000;
	end process;
 
	-- Data
	data_in : process 
		variable pos: integer := 0;
		--variable data_array0: std_logic_vector (0 to 31) := "10000001010101010000000100000000"; -- write 01010101 to addr 1000 0001
		variable data_array0: std_logic_vector (0 to 47) := "100000011000000100000001000000010000000100000001";
	begin
		wait for spi_clk_period/4;
		spi_mosi <= data_array0(pos);
		if pos = 47 then
			pos := 0;
		else
			pos := pos +1;
		end if;
		
		wait for 3*spi_clk_period/4;
	end process;

END;
