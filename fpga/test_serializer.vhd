--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   09:27:04 05/02/2012
-- Design Name:   
-- Module Name:   C:/Users/RHA/xilinxworkspace/spi_client/test_serializer.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: spi_serializer_in
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
 
ENTITY test_serializer IS
END test_serializer;
 
ARCHITECTURE behavior OF test_serializer IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 
    COMPONENT spi_serializer
    PORT(
			spi_clk: in std_logic;
			spi_mosi: in std_logic;
			reset: in std_logic;
			clk: in std_logic;
			
			-- Parallel data out
			parallel_data_out: out std_logic_vector(7 downto 0);
			parallel_data_out_valid: out std_logic := '0';
			parallel_data_out_collision: out std_logic := '0'; -- If the does not get emtied there will be a collision
			parallel_data_out_ack: in std_logic := '0'; -- ack that the data is read
			
			-- Parallel data in
			parallel_data_in: in std_logic_vector(7 downto 0);
			parallel_data_in_valid: in std_logic := '0';
			parallel_data_in_ack: out std_logic := '0' -- ack that the data is read
        );
    END COMPONENT;
    

   --Inputs
   signal spi_clk : std_logic := '0';
   signal spi_mosi : std_logic := '0';
   signal reset : std_logic := '0';
   signal clk : std_logic := '0';

 	--Outputs
   signal parallel_data_out : std_logic_vector(7 downto 0);
   signal parallel_data_out_valid : std_logic := '0';
	signal parallel_data_out_collision: std_logic; -- If the does not get emtied there will be a collision
	signal parallel_data_out_ack: std_logic := '0'; -- ack that the data is read
			-- Parallel data in
	signal parallel_data_in: std_logic_vector(7 downto 0) := (others => 'X');
	signal parallel_data_in_valid: std_logic := '0';
	signal parallel_data_in_ack: std_logic := '0'; -- ack that the data is read
	
   -- Clock period definitions
   constant spi_clk_period : time := 10 ns;
   constant clk_period : time := 1 ns;
 
BEGIN
 
 
	-- Instantiate the Unit Under Test (UUT)
   uut: spi_serializer PORT MAP (
          spi_clk => spi_clk,
          spi_mosi =>spi_mosi,
          reset => reset,
          clk => clk,
          parallel_data_out => parallel_data_out,
          parallel_data_out_valid => parallel_data_out_valid,
			 parallel_data_out_collision => parallel_data_out_collision,
			 parallel_data_out_ack => parallel_data_out_ack,
			 parallel_data_in => parallel_data_in,
			 parallel_data_in_valid => parallel_data_in_valid,
			 parallel_data_in_ack => parallel_data_in_ack
        );

   -- Clock process definitions
   spi_clk_process :process
   begin
		spi_clk <= '0';
		wait for spi_clk_period/2;
		spi_clk <= '1';
		wait for spi_clk_period/2;
   end process;
 
   clk_process :process
   begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
   end process;
 
 
	-- Data
	data_in : process 
		variable pos: integer := 0;
		variable data_array: std_logic_vector (0 to 15) := "0011001101010101";
	begin
		wait for spi_clk_period/4;
		spi_mosi <= data_array(pos);
		if pos = 15 then
			pos := 0;
		else
			pos := pos +1;
		end if;
		
		wait for 3*spi_clk_period/4;
	end process;

	-- P Data
	pdata : process (clk,parallel_data_out_valid, parallel_data_in_ack)
	begin
--		if rising_edge(parallel_data_out_valid) then
--			if parallel_data_out = "00110011" then
--				parallel_data_in <= "10010010";
--				parallel_data_in_valid <= '1';
--			end if;
--			parallel_data_out_ack <= '1';
--		end if;
--		if rising_edge(parallel_data_in_ack) then
--			parallel_data_in <= "XXXXXXXX";
--			parallel_data_in_valid <= '0';
--		end if;
		if rising_edge(clk) then
			if parallel_data_out_valid = '1' then
				if parallel_data_out = "00110011" then
					parallel_data_in <= "10010010";
					parallel_data_in_valid <= '1';
				end if;
				parallel_data_out_ack <= '1';
			else
				parallel_data_out_ack <= '0';
			end if;
			if parallel_data_in_ack = '1' then
				parallel_data_in_valid <= '0';
				parallel_data_in <= "XXXXXXXX";
			end if;
		end if;
	end process;

END;
