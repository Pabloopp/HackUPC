library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity conv_3x3_center is
    Port ( 
        clk        : in STD_LOGIC;
        rst        : in STD_LOGIC;
        start      : in STD_LOGIC;
        pixels_in  : in STD_LOGIC_VECTOR(71 downto 0);  -- 9 pixels de 8 bits (3x3)
        pixel_out  : out STD_LOGIC_VECTOR(7 downto 0);  -- Pixel central procesado
        done       : out STD_LOGIC;
        ack        : in STD_LOGIC
    );
end conv_3x3_center;

architecture Behavioral of conv_3x3_center is
    -- Kernel Sobel X (detector de bordes verticales)
    type kernel_array is array(0 to 8) of integer;
    constant kernel : kernel_array := (
        1, 0, -1,
        2, 0, -2,
        1, 0, -1
    );
    
    type state_type is (IDLE, PROCESSING, FINISHED);
    signal state : state_type;
    
    -- Registros para los píxeles de entrada
    signal pixel_regs : std_logic_vector(71 downto 0);

begin
    process(clk, rst)
        variable sum : integer := 0;
    begin
        if rst = '1' then
            state <= IDLE;
            pixel_out <= (others => '0');
            done <= '0';
            pixel_regs <= (others => '0');
        elsif rising_edge(clk) then
            case state is
                when IDLE =>
                    done <= '0';
                    if start = '1' then
                        pixel_regs <= pixels_in;
                        state <= PROCESSING;
                    end if;
                
                when PROCESSING =>
                    -- Aplicamos convolución solo al píxel central
                    sum := 0;
                    for i in 0 to 8 loop
                        sum := sum + (to_integer(unsigned(pixel_regs((i+1)*8-1 downto i*8))) * kernel(i));
                    end loop;
                    
                    -- Aplicar saturación (0-255)
                    if sum < 0 then
                        sum := 0;
                    elsif sum > 255 then
                        sum := 255;
                    end if;
                    
                    pixel_out <= std_logic_vector(to_unsigned(sum, 8));
                    state <= FINISHED;
                
                when FINISHED =>
                    done <= '1';
                    if ack = '1' then
                        state <= IDLE;
                    end if;
                when others =>
                    state <= IDLE;
            end case;
        end if;
    end process;
end Behavioral;