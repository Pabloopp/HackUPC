library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity conv_3x3_center_tb is
end conv_3x3_center_tb;

architecture tb of conv_3x3_center_tb is
    -- Componente a testear
    component conv_3x3_center
    port (
        clk       : in STD_LOGIC;
        rst       : in STD_LOGIC;
        start     : in STD_LOGIC;
        pixels_in : in STD_LOGIC_VECTOR(71 downto 0);
        pixel_out : out STD_LOGIC_VECTOR(7 downto 0);
        done      : out STD_LOGIC
    );
    end component;

    -- Señales de prueba
    signal clk       : STD_LOGIC := '0';
    signal rst       : STD_LOGIC := '1';
    signal start     : STD_LOGIC := '0';
    signal pixels_in : STD_LOGIC_VECTOR(71 downto 0) := (others => '0');
    signal pixel_out : STD_LOGIC_VECTOR(7 downto 0);
    signal done      : STD_LOGIC;

    -- Constantes
    constant CLK_PERIOD : time := 10 ns;
    
begin
    -- Instancia del módulo bajo prueba
    uut: conv_3x3_center port map (
        clk => clk,
        rst => rst,
        start => start,
        pixels_in => pixels_in,
        pixel_out => pixel_out,
        done => done
    );

    -- Generación de reloj
    clk <= not clk after CLK_PERIOD/2;

    -- Proceso de estimulación
    stim_proc: process
    begin
        -- Inicialización
        wait for CLK_PERIOD*2;
        rst <= '0';
        wait for CLK_PERIOD;

        -- Caso 1: Borde vertical claro
        pixels_in <= X"00_00_00_FF_FF_FF_00_00_00"; 
        start <= '1';
        wait for CLK_PERIOD;
        start <= '0';
        wait until done = '1';
        wait for CLK_PERIOD*2;

        -- Caso 2: Sin bordes (uniforme)
        pixels_in <= X"80_80_80_80_80_80_80_80_80"; 
        start <= '1';
        wait for CLK_PERIOD;
        start <= '0';   
        wait until done = '1';
        wait for CLK_PERIOD*2;


        --pixels_in <= X"00_00_7F_00_00_FF_00_00_7F";
        pixels_in <= X"7F_00_00_FF_00_00_7F_00_00";
        start <= '1';
        wait for CLK_PERIOD;
        start <= '0';
        wait until done = '1';
        wait for CLK_PERIOD*2;

        -- Finalización
        wait;
    end process;
end tb;