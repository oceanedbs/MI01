----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    15:41:13 10/09/2017 
-- Design Name: 
-- Module Name:    decodeur - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
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
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity decodeur is
	PORT(PB_0, SW_0 : IN BIT;
		LED_0 : OUT BIT;
		LED_7654 : OUT INTEGER RANGE 0 to 15);
end decodeur;

architecture Behavioral of decodeur is
begin
process(PB_0)
variable state : INTEGER RANGE 0 to 5;
begin
if(PB_0'Event and PB_0 = '1') then
case state is
when 0 => if (SW_0 = '1') then 
state := 1; 
LED_0 <= '0'; 
 end if;
when 1 => if (SW_0 = '1') then 
state := 2; 
 else 
  LED_0 <= '0'; 
state :=1; 
 end if;
when 2 => if (SW_0 = '0') then 
state := 3; 
 else 
  LED_0 <= '0';
  state :=2; 
 end if;
when 3 => if (SW_0 = '1') then 
state := 4; 
 else
  LED_0 <= '0'; 
  state :=3; 
 end if;
when 4 => if (SW_0 = '0') then 
state := 5; 
else LED_0 <= '0'; 
state :=4;
end if;
when others => state := 0;
end case;
LED_7654 <= state;
end if;end process;
end Behavioral;

