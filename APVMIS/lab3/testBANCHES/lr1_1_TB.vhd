library IEEE;
use IEEE.STD_LOGIC_TEXTIO.ALL;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use STD.TEXTIO.ALL;

entity lr1_1_TB is
end lr1_1_TB;

architecture Behavioral of lr1_1_TB is

component lab1 is
    Port ( 
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
        

file inputFile : text;
file resultFile : text;

begin

mapping : lab1 port map(
        B => B,
        OE => OE,
        AB => AB,
        Y => Y,
        A => A
);

process
    variable inputLine : line;
    variable input : std_logic_vector(10 downto 1);
    variable result : std_logic_vector(4 downto 1);
begin

    file_open(inputFile, "E:\BSUIR\7th_sem\APVMIS\lab1\LAB3\lr1_1\INPUT_1.txt", read_mode);
    file_open(resultFile, "E:\BSUIR\7th_sem\APVMIS\lab1\LAB3\lr1_1\RESULT_1.txt", read_mode);
    
        while not endfile(inputFile) and not endfile(resultFile) loop
            
            readline(inputFile, inputLine);
            read(inputLine, input);
                     
            (A(1), A(2), A(3), A(4), B(1), B(2), B(3), B(4), OE, AB) <= input(10 downto 1);
           
            readline(resultFile, inputLine);
            read(inputLine, result);
            
            Y <= result(4 downto 1);
            wait for 5ns;
            
            assert(Y = result)
            report "Test failed for A(1)=" & std_logic'image(A(1)) & 
                    " A(2)=" & std_logic'image(A(2)) & 
                    " A(3)=" & std_logic'image(A(3)) &
                    " A(4)=" & std_logic'image(A(4)) & 
                    " B(1)=" & std_logic'image(B(1)) &
                    " B(2)=" & std_logic'image(B(2)) &
                    " B(3)=" & std_logic'image(B(3)) &
                    " B(4)=" & std_logic'image(B(4)) &
                    " OE=" & std_logic'image(OE) &
                    " AB=" & std_logic'image(AB) &
                    " Y=" & integer'image(to_integer(unsigned(Y)));
          
        end loop;
        
        file_close(inputFile);
        file_close(resultFile);
 
    wait;

end process;

end Behavioral;
