----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:39:20 05/01/2012 
-- Design Name: 
-- Module Name:    top - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity top is
	Port (
		buffer_collision : out std_logic;
		board_clk: in std_logic;
		spi_clk: in std_logic;
		spi_ss: in std_logic;
		spi_mosi: in std_logic;
		spi_miso: out std_logic;
		
		--Motor related
		JA : out std_logic_vector(7 downto 0);
      JC : in  STD_LOGIC_VECTOR(7 DOWNTO 0);
		Segm : out STD_LOGIC_VECTOR(6 downto 0);					
		An   : out STD_LOGIC_VECTOR(3 downto 0);
		Leds : out STD_LOGIC_VECTOR(5 downto 0));
end top;

architecture Behavioral of top is
 component Motor_memory_interface is
     Port (JA : out STD_LOGIC_VECTOR(7 downto 0);
			
			 JC : in  STD_LOGIC_VECTOR(7 DOWNTO 0);
			 Clk: in STD_LOGIC;
			 
			 --ram
			 mem_clk: out STD_LOGIC;
			 we: out STD_LOGIC_VECTOR(0 downto 0);
			 addr: out STD_LOGIC_VECTOR(4 downto 0);
			 din: out STD_LOGIC_VECTOR(7 downto 0);
			 dout: in STD_LOGIC_VECTOR(7 downto 0);
			 Segm : out STD_LOGIC_VECTOR(6 downto 0);					
			 An   : out STD_LOGIC_VECTOR(3 downto 0);
			 Leds : out STD_LOGIC_VECTOR(5 downto 0));
 end component;



	component spi_serializer
	Port (
		spi_clk: in std_logic;
		spi_ss: in std_logic;
		spi_mosi: in std_logic;
		spi_miso: out std_logic := '0';
		reset: in std_logic;
		clk: in std_logic;
		

		-- Parallel data out
		parallel_data_out: out std_logic_vector(7 downto 0);
		parallel_data_out_valid: out std_logic := '0';
		parallel_data_out_collision: out std_logic := '0'; -- If the does not get emptied there will be a collision
		parallel_data_out_ack: in std_logic := '0'; -- ack that the data is read

		-- Parallel data in
		parallel_data_in: in std_logic_vector(7 downto 0);
		parallel_data_in_valid: in std_logic := '0';
		parallel_data_in_ack: out std_logic := '0' -- ack that the data is read
	);
	end component;
	
--	component spi_ledswitch_test
--	Port (
--		clk: in std_logic;
--		spi_data_in: in std_logic_vector (7 downto 0);
--		spi_data_in_ready: in std_logic;
--		spi_data_in_ack: out std_logic := '0';
--		spi_data_out: out std_logic_vector (7 downto 0);
--		spi_data_out_ready: out std_logic;
--		spi_data_out_ack: in std_logic;
--		leds: out std_logic_vector (7 downto 0);
--		switches: in std_logic_vector (7 downto 0)
--	);
--	end component;
	
	COMPONENT memory
	PORT (
		clka : IN STD_LOGIC;
		ena : IN STD_LOGIC;
		wea : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
		addra : IN STD_LOGIC_VECTOR(4 DOWNTO 0);
		dina : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		douta : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
		clkb : IN STD_LOGIC;
		web : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
		addrb : IN STD_LOGIC_VECTOR(4 DOWNTO 0);
		dinb : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		doutb : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
	);
	END COMPONENT;
	
	component spi_protocol_controller
	Port (
		clk: in std_logic;
		mem_en : out STD_LOGIC;
		mem_addr : out STD_LOGIC_VECTOR(4 DOWNTO 0);
		mem_we : out STD_LOGIC_VECTOR(0 DOWNTO 0);
		from_spi: in std_logic_vector(7 downto 0);
		from_spi_valid: in std_logic := '0';
		from_spi_ack: out std_logic := '0'; -- ack that the data is read

		to_spi_valid: out std_logic := '0';
		to_spi_ack: in std_logic := '0' -- ack that the data is read
	);
	end component;
	
	signal from_spi, to_spi: std_logic_vector (7 downto 0) := "00000000";
	signal from_spi_valid, from_spi_ack, to_spi_valid, to_spi_ack: std_logic := '0';
	
	signal mem_ena, mem_clkb: std_logic := '0';
	signal mem_wea, mem_web: STD_LOGIC_VECTOR(0 DOWNTO 0) := "0";
	signal mem_addra, mem_addrb: STD_LOGIC_VECTOR(4 DOWNTO 0) := "00000";
	signal mem_dinb, mem_doutb: STD_LOGIC_VECTOR(7 DOWNTO 0) := "00000000";
begin
	
	spi_serializer0 : spi_serializer port map (
			spi_clk => spi_clk,
			spi_ss => spi_ss,
			spi_mosi => spi_mosi,
			spi_miso => spi_miso,
			reset => '0',
			clk => board_clk,
			parallel_data_out => from_spi,
			parallel_data_out_valid => from_spi_valid,
			parallel_data_out_collision => buffer_collision,
			parallel_data_out_ack => from_spi_ack,
			parallel_data_in => to_spi,
			parallel_data_in_valid => to_spi_valid,
			parallel_data_in_ack => to_spi_ack
	);
	memory0 : memory port map (
			clka => board_clk,
			ena => mem_ena,
			wea => mem_wea,
			addra => mem_addra,
			dina => from_spi,
			douta => to_spi,
			
			clkb => mem_clkb,
			web => mem_web,
			addrb => mem_addrb,
			dinb => mem_dinb,
			doutb => mem_doutb
			
	);
	spi_protocol_controller0 : spi_protocol_controller port map (
			clk => board_clk,
			mem_en => mem_ena,
			mem_addr => mem_addra,
			mem_we => mem_wea,
			from_spi => from_spi,
			from_spi_valid => from_spi_valid,
			from_spi_ack => from_spi_ack,
			to_spi_valid => to_spi_valid,
			to_spi_ack => to_spi_ack
	);
	
	MMI : Motor_memory_interface port map (
			 JA => JA,
			 JC=>JC,
			 Clk  => board_clk,
			 
			 --ram
			 mem_clk  => mem_clkb,
			 we   => mem_web,
			 addr => mem_addrb,
			 din  => mem_dinb,
			 dout => mem_doutb,
			 Segm => Segm,				
			 An=>An,
			 Leds=>Leds);
--	spi_ledswitch_test0 : spi_ledswitch_test port map (
--			clk => board_clk,
--			spi_data_in => spi_data_in,
--			spi_data_in_ready => spi_data_in_ready,
--			spi_data_in_ack => spi_data_in_ack,
--			spi_data_out => spi_data_out,
--			spi_data_out_ready => spi_data_out_ready,
--			spi_data_out_ack => spi_data_out_ack,
--			leds => leds,
--			switches => switches
--	);

end Behavioral;

