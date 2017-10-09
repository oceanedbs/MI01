----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    14:24:41 10/09/2017 
-- Design Name: 
-- Module Name:    compteur - Behavioral 
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
entity compteur is
PORT(PB_0,PB_1:IN BIT;
		LED_10: OUT INTEGER RANGE 0 to 3);
end compteur;

architecture Behavioral of compteur is

begin
PROCESS(PB_0, PB_1)
VARIABLE resultat:INTEGER range 0 to 3;
BEGIN
IF (PB_1 = '1') THEN
	resultat := 0;
ELSE IF(PB_0'EVENT and PB_0='1') THEN

			resultat := resultat + 1;
end if;
END IF;

LED_10 <= resultat;

END PROCESS;

end Behavioral;