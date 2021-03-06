----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 21.09.2021 18:09:34
-- Design Name: 
-- Module Name: d - Behavioral
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

entity d is
port (
        set: in STD_LOGIC;
        d_in: in STD_LOGIC;
        c: in STD_LOGIC;
        reset: in STD_LOGIC;
        q: out STD_LOGIC;
        not_q: out STD_LOGIC
    );
end d;

architecture Behavioral of d is
begin

    process (set, c, reset) is
    begin
        if  set = '0' then
                q <= '1';
                not_q <= '0';
            elsif reset = '0' then
                    q <= '0';
                    not_q <= '1';
                elsif rising_edge(c) then 
                    q <= d_in;
                    not_q <= not d_in;
        end if;
    end process;
end Behavioral;