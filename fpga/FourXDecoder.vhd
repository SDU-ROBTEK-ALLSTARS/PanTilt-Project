----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    14:18:59 05/07/2012 
-- Design Name: 
-- Module Name:    FourXDecoder - Behavioral 
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
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity FourXDecoder is
    Port ( a 	: in  STD_LOGIC;
           b 	: in  STD_LOGIC;
		rst_bar 	: in  STD_LOGIC;
			  clk : in  STD_LOGIC;
           pos : out STD_LOGIC_VECTOR( 15 downto 0) );
end FourXDecoder;

architecture Behavioral of FourXDecoder is
	subtype state is std_logic_vector(1 downto 0);
	signal present_state, next_state : state;
	signal 	cnten : STD_LOGIC;
	signal	up :  STD_LOGIC; 
	constant state_p: state := "00";
	constant state_q: state := "01";
	constant state_r: state := "11";
	constant state_s: state := "10";

begin

	state_reg: process (clk)
	begin
		if rising_edge(clk) then
			if ( rst_bar = '0' ) then
				present_state <= (b,a);
			else
				present_state <= next_state;
			end if;
		end if;
	end process;
	
	outputs: process (a, b, present_state)
		variable input : std_logic_vector (1 downto 0);
	begin
		input := (b,a);
		
		case present_state is
		
			when state_p =>
			if input = "01" then up <= '1'; cnten <= '1';
			elsif input = "10" then up <= '0'; cnten <= '1';
			else up <= '-'; cnten <= '0';
			end if;
			
			when state_q =>
			if input = "11" then up <= '1'; cnten <= '1';
			elsif input = "00" then up <= '0'; cnten <= '1';
			else up <= '-'; cnten <= '0';
			end if;
			
			when state_r =>
			if input = "10" then up <= '1'; cnten <= '1';
			elsif input = "01" then up <= '0'; cnten <= '1';
			else up <= '-'; cnten <= '0';
			end if;
			
			when state_s =>
			if input = "00" then up <= '1'; cnten <= '1';
			elsif input = "11" then up <= '0'; cnten <= '1';
			else up <= '-'; cnten <= '0';
			end if;
			
			when others =>
			
		end case;
	end process;
	
	nxt_state: process (a, b, present_state)
		variable input : std_logic_vector (1 downto 0);
	begin 
		input := (b,a);
		
		case present_state is
		
			when state_p =>
			if input = "01" then next_state <= state_q;
			elsif input = "10" then next_state <= state_s;
			else next_state <= state_p;
			end if;

			when state_q =>
			if input = "01" then next_state <= state_r;
			elsif input = "10" then next_state <= state_p;
			else next_state <= state_q;
			end if;

			when state_r =>
			if input = "01" then next_state <= state_s;
			elsif input = "10" then next_state <= state_q;
			else next_state <= state_r;
			end if;

			when state_s =>
			if input = "01" then next_state <= state_p;
			elsif input = "10" then next_state <= state_r;
			else next_state <= state_s;
			end if;		

			when others =>
			
		end case;
	end process;
	
	count_master : process (up, cnten)
		variable counter : STD_LOGIC_VECTOR( 15 downto 0) := "1000000000000000";
	begin
		
		if cnten = '1' then
		
			if up = '1' then
				counter := counter + 1;
			elsif up = '0' then 
				counter := counter - 1;
			end if;
			pos <= counter;
		end if;
	end process;
end Behavioral;

