----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 15.09.2021 18:30:59
-- Design Name: 
-- Module Name: second - Behavioral
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

entity second is
    port (
        A: in std_logic_vector(4 downto 1);
        B: in std_logic_vector(4 downto 1);
        OE: in STD_LOGIC;
        AB: in STD_LOGIC;
        Y: out std_logic_vector(4 downto 1)
    );
end second;

architecture Behavioral of second is

begin

process(A, B, OE, AB) is
    begin
          
        if (((A(1) and (not AB)) = '1') or ((B(1) and AB) = '1')) then        
            Y(1) <= '1';
            else Y(1) <= '0';                 
        end if; 
        
        if (((A(2) and (not AB)) = '1') or ((B(2) and AB) = '1')) then        
            Y(2) <= '1';
            else Y(2) <= '0';                 
        end if; 
                
        if (((A(3) and (not AB)) = '1') or ((B(3) and AB) = '1')) then        
             Y(3) <= '1';
             else Y(3) <= '0';                 
        end if; 
                        
        if (((A(4) and (not AB)) = '1') or ((B(4) and AB) = '1')) then        
             Y(4) <= '1';
             else Y(4) <= '0';                 
        end if; 
        
        if (OE = '1') then Y <= (others => 'Z');
       
        end if;
           
        
    end process;

end Behavioral;
