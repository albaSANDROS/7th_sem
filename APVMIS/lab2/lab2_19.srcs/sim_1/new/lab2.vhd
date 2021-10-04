----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 01.10.2021 16:46:42
-- Design Name: 
-- Module Name: lab2 - Behavioral
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

entity lab2 is
port (
        load: in STD_LOGIC;
        uo: in STD_LOGIC;
        clock: in STD_LOGIC;
        enp: in STD_LOGIC;
        ent: in STD_LOGIC;
        data: in std_logic_vector(4 downto 1);
        q: out std_logic_vector(4 downto 1);
        rco: out STD_LOGIC
    );
end lab2;

architecture Behavioral of lab2 is

component d
    port(
        set, d_in, c, reset: in STD_LOGIC;

        q, not_q: out  std_logic
    );
end component;

signal VCC: std_logic := '1';
signal first_line_output, second_line_output: std_logic_vector(7 downto 0);

signal counter: std_logic_vector(4 downto 1);
signal q_temp: std_logic_vector(4 downto 1);
signal flag: STD_LOGIC;
constant set: STD_LOGIC := '1';
constant reset: STD_LOGIC := '1';

begin

   process(clock) is
    begin
    if (clock = '1') then
    
          flag <= '0';        
          
          flag <= ((load) and (not data(1))) or 
          ((not q_temp(1)) and ((not enp) and (not ent) and load)) or 
          (load and ((not enp) or (
          not ent)) and q_temp(1));

         
    end if;
    end process;

end Behavioral;
