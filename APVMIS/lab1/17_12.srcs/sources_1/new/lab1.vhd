----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/07/2021 05:54:40 PM
-- Design Name: 
-- Module Name: lab1 - Behavioral
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

entity lab1 is
    port (
        A: in std_logic_vector(1 to 4);
        B: in std_logic_vector(1 to 4);
        OE: in STD_LOGIC;
        AB: in STD_LOGIC;
        Y: out std_logic_vector(1 to 4)
    );
end lab1;

architecture Behavioral of lab1 is

begin
    
    Y(1) <= not OE and ((A(1) and (not AB)) or (B(1) and AB)); 
    
    Y(2) <= not OE and ((A(2) and (not AB)) or (B(2) and AB)); 
    
    Y(3) <= not OE and ((A(3) and (not AB)) or (B(3) and AB)); 
    
    Y(4) <= not OE and ((A(4) and (not AB)) or (B(4) and AB)); 
    
        
end Behavioral;
