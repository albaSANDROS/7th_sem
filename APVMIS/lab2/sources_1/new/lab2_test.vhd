----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 05.10.2021 17:45:41
-- Design Name: 
-- Module Name: lab2_test - Behavioral
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

entity lab2_test is
--  Port ( );
end lab2_test;

architecture Behavioral of lab2_test is

    component lab2
        port (
        
            load, ud, clock, enp, ent: in std_logic;
            data: in std_logic_vector(4 downto 1);
            rco: out std_logic;
            q: out std_logic_vector(4 downto 1)
        );
    end component;
    
    signal load, enp, ent : STD_LOGIC := '1';
    signal data : std_logic_vector(4 downto 1);
    signal c: STD_LOGIC := '1';
    signal rco, ud: STD_LOGIC;
    signal q: std_logic_vector(4 downto 1);
    constant clk_period : time := 20 ns;
        
begin

   c <= not c after clk_period/2;
    
mapping: lab2 port map(
        load => load,
        clock => c,
        enp => enp,
        ent => ent,
        data => data,
        rco => rco,
        q => q,
        ud => ud
      );
 
      process 
      begin
      
          wait for clk_period/2;
          
          load <= '0';
          data(1) <= '1';
          data(2) <= '0';
          data(3) <= '1';
          data(4) <= '1';
          ud <= '1';
          enp <= '0';
          ent <= '0';
          
          wait for clk_period*1.25;
          
          load <= '1';
          
          wait for clk_period*4.25;
          
          enp <= '1';
          ent <= '1';
          
          wait for clk_period;
          
          ud <= '0';
          
          wait for clk_period;
                    
          enp <= '0';
          ent <= '0';
         
          wait for 100 ns;
      
      end process;      
      

end Behavioral;
