----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 21.09.2021 18:27:03
-- Design Name: 
-- Module Name: d_test - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
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
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity d_test is
--  Port ( );
end d_test;

architecture Behavioral of d_test is
component d 

port (
        set: in STD_LOGIC;
        d_in: in STD_LOGIC;
        c: in STD_LOGIC;
        reset: in STD_LOGIC;
        q: out STD_LOGIC;
        not_q: out STD_LOGIC
    );
    
end component;

        signal set: STD_LOGIC := '1';
        signal d_in: STD_LOGIC := '1';
        signal c: STD_LOGIC := '0';
        signal reset: STD_LOGIC := '1';
        signal q: STD_LOGIC;
        signal not_q: STD_LOGIC;
        
        constant clkhalf : time := 10 ns;

begin

c <= not c after clkhalf;

mapping: d port map(
    d_in => d_in,
    c => c,
    set => set,
    reset => reset,
    q => q,
    not_q => not_q
  );
  
  process 
  begin
  
      wait for 20 ns;
      
      reset <= '0';
      
      wait for 20 ns;
      
      reset <= '1';
      
      wait for 20 ns;
      
      d_in <= '0';
      
      wait for 20 ns;
      
      set <= '0';
  
      wait for 20 ns;
      
      set <= '1';
      
      wait for 20 ns;
      
      d_in <= '1';
      
      wait for 20 ns;
      
      d_in <= '0';
      
      wait for 100 ns;
  
  end process;


end Behavioral;
