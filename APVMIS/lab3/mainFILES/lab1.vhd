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
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity lab1 is
    port (
        A: in std_logic_vector(4 downto 1);
        B: in std_logic_vector(4 downto 1);
        OE: in STD_LOGIC;
        AB: in STD_LOGIC;
        Y: out std_logic_vector(4 downto 1)
    );
end lab1;

architecture Behavioral of lab1 is   

signal y_par: STD_LOGIC_VECTOR(4 downto 1);

begin

     
    y_par(1) <= ((A(1) and (not AB)) or (B(1) and AB)); 
    
    y_par(2) <= ((A(2) and (not AB)) or (B(2) and AB)); 
    
    y_par(3) <= ((A(3) and (not AB)) or (B(3) and AB)); 
    
    y_par(4) <= ((A(4) and (not AB)) or (B(4) and AB));
    
   
    Y(1) <= y_par(1) when OE = '0' else 'Z';
    Y(2) <= y_par(2) when OE = '0' else 'Z';
    Y(3) <= y_par(3) when OE = '0' else 'Z';
    Y(4) <= y_par(4) when OE = '0' else 'Z';
        
end Behavioral;
