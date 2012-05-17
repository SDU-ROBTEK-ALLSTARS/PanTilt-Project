--TODO: Rename file to largebinaryTo7Seg

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

--This module takes a clock signal (50 mhz) and a binary number (16 bit) 
--and outputs it via Segm and An (7 seg display, with 4 numbers)
entity largebinaryToBCD is
    Port ( largeBinary   : in  STD_LOGIC_VECTOR(15 downto 0);
			  Clk           : in  STD_LOGIC;
			  Segm          : out STD_LOGIC_VECTOR(6 downto 0);					
			  An            : out STD_LOGIC_VECTOR(3 downto 0)); 
end largebinaryToBCD;

architecture Behavioral of largebinaryToBCD is

	--sub components
	--Binary (8 bit input) and outputs to 7seg display
	component binaryToBCD
	Port ( binary: in  STD_LOGIC_VECTOR(3 downto 0);
           output : out  STD_LOGIC_VECTOR(6 downto 0));
	end component;
	
	--4 bit multiplexer
	component bit4mux
	port (input: in STD_LOGIC_VECTOR (1 downto 0 );
		   output: out STD_LOGIC_VECTOR(3 downto 0);
			enable: in STD_LOGIC);
	end component;	

	--signals
	signal displaynumber     : STD_LOGIC_VECTOR (3 downto 0); -- The current number being sent to the 7seg
	signal muxIN				 : STD_LOGIC_VECTOR (1 downto 0); -- Input to the multiplexer.
   signal enable_display	 : STD_LOGIC;							 -- Whether the display is blank (changing anode) og active
	signal negated_anodes	 : STD_LOGIC_VECTOR (3 downto 0); -- The anodes are active low, and therefore negated before output


--This component is a 4 state machine, where the state goes around in a loop.
--State 1: Blank screen waiting a bit
--State 2: Still Blank screen, switching anode and number.
--State 3: waiting a bit again before activating diodes.
--State 4: Activate anodes (most of the time is spent here)
begin
	U1 : binaryToBCD PORT MAP( binary => displaynumber, output => Segm(6 downto 0));
	U2 : bit4mux     PORT MAP(input => muxIN, output => negated_anodes,enable => enable_display);
	An <= not negated_anodes;

	--The clocked state switch
	process(Clk,displaynumber,muxIN,largebinary,enable_display)
		--State variable (as described above)
		variable state 				  : STD_LOGIC_VECTOR(1 downto 0) := "00"; 
		--Waiting incrementing register used in the waiting loops
		variable waittimer 			  : STD_LOGIC_VECTOR(31 downto 0) := "00000000000000000000000000000000";
		--Currently active display
		variable displaycounter 	  : STD_LOGIC_VECTOR (1 downto 0) := "00";  
	
	begin
		if(rising_edge(Clk)) then
			case state is
				
				--Turn off output and wait
				when "00" => 
					enable_display <= '0'; 
					muxIN <= displaycounter;
					--waiting loop
					case waittimer is
						when "00000000000000000000000000010000" =>
							waittimer := "00000000000000000000000000000000";
							state := state+1;
						when others =>	
							waittimer := waittimer+1;
					end case;
					
				--switch output segments + anode display still turned off
				when "01" => 
					displaycounter := displaycounter +1;
					enable_display <= '0';
					muxIN <= displaycounter;

					case displaycounter is
						when "00" => displaynumber<= largebinary(3 downto 0);
						when "01" => displaynumber<= largebinary(7 downto 4);
						when "10" => displaynumber<= largebinary(11 downto 8);
						when "11" => displaynumber<= largebinary(15 downto 12);
						when others => displaynumber <= "0000";						
					end case;
					state := state +1;
				
				--Wait a bit more
				when "10" => 
					enable_display <= '0'; 
					muxIN <= displaycounter;
					case waittimer is
						when "00000000000000000000000000010000" =>
							waittimer := "00000000000000000000000000000000";
							state := state+1;
						when others =>
							waittimer := waittimer+1;
					end case;
					
				--enable display igen
				when others => 
					enable_display <= '1';
					muxIN <= displaycounter;
					case waittimer is
						when "00000000000000000000001000000000" =>
							waittimer := "00000000000000000000000000000000";
							state := state+1;
						when others => 
							waittimer := waittimer+1;
					end case;
			end case;
		end if;
	end process;	
end Behavioral;

