----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09.09.2021 16:28:42
-- Design Name: 
-- Module Name: test - Behavioral
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
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity test is
--  Port ( );
end test;

architecture Behavioral of test is
  component lab1
  port (
          A: in std_logic_vector(4 downto 1);
          B: in std_logic_vector(4 downto 1);
          OE: in STD_LOGIC;
          AB: in STD_LOGIC;
          Y: out std_logic_vector(4 downto 1)
      );
end component;

        signal A: std_logic_vector(4 downto 1);
        signal B: std_logic_vector(4 downto 1);
        signal OE: STD_LOGIC;
        signal AB: STD_LOGIC;
        signal Y: std_logic_vector(4 downto 1);

begin

 mapping: lab1 port map (
    B => B,
    OE => OE,
    AB => AB,
    Y => Y,
    A => A
  );

  test_case: process
  begin
    for i in std_logic range '0' to '1' loop
        OE <= i;
        for j in std_logic range '0' to '1' loop
            AB <= j;
            for n in 0 to 15 loop
                A <= std_logic_vector(conv_unsigned(n, 4));
                for k in 0 to 15 loop          
                    B <= std_logic_vector(conv_unsigned(k, 4));
                    wait for 5ns;
                end loop;
            end loop;
        end loop;
    end loop;
  end process;


end Behavioral;
