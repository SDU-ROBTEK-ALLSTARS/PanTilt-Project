library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity PWMv1 is
   -- generic (
	--		  prescaler_value : STD_LOGIC_VECTOR(15 downto 0) := (others => '0'); --Increase pwm period
	--		  bit_reduction   : integer := 0);												 --Decrease pwm period
    Port ( 
			  Clk 			   : in   STD_LOGIC;
           duty_cycle   	: in   STD_LOGIC_VECTOR(14 downto 0);
           output 			: out  STD_LOGIC);
end PWMv1;




architecture Behavioral of PWMv1 is
	component prescaler is
		generic ( bitcount   : integer := 8);
		Port    ( Value 	   : in  STD_LOGIC_VECTOR;
					 Clk        : in  STD_LOGIC;
                Output     : out  STD_LOGIC);
	end component;
	signal scaled_clock 	   : STD_LOGIC;
	constant prescaler_value : STD_LOGIC_VECTOR ( 15 downto 0) := (others => '0');
	constant bit_reduction 	 : integer := 4;
	
begin
	--Input scaling
	U1: prescaler
	generic map (bitcount   => 16)
	port map    (Value   => prescaler_value,
				    Output => scaled_clock,
					 Clk => Clk);

	process(Clk,duty_cycle,scaled_clock)
												--14 - bitreduction
		variable counter : STD_LOGIC_VECTOR (14-bit_reduction downto 0):= ( 0 => '0', others => '1');
		variable outputV : STD_LOGIC;

	begin 
		if rising_edge(Clk) and (scaled_clock = '1')  then
			if (counter = 0) then
				counter := ( 0 => '0', others => '1');
			else
				counter := counter - 1;
			end if; -- counter = 0
											  -- 0 erstattet med bit reduction
			if (duty_cycle(14 downto bit_reduction) > counter) then
				outputV := '1';
			else
				outputV := '0';
			end if; -- duty_cycle > counter	
		end if; --Clk
		output <= outputV;
	end process;
end Behavioral;

