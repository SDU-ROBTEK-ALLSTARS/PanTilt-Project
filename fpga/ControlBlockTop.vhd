library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity ControlBlockTop is
    Port (   Clk   		  : in  STD_LOGIC;
				 Input_number : in STD_LOGIC_VECTOR(15 downto 0);
				 Freerun        : in STD_LOGIC;
				 Pin1			  : out STD_LOGIC;
				 Pin2			  : out STD_LOGIC;
				 Enable		  : out STD_LOGIC);
end ControlBlockTop;















architecture Behavioral of ControlBlockTop is
--Subcomponent declarations
	component PWMv1 is
		Port (
			  Clk 			   : in   STD_LOGIC;
           duty_cycle   	: in   STD_LOGIC_VECTOR(14 downto 0);
           output 			: out  STD_LOGIC);
	end component;
	
	component AuxControl is
    Port ( Direction : in  STD_LOGIC;
           Freerun 		: in  STD_LOGIC;
           Pwm_in 	: in  STD_LOGIC;
			  Pin2 		: out  STD_LOGIC;
           Pin1 		: out  STD_LOGIC;
           Enable 	: out  STD_LOGIC);
	end component;


	component SignedToMagnitude is
    Port ( Inputnumber 	: in  STD_LOGIC_VECTOR(15 downto 0);
			  Clk				: in STD_LOGIC;
           sign 			: out  STD_LOGIC;
           magnitude 	: out  STD_LOGIC_VECTOR(14 downto 0));
	end component;

--signal declaration
	
	signal duty_cycle   : STD_LOGIC_VECTOR(14 downto 0);
	signal PWM_signal   : STD_LOGIC;
	signal decoded_sign : STD_LOGIC;
	


begin
	--Component instantiation
	Pwm: PWMv1
	port map    (duty_cycle      => duty_cycle,
				    Output 		     => PWM_signal,
					 Clk 				  => Clk);
	
	Aux: AuxControl
	port map    (Direction => decoded_sign,
				    Freerun   => Freerun,
                Pwm_in 	  => PWM_signal,
					 Pin2 	  => Pin2,
					 Pin1 	  => Pin1,
					 Enable 	  => Enable);
	

	Sign: SignedToMagnitude
	port map	(Inputnumber => input_number,
				 Clk			 => Clk,
				 sign 	    => decoded_sign,
             magnitude   => duty_cycle);
	
	--Signal routing

	
	
	
--	process(PWMA_duty,Clk)
--		variable PWMA_duty_v : STD_LOGIC_VECTOR(14 downto 0) := ("011111111111111");
--		variable temp 			: STD_LOGIC_VECTOR(6  downto 0) := ("0000001");--
--	begin
--		if rising_edge(Clk)then
--			PWMA_duty <= PWMA_duty_v;
	--		JA(8 downto 1) <= "11111111";
--			JA(8 downto 2) <= temp;
		
--		end if; --Clk
--	end process;
end Behavioral;

