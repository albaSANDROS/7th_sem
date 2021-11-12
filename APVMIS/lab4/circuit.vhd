----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 08.11.2021 16:26:14
-- Design Name: 
-- Module Name: circuit - Behavioral
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

entity circuit is
    Port ( 
        ledsmain: out std_logic_vector(3 downto 0);
        ledsboard: out std_logic_vector(3 downto 0);
        pushbuttons: in std_logic_vector(4 downto 0);
        dipswitch: in std_logic_vector(3 downto 0);
        sysclk_p: in std_logic;
        sysclk_n: in std_logic
    );
end circuit;

architecture Behavioral of circuit is
    component lab2 is
        port (          
            load: in STD_LOGIC;
            ud: in STD_LOGIC;
            clock: in STD_LOGIC;
            enp: in STD_LOGIC;
            ent: in STD_LOGIC;
            data: in std_logic_vector(4 downto 1);
            ql: out std_logic_vector(4 downto 1);
            rco: out STD_LOGIC
        );
    end component;
    
    component ibufds
        port (
            i, ib : in std_logic; 
            o : out std_logic
        );
    end component;
    
    component DIVIDER is
        port ( 
            CLK_IN : in STD_LOGIC;
            CLK_OUT : out STD_LOGIC
        );
    end component;

signal counter: std_logic_vector(4 downto 1);
signal q_temp: std_logic_vector(4 downto 1);
signal data_temp: std_logic_vector(4 downto 1);
constant setc: STD_LOGIC := '1';
constant resetc: STD_LOGIC := '1';
signal nEPT: STD_LOGIC;
signal clock: STD_LOGIC;
signal CLK_NO_DIV: STD_LOGIC;

begin

lab2_i: lab2 port map(
    load => pushbuttons(0),
    clock => clock,
    ud =>  pushbuttons(1),
    enp => pushbuttons(2),
    ent => pushbuttons(3),
    data(1) => dipswitch(0),
    data(2) => dipswitch(1),
    data(3) => dipswitch(2),
    data(4) => dipswitch(3),
    
    ql(1) => ledsmain(0),
    ql(2) => ledsmain(1),
    ql(3) => ledsmain(2),
    ql(4) => ledsmain(3),
    rco   => ledsboard(0)
);


buffds: ibufds port map (
    i => sysclk_p, 
    ib => sysclk_n, 
    o => CLK_NO_DIV
);

div: DIVIDER port map (
    CLK_IN => CLK_NO_DIV, 
    CLK_OUT => CLOCK
);

end Behavioral;
