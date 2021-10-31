library IEEE;
use IEEE.STD_LOGIC_TEXTIO.ALL;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use STD.TEXTIO.ALL;

entity lr2_TB is
end lr2_TB;

architecture Behavioral of lr2_TB is

component lab2 is
    Port ( 
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

    signal load,  enp, ent : STD_LOGIC := '1';
    signal data : std_logic_vector(4 downto 1);
    signal c: STD_LOGIC := '1';
    signal rco, ud: STD_LOGIC;
    signal q: std_logic_vector(4 downto 1);
    --constant clk_period : time := 20 ns; 

file inputFile : text;
file resultFile : text;
   shared variable result : std_logic_vector(5 downto 1);
begin

mapping : lab2 port map(
    load => load,
    clock => c,
    enp => enp,
    ent => ent,
    data => data,
    rco => rco,
    ql => q,
    ud => ud
);

process
    variable inputLine : line;
    variable input : std_logic_vector(9 downto 1);
    --variable result : std_logic_vector(5 downto 1);
begin

    file_open(inputFile, "E:\BSUIR\7th_sem\APVMIS\lab1\LAB3\lr2\INPUT_2.txt", read_mode);
    file_open(resultFile, "E:\BSUIR\7th_sem\APVMIS\lab1\LAB3\lr2\RESULT_2.txt", read_mode);
    
        while not endfile(inputFile) and not endfile(resultFile) loop
            
            readline(inputFile, inputLine);
            read(inputLine, input);
                     
            (load, ud, c, enp, ent, data(1), data(2), data(3), data(4)) <= input(9 downto 1);
            --(data(4), data(3),  data(2), data(1), ent, enp, c, ud, load ) <= input(9 downto 1);
            wait for 5ns;
            readline(resultFile, inputLine);
            read(inputLine, result);
            
            --(rco, q(4), q(3), q(2), q(1)) <= result (5 downto 1);     
            
            assert(q(1) = result(1) and 
            q(2) = result(2) and 
            q(3) = result(3) and 
            q(4) = result(4) and 
            rco = result(5))
            
            report "Test failed";
          
        end loop;
        
        file_close(inputFile);
        file_close(resultFile);
 
    wait;

end process;

end Behavioral;
