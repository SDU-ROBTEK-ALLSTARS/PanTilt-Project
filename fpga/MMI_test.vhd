--------------------------------------------------------------------------------
-- Company: 
-- Engineer:
--
-- Create Date:   16:27:06 05/25/2012
-- Design Name:   
-- Module Name:   C:/Users/dellux09/pantilt/PanTilt-Project/fpga/MMI_test.vhd
-- Project Name:  spi_client
-- Target Device:  
-- Tool versions:  
-- Description:   
-- 
-- VHDL Test Bench Created by ISE for module: Motor_memory_interface
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
 
ENTITY MMI_test IS
END MMI_test;
 
ARCHITECTURE behavior OF MMI_test IS 
 
    -- Component Declaration for the Unit Under Test (UUT)
 		  
COMPONENT testmem
  PORT (
    clka : IN STD_LOGIC;
    ena : IN STD_LOGIC;
    wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addra : IN STD_LOGIC_VECTOR(4 DOWNTO 0);
    dina : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    douta : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    clkb : IN STD_LOGIC;
    enb : IN STD_LOGIC;
    web : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    addrb : IN STD_LOGIC_VECTOR(4 DOWNTO 0);
    dinb : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    doutb : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
  );
END COMPONENT;

    COMPONENT Motor_memory_interface
    PORT(
         JA : OUT  std_logic_vector(7 downto 0);
         JC : IN  std_logic_vector(7 downto 0);
         Clk : IN  std_logic;
         mem_clk : OUT  std_logic;
         we : OUT  std_logic_vector(0 downto 0);
         addr : OUT  std_logic_vector(4 downto 0);
         din : OUT  std_logic_vector(7 downto 0);
         dout : IN  std_logic_vector(7 downto 0);
         Segm : OUT  std_logic_vector(6 downto 0);
         An : OUT  std_logic_vector(3 downto 0);
         Leds : OUT  std_logic_vector(6 downto 0)
        );
    END COMPONENT;
    

   --Inputs
   signal JC : std_logic_vector(7 downto 0);
   signal Clk : std_logic := '0';
   signal dout : std_logic_vector(7 downto 0);

 	--Outputs
   signal JA : std_logic_vector(7 downto 0);
   signal mem_clk : std_logic;
   signal we : std_logic_vector(0 downto 0);
   signal addr : std_logic_vector(4 downto 0);
   signal din : std_logic_vector(7 downto 0);
   signal Segm : std_logic_vector(6 downto 0);
   signal An : std_logic_vector(3 downto 0);
   signal Leds : std_logic_vector(6 downto 0);

   -- Clock period definitions
   constant Clk_period : time := 10 ns;
   constant mem_clk_period : time := 10 ns;

   constant input_period : time := 1000 ns;
BEGIN
 mem_inst : testmem
  PORT MAP (
    clka => '0',
    ena => '0',
    wea => (others => '0'),
    addra => (others =>'0'),
    dina => (others =>'0'),
    douta => open,
    clkb => CLK,
    web => we,
    addrb => addr,
    dinb => din,
    doutb => dout,
	 enb => '1'
  );
	-- Instantiate the Unit Under Test (UUT)
   uut: Motor_memory_interface PORT MAP (
          JA => JA,
          JC => JC,
          Clk => Clk,
          mem_clk => mem_clk,
          we => we,
          addr => addr,
          din => din,
          dout => dout,
          Segm => Segm,
          An => An,
          Leds => Leds
        );

   -- Clock process definitions
   Clk_process :process
   begin
		Clk <= '0';
		wait for Clk_period/2;
		Clk <= '1';
		wait for Clk_period/2;
   end process;
 
   mem_clk_process :process
   begin
		mem_clk <= '0';
		wait for mem_clk_period/2;
		mem_clk <= '1';
		wait for mem_clk_period/2;
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
	
	input_process: process
	begin
		JC(2) <= '0';
		JC(5) <= '0';
		wait for input_period;
		JC(1) <= '0';
		JC(6) <= '0';
		wait for input_period;
		JC(2) <= '1';
		JC(5) <= '1';
		wait for input_period;
		JC(1) <= '1';
		JC(6) <= '1';
		wait for input_period;
	end process;
	
	input_process_2:process
	begin
		JC(0) <= '0';
		JC(4) <= '0';
		wait for input_period *1000;
		JC(0) <= '1';
		JC(4) <= '1';
		wait for input_period * 1000;
	end process;
	
END;
