----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    13:01:47 05/15/2012 
-- Design Name: 
-- Module Name:    dxdt - Behavioral 
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

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity dxdt is
    Port ( value     : in   STD_LOGIC_VECTOR(15 downto 0);
           clk 	   : in   STD_LOGIC;
           recip_vel : out  STD_LOGIC_VECTOR(15 downto 0));
			  --vel_sign  : out  STD_LOGIC_VECTOR(15 downto 0));
end dxdt;

architecture Behavioral of dxdt is
constant extraBits				 : integer := 4;
constant time_out 				 : STD_LOGIC_VECTOR (14+extrabits downto 0) := "1111111111111111100";

--debug 
constant output_downsample		 : STD_LOGIC_VECTOR (20 downto 0) := "111111111111111111111";

begin
	process(clk,value)
      variable temp 				 : STD_LOGIC_VECTOR (15 downto 0);
	   variable time_counter    : STD_LOGIC_VECTOR (14+extraBits downto 0);
		variable last_time       : STD_LOGIC_VECTOR (14+extraBits downto 0);
		variable last_value      : STD_LOGIC_VECTOR (15 downto 0);
		variable sign  			 : STD_LOGIC;
		variable timed_out 		 : STD_LOGIC;
		
		--debug
		variable downsample_counter : STD_LOGIC_VECTOR(20 downto 0) := (others => '0');
	begin
		if rising_edge(clk) then
			if (time_counter  = time_out) then
				timed_out := '1';
			else
				time_counter := time_counter + 1;
				timed_out := '0';
			end if;
			
			if(value /= last_value) then
					--determine sign
					if (value < last_value) then
						sign := '1';
					else
						sign := '0';
					end if;					
					last_time       :=time_counter;
					last_value		 := value;
					time_counter    := (0 => '1', others => '0');
			end if; --value != last_value

			--debug
			downsample_counter := downsample_counter  +1;
			if(downsample_counter = output_downsample) then
			
			if(timed_out = '0') then
				--Signed conversion
				if (sign = '0') then
					recip_vel(15) <= '0';
					recip_vel(14 downto 0) <= last_time(14+extraBits downto 0+extraBits);
				else
					temp(15) := '0';
					temp(14 downto 0)      := last_time(14+extraBits downto 0+extraBits);
					recip_vel <= (not temp)+1;
				end if;
			else
				recip_vel <= (others => '0');
			end if;
	
				downsample_counter := (others => '0');
			end if;
			--end debug

	
		end if;--Rising_edge(clk)
	end process;
end Behavioral;

