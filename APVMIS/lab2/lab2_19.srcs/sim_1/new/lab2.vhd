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
        ud: in STD_LOGIC;
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
signal data_temp: std_logic_vector(4 downto 1);
constant setc: STD_LOGIC := '1';
constant resetc: STD_LOGIC := '1';

begin

        data_temp(1) <= ((not load) and data(1)) or
          ((not q_temp(1)) and ((not enp) and (not ent) and load)) or 
          (load and (enp or ent) and q_temp(1));

    d1 : d port map (
        set => setc,
        reset => resetc,
        d_in => data_temp(1),
        c => clock,
        q => q_temp(1)
    );

          counter(1) <= not (((not ud) and (q_temp(1))) or ((ud) and (not q_temp(1))));
          
          data_temp(2) <= ((not load) and data(2)) or
                    ((not q_temp(2)) and ((not enp) and (not ent) and load) and counter(1)) or 
                    ((q_temp(2) and load) and not((not enp) and (not ent) and load) and counter(1));
          
              d2 : d port map (
                  set => setc,
                  reset => resetc,
                  d_in => data_temp(2),
                  c => clock,
                  q => q_temp(2)
              );
          
          counter(2) <= not (((not ud) and (q_temp(2))) or ((ud) and (not q_temp(2))));
          
          data_temp(3) <= ((not load) and data(3)) or
                    ((not q_temp(3)) and ((not enp) and (not ent) and load)) or 
                    (load and ((not enp) or (
                    not ent)) and q_temp(3));
          
              d3 : d port map (
                  set => setc,
                  reset => resetc,
                  d_in => data_temp(3),
                  c => clock,
                  q => q_temp(3)
              );
          
          counter(3) <= not (((not ud) and (q_temp(3))) or ((ud) and (not q_temp(3))));
          
          data_temp(4) <= ((not load) and data(4)) or
                    ((not q_temp(4)) and ((not enp) and (not ent) and load)) or 
                    (load and ((not enp) or (
                    not ent)) and q_temp(4));
          
              d4 : d port map (
                  set => setc,
                  reset => resetc,
                  d_in => data_temp(4),
                  c => clock,
                  q => q_temp(4)
              );
          
          counter(4) <= not (((not ud) and (q_temp(4))) or ((ud) and (not q_temp(4))));

          rco <= (not ent) and counter(1) and counter(2) and conuter(3) and counter(4);
         

end Behavioral;
