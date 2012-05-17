----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    13:33:00 05/07/2012 
-- Design Name: 
-- Module Name:    Motor_memory_interface - Behavioral 
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

entity Motor_memory_interface is
    Port (JA : out STD_LOGIC_VECTOR(7 downto 0);
			 JC : in  STD_LOGIC_VECTOR(7 DOWNTO 0);
			 Clk: in STD_LOGIC;
			 
			 --ram 
			 mem_clk: out STD_LOGIC;
			 we: out STD_LOGIC_VECTOR(0 downto 0);
			 addr: out STD_LOGIC_VECTOR(4 downto 0);
			 din: out STD_LOGIC_VECTOR(7 downto 0);
			 dout: in STD_LOGIC_VECTOR(7 downto 0);
			 
			 --DEBUG
          Segm : out STD_LOGIC_VECTOR(6 downto 0);					
			 An   : out STD_LOGIC_VECTOR(3 downto 0);
			 Leds : out STD_LOGIC_VECTOR(6 downto 0));
			 
end Motor_memory_interface;

architecture Behavioral of Motor_memory_interface is
--State machine variables
	type STATE_TYPE is(
	   READ_DUTY_A_MSB_SETUP,
		READ_DUTY_A_MSB_WAIT,
		READ_DUTY_A_MSB,
		
		READ_DUTY_A_LSB_SETUP,
		READ_DUTY_A_LSB_WAIT,
		READ_DUTY_A_LSB,
		
		READ_DUTY_B_MSB_SETUP,
		READ_DUTY_B_MSB_WAIT,
		READ_DUTY_B_MSB,
		
		READ_DUTY_B_LSB_SETUP,
		READ_DUTY_B_LSB_WAIT,
		READ_DUTY_B_LSB,
		
		SET_MOTOR_POS_A_MSB,
		SET_MOTOR_POS_A_MSB_DONE,
		
		SET_MOTOR_POS_A_LSB,
		SET_MOTOR_POS_A_LSB_DONE,
		
		SET_MOTOR_POS_B_MSB,
		SET_MOTOR_POS_B_MSB_DONE,
		
		SET_MOTOR_POS_B_LSB,
		SET_MOTOR_POS_B_LSB_DONE,
		
		SET_MOTOR_VEL_A_MSB,
		SET_MOTOR_VEL_A_MSB_DONE,
		
		SET_MOTOR_VEL_A_LSB,
		SET_MOTOR_VEL_A_LSB_DONE,
		
		SET_MOTOR_VEL_B_MSB,
		SET_MOTOR_VEL_B_MSB_DONE,
		
		SET_MOTOR_VEL_B_LSB,
		SET_MOTOR_VEL_B_LSB_DONE,
		
		
		
		
		
		
		
		READ_AUX_SETUP,
		READ_AUX_WAIT,
		READ_AUX,
		
		WRITE_AUX,
		WRITE_AUX_DONE
		
		);
	signal state : STATE_TYPE := READ_DUTY_A_MSB_SETUP;




--Signals
	signal index_0:     STD_LOGIC;
	signal index_1:     STD_LOGIC;
	signal sensor1_a:   STD_LOGIC;
	signal sensor1_b:   STD_LOGIC;
	signal sensor2_a:   STD_LOGIC;
	signal sensor2_b:   STD_LOGIC;


	signal in1_a :   STD_LOGIC;
	signal in2_a :   STD_LOGIC;
	signal en_a  :   STD_LOGIC;
	signal in1_b :   STD_LOGIC;
	signal in2_b :   STD_LOGIC;
	signal en_b  :   STD_LOGIC;
	
	signal motor_a_input   : STD_LOGIC_VECTOR (15 downto 0);
	signal motor_a_decoder : STD_LOGIC_VECTOR (15 downto 0);
	signal motor_a_freerun : STD_LOGIC;
	
	signal motor_b_input   : STD_LOGIC_VECTOR (15 downto 0);
	signal motor_b_decoder : STD_LOGIC_VECTOR (15 downto 0);
	signal motor_b_freerun : STD_LOGIC;
	
	signal decoder_reset_a_from_arm : STD_LOGIC;
	signal decoder_reset_b_from_arm : STD_LOGIC;
	signal decoder_reset_a_from_fpga: STD_LOGIC;
	signal decoder_reset_b_from_fpga: STD_LOGIC;
	signal decoder_reset_a          : STD_LOGIC;
	signal decoder_reset_b 			  : STD_LOGIC;
	signal normalized_position_a : STD_LOGIC_VECTOR(15 downto 0);
	signal normalized_position_b : STD_LOGIC_VECTOR(15 downto 0);
	
	signal velocity_a				  : STD_LOGIC_VECTOR(15 downto 0);
	signal velocity_b				  : STD_LOGIC_VECTOR(15 downto 0);
	
	signal watchdog_signal   	  : STD_LOGIC_VECTOR(1 downto 0);
	
	signal alive					  : STD_LOGIC;
	
--Subcomponent descriptors
component ControlBlockTop is
	Port (    Clk   		  : in  STD_LOGIC;
				 Input_number : in STD_LOGIC_VECTOR(15 downto 0);
				 Freerun        : in STD_LOGIC;
				 Pin1			  : out STD_LOGIC;
				 Pin2			  : out STD_LOGIC;
				 Enable		  : out STD_LOGIC);
end component;

COMPONENT FourXDecoderv2
	Port ( a 					: in  STD_LOGIC;
          b 					: in  STD_LOGIC;
			 rst_counter   	: in 	STD_LOGIC;
			 clk 					: in  STD_LOGIC;
          pos 					: out STD_LOGIC_VECTOR( 15 downto 0) );
end component;

component largebinaryToBCD 
	Port ( largeBinary    : in  STD_LOGIC_VECTOR(15 downto 0);
			  Clk           : in  STD_LOGIC;
			  Segm          : out STD_LOGIC_VECTOR(6 downto 0);					
			  An            : out STD_LOGIC_VECTOR(3 downto 0));		  
	end component;
	
component decoder_a_normalizer
	Port (  value            : in  STD_LOGIC_VECTOR(15 downto 0);
           clk              : in  STD_LOGIC;
           index            : in  STD_LOGIC;
			  reset_counter    : out STD_LOGIC;
           normalized_value : out STD_LOGIC_VECTOR(15 downto 0));
end component;

component decoder_b_normalizer
	Port (  value            : in  STD_LOGIC_VECTOR(15 downto 0);
           clk              : in  STD_LOGIC;
           index            : in  STD_LOGIC;
			  reset_counter    : out  STD_LOGIC;
           normalized_value : out  STD_LOGIC_VECTOR(15 downto 0));
end component;

component dxdt 
    Port ( 
			  value     : in   STD_LOGIC_VECTOR(15 downto 0);
           clk 	   : in   STD_LOGIC;
           recip_vel : out  STD_LOGIC_VECTOR(15 downto 0));
end component;

component watchdog
    Port ( input : in  STD_LOGIC_VECTOR(1 downto 0);
           clk : in  STD_LOGIC;
           alive : out  STD_LOGIC);
end component;

--constants
constant ADDRESS_DUTY_A_MSB			 : STD_LOGIC_VECTOR (4 downto 0) := "00010"; --Done
constant ADDRESS_DUTY_A_LSB			 : STD_LOGIC_VECTOR (4 downto 0) := "00011"; --Done
constant ADDRESS_DUTY_B_MSB			 : STD_LOGIC_VECTOR (4 downto 0) := "00100"; --Done
constant ADDRESS_DUTY_B_LSB 			 : STD_LOGIC_VECTOR (4 downto 0) := "00101"; --Done

constant ADDRESS_POS_A_MSB 			 : STD_LOGIC_VECTOR (4 downto 0) := "00110"; --Done
constant ADDRESS_POS_A_LSB 			 : STD_LOGIC_VECTOR (4 downto 0) := "00111"; --Done
constant ADDRESS_POS_B_MSB 			 : STD_LOGIC_VECTOR (4 downto 0) := "01000"; --Done
constant ADDRESS_POS_B_LSB 			 : STD_LOGIC_VECTOR (4 downto 0) := "01001"; --Done

constant ADDRESS_VEL_A_MSB  			 : STD_LOGIC_VECTOR (4 downto 0) := "01010";
constant ADDRESS_VEL_A_LSB  			 : STD_LOGIC_VECTOR (4 downto 0) := "01011";
constant ADDRESS_VEL_B_MSB 			 : STD_LOGIC_VECTOR (4 downto 0) := "01100";
constant ADDRESS_VEL_B_LSB 			 : STD_LOGIC_VECTOR (4 downto 0) := "01101";

constant ADDRESS_AUX_FROM_ARM			 : STD_LOGIC_VECTOR (4 downto 0) := "01110"; --Done
constant ADDRESS_AUX_TO_ARM			 : STD_LOGIC_VECTOR (4 downto 0) := "01111";

--BITS FRA ARM
constant MOTOR_A_FREERUN_POS : integer := 0; --Done and connected
constant MOTOR_B_FREERUN_POS : integer := 1; --Done and connected
constant DECODER_A_RESET_POS : integer := 2; --Done and connected
constant DECODER_B_RESET_POS : integer := 3; --Done and connected
constant WATCH_DOG_A_POS     : integer := 4; --Done
constant WATCH_DOG_B_POS     : integer := 5; --Done
 
--BITS TIL ARM
constant HALL_SENSOR_A_POS   : integer := 0;
constant HALL_SENSOR_B_POS   : integer := 1;


begin
--component instantiation
 Motor_A : ControlBlockTop
	port map ( Clk => Clk,
				  Input_number => motor_a_input,
				  Freerun      =>	motor_a_freerun,
				  Pin1			=>	in1_a,
				  Pin2			=>	in2_a,
				  Enable		   => en_a);

 Motor_B : ControlBlockTop
	port map ( Clk => Clk,
				  Input_number => motor_b_input,
		--		  Input_number => "0111111111111111",
				  Freerun      =>	motor_b_freerun,
				  Pin1			=>	in1_b,
				  Pin2			=>	in2_b,
				  Enable		   => en_b);
 Segment : largebinaryToBCD 
	PORT MAP  (
				   --largeBinary => normalized_position_b, 
	            largeBinary => velocity_b,
					Segm => Segm,
					An => An,
					Clk => Clk);

decoder_a:	FourXDecoderv2
	 Port map ( a => sensor1_a,
					b => sensor1_b,	
					rst_counter => decoder_reset_a,
					clk  => clk,	 	
					pos => motor_a_decoder	
				  );
				  
decoder_a_normalizer_inst : decoder_a_normalizer
	port map (  value => motor_a_decoder,
					clk => clk,
					index => index_1,
					reset_counter => decoder_reset_a_from_fpga,
					normalized_value => normalized_position_a
				);			  
			  
decoder_b:	FourXDecoderv2
	 Port map ( a => sensor2_b,
					b => sensor2_a,	
					rst_counter => decoder_reset_b,
					clk  => clk,		
					pos => motor_b_decoder	
				  );

decoder_b_normalizer_inst : decoder_b_normalizer
	port map (  value => motor_b_decoder,
					clk => clk,
					index => index_0,
					reset_counter => decoder_reset_b_from_fpga,
					normalized_value => normalized_position_b
				);

dpadt : dxdt	
	port map (value     => normalized_position_a,
				 clk       => clk,
				 recip_vel => velocity_a
            );	
dpbdt : dxdt	
	port map (value     => normalized_position_b,
				 clk       => clk,
				 recip_vel => velocity_b
            );	
				
fido : watchdog
    port map ( input => watchdog_signal,
					clk   => clk,
               alive => alive);
--signal routing
	JA(0) <= in1_a;
	JA(1) <= en_a;
	JA(2) <= in2_b;
	JA(3) <= '0'; --Not physically connected
	JA(4) <= in2_a;
	JA(5) <= in1_b;
	JA(6) <= en_b;
	JA(7) <= '0'; --Not physically connected
	mem_clk <= clk;

   index_0   <=JC(0);
	sensor1_b <= JC(1); 
	sensor2_b <= JC(2);
	index_1   <= JC(4);
	sensor1_a <= JC(5);
	sensor2_a <= JC(6);
	
	leds(0)<= sensor1_a;
	leds(1)<= sensor1_b;
	leds(2)<= sensor2_a;
	leds(3)<= sensor2_b;
	leds(4)<= index_0;
	leds(5)<= index_1;
	
	decoder_reset_b <= decoder_reset_b_from_arm or decoder_reset_b_from_fpga;
	decoder_reset_a <= decoder_reset_a_from_arm or decoder_reset_a_from_fpga;
--the rest :D
--memory process here

	process (Clk,dout,velocity_a,velocity_b,normalized_position_a,normalized_position_b,index_0,index_1)
	--Variables
		variable v_we:   STD_LOGIC_VECTOR(0 downto 0); --0 for read, 1 for write
		variable v_addr: STD_LOGIC_VECTOR(4 downto 0);
		variable v_din:  STD_LOGIC_VECTOR(7 downto 0);  --WRITE TO THIS
		variable v_dout: STD_LOGIC_VECTOR(7 downto 0); --READ FROM THIS
		variable v_motor_a_input : STD_LOGIC_VECTOR(15 downto 0);
		variable v_motor_b_input : STD_LOGIC_VECTOR(15 downto 0);
		variable v_motor_a_pos : STD_LOGIC_VECTOR(15 downto 0) := "0000000000000000";
		variable v_motor_b_pos : STD_LOGIC_VECTOR(15 downto 0) := "0000000000000000";
		variable v_motor_a_vel : STD_LOGIC_VECTOR(15 downto 0) := "0000000000000000";
		variable v_motor_b_vel : STD_LOGIC_VECTOR(15 downto 0) := "0000000000000000";
		variable v_aux_in				   : STD_LOGIC_VECTOR(7 downto 0);
		variable v_aux_out				: STD_LOGIC_VECTOR(7 downto 0):="00000000";
	begin
		if rising_edge(Clk) then
		v_dout := dout; --Read input
		v_motor_a_pos := normalized_position_a;
		v_motor_b_pos := normalized_position_b;
		v_motor_a_vel := velocity_a;
		v_motor_b_vel := velocity_b;
		v_aux_out(HALL_SENSOR_A_POS) := index_0;
		v_aux_out(HALL_SENSOR_B_POS) := index_1;
			case state is
			
-----------------------PWM-----------------------------------
----------------- MOTOR A -----------------------------------
				------MSB------
				when READ_DUTY_A_MSB_SETUP =>
					v_we   := "0";
					v_addr := ADDRESS_DUTY_A_MSB;
					state <= READ_DUTY_A_MSB_WAIT;
				when READ_DUTY_A_MSB_WAIT=>
					state <= READ_DUTY_A_MSB;
				when READ_DUTY_A_MSB =>
					v_motor_a_input(15 downto 8) := v_dout;
					state <= READ_DUTY_A_LSB_SETUP;
				
				------LSB------	
				when READ_DUTY_A_LSB_SETUP =>
					v_we	 := "0";
					v_addr := ADDRESS_DUTY_A_LSB;
					state <= READ_DUTY_A_LSB_WAIT;
				when READ_DUTY_A_LSB_WAIT=>
					state <= READ_DUTY_A_LSB;
				when READ_DUTY_A_LSB =>
					v_motor_a_input(7 downto 0) := v_dout;
					state <= READ_DUTY_B_MSB_SETUP;
					
----------------- MOTOR B -----------------------------------					
				------MSB------
				when READ_DUTY_B_MSB_SETUP =>
					v_we   := "0";
					v_addr := ADDRESS_DUTY_B_MSB;
					state <= READ_DUTY_B_MSB_WAIT;
				when READ_DUTY_B_MSB_WAIT =>
					state <= READ_DUTY_B_MSB;
				when READ_DUTY_B_MSB =>
					v_motor_b_input(15 downto 8) := v_dout;
					state <= READ_DUTY_B_LSB_SETUP;
				
				------LSB------
				when READ_DUTY_B_LSB_SETUP =>
					v_we   := "0";
					v_addr := ADDRESS_DUTY_B_LSB; 
					state <= READ_DUTY_B_LSB_WAIT;
				when READ_DUTY_B_LSB_WAIT=>
					state <= READ_DUTY_B_LSB;
				when READ_DUTY_B_LSB =>
					v_motor_b_input(7 downto 0) := v_dout;
					state <= SET_MOTOR_POS_A_MSB;
					
----------------POSITION-------------------------------------
----------------- MOTOR A -----------------------------------
				when SET_MOTOR_POS_A_MSB =>
					v_we   := "1";
					v_addr := ADDRESS_POS_A_MSB;
					v_din  := v_motor_a_pos(15 downto 8);
					state <= SET_MOTOR_POS_A_MSB_DONE;
					
				when SET_MOTOR_POS_A_MSB_DONE =>
					state <= SET_MOTOR_POS_A_LSB;
					
				when SET_MOTOR_POS_A_LSB =>
					v_we   := "1";
					v_addr := ADDRESS_POS_A_LSB;
					v_din  := v_motor_a_pos(7 downto 0);
					state <= SET_MOTOR_POS_A_LSB_DONE;
					
				when SET_MOTOR_POS_A_LSB_DONE =>
					state <= SET_MOTOR_POS_B_MSB;
					
----------------- MOTOR B -----------------------------------
				when SET_MOTOR_POS_B_MSB =>
					v_we   := "1";
					v_addr := ADDRESS_POS_B_MSB;
					v_din  := v_motor_b_pos(15 downto 8);
					state <= SET_MOTOR_POS_B_MSB_DONE;
					
				when SET_MOTOR_POS_B_MSB_DONE =>
					state <= SET_MOTOR_POS_B_LSB;
					
				when SET_MOTOR_POS_B_LSB =>
					v_we   := "1";
					v_addr := ADDRESS_POS_B_LSB;
					v_din  := v_motor_b_pos(7 downto 0);
					state <= SET_MOTOR_POS_B_LSB_DONE;
					
				when SET_MOTOR_POS_B_LSB_DONE =>
					state <= SET_MOTOR_VEL_A_MSB;

----------------- VELOCITY WRITE--------------------------
----------------- MOTOR A ----------------
				when SET_MOTOR_VEL_A_MSB => 
					v_we   := "1";
					v_addr := ADDRESS_VEL_A_MSB;
					v_din  := v_motor_a_vel(15 downto 8);
					state <= SET_MOTOR_VEL_A_MSB_DONE;
				when SET_MOTOR_VEL_A_MSB_DONE => 
					state <= SET_MOTOR_VEL_A_LSB;
				
				when SET_MOTOR_VEL_A_LSB =>
					v_we   := "1";
					v_addr := ADDRESS_VEL_A_LSB;
					v_din  := v_motor_a_vel(7 downto 0);
					state <= SET_MOTOR_VEL_A_LSB_DONE;
				when SET_MOTOR_VEL_A_LSB_DONE => 
					state <= SET_MOTOR_VEL_B_MSB;


----------------- MOTOR B ----------------
--ADDRESS_VEL_B_MSB
--ADDRESS_VEL_B_LSB

--STATES
		
				when SET_MOTOR_VEL_B_MSB => 
					v_we   := "1";
					v_addr := ADDRESS_VEL_B_MSB;
					v_din  := v_motor_b_vel(15 downto 8);
					state <= SET_MOTOR_VEL_B_MSB_DONE;
				when SET_MOTOR_VEL_B_MSB_DONE => 
					state <= SET_MOTOR_VEL_B_LSB;
					
				when SET_MOTOR_VEL_B_LSB => 					
					v_we   := "1";
					v_addr := ADDRESS_VEL_B_LSB;
					v_din  := v_motor_b_vel(7 downto 0);
					state <= SET_MOTOR_VEL_B_LSB_DONE;
				when SET_MOTOR_VEL_B_LSB_DONE => 
					state <= READ_AUX_SETUP;
		

-----------------AUX------------------------------------					
            --------READ--------
 				when READ_AUX_SETUP =>
				   v_we   := "0";
					v_addr := ADDRESS_AUX_FROM_ARM;
					state <= READ_AUX_WAIT;
				when READ_AUX_WAIT=>
					state <= READ_AUX;
				when READ_AUX =>
					v_aux_in := v_dout;
					state <= WRITE_AUX;
            --------WRITE--------
				when WRITE_AUX => 
					v_we   := "1";
					v_addr := ADDRESS_AUX_TO_ARM;
					v_din  := v_aux_out;
					state <=WRITE_AUX_DONE;
				when WRITE_AUX_DONE => 
					state <=READ_DUTY_A_MSB_SETUP;

---------------CLEANUP----------------------------------
				when others =>
					state <= READ_DUTY_A_MSB_SETUP;
			end case;
----------------END STATE MACHINE------------------------
			we   <= v_we;
			addr <= v_addr;
			din  <= v_din;
			motor_a_input <= v_motor_a_input;
			motor_b_input <= v_motor_b_input;
			motor_a_freerun <= v_aux_in(MOTOR_A_FREERUN_POS) and (not alive);
			motor_b_freerun <= v_aux_in(MOTOR_B_FREERUN_POS) and (not alive);
			decoder_reset_a_from_arm <= v_aux_in(DECODER_A_RESET_POS);
			decoder_reset_b_from_arm <= v_aux_in(DECODER_B_RESET_POS);
			watchdog_signal(0) <= v_aux_in(WATCH_DOG_A_POS);
			watchdog_signal(1) <= v_aux_in(WATCH_DOG_B_POS);
			
		end if; --rising_edge(Clk)
	end process;




end Behavioral;

