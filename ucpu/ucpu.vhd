-- @file ucpu.vhd
-- @brief An incredible simple microcontroller 
-- @license MIT
-- @author Richard James Howe
-- @copyright Richard James Howe (2017)
--
-- Based on:
-- https://stackoverflow.com/questions/20955863/vhdl-microprocessor-microcontroller
--
--  INSTRUCTION    87 6543210  OPERATION
--  ADD WITH CARRY 00 ADDRESS  A = A   + MEM[ADDRESS] 
--  NOR            01 ADDRESS  A = A NOR MEM[ADDRESS]
--  STORE          10 ADDRESS  MEM[ADDRESS] = A
--  JCC            11 ADDRESS  IF(CARRY) { PC = ADDRESS, CLEAR CARRY }
--
-- @todo Make a single port RAM version

library ieee,work;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ucpu is
	generic(
		width: positive range 8 to 32 := 8);
	port(
		clk: in  std_logic;
		rst: in  std_logic;

		pc:  out std_logic_vector(width - 3 downto 0);
		op:  in  std_logic_vector(width - 1 downto 0);

		adr: out std_logic_vector(width - 3 downto 0);
		di:  in  std_logic_vector(width - 1 downto 0);
		re:  out std_logic;
		do:  out std_logic_vector(width - 1 downto 0);
		we:  out std_logic);
end entity;

architecture rtl of ucpu is
	signal a_c,   a_n:       std_logic_vector(di'high + 1 downto di'low) := (others => '0'); -- accumulator
	signal pc_c,  pc_n:      std_logic_vector(pc'range)                  := (others => '0');
	signal alu:              std_logic_vector(1 downto 0)                := (others => '0');
	signal state_c, state_n: std_logic                                   := '0'; 
begin
	pc  <= pc_c;
	do  <= a_c(do'range);
	alu <= op(op'high downto op'high - 1);

	process(clk, rst)
	begin
		if rst = '1' then
			pc_c    <= (others => '0');
			a_c     <= (others => '0');
			state_c <= '0';
		elsif rising_edge(clk) then
			pc_c    <= pc_n;
			state_c <= state_n;
			a_c     <= a_n;
		end if;
	end process;

	process(op, alu, di, a_c, pc_c, state_c)
	begin
		we    <= '0';
		a_n   <= a_c;
		pc_n  <= pc_c;
		re    <= '0';
		adr   <= op(adr'range);
		if state_c = '0' then
			state_n <= '1'; -- Turn into state_n <= not alu(1) when store is moved
			re      <= not alu(1); -- state on ADD and NOR
			pc_n    <= std_logic_vector(unsigned(pc_c) + 1);

			if alu = "11" then -- JCC
				if a_c(a_c'high) = '0' then 
					a_n(a_n'high) <= '0'; -- clear carry
					pc_n          <= op(pc_n'range); 
				end if;
			end if;
		else
			state_n <= '0';
			if    alu = "00" then a_n <= std_logic_vector(unsigned('0' & a_c(di'range)) + unsigned('0' & di));
			elsif alu = "01" then a_n <= a_c nor ('0' & di);
			elsif alu = "10" then we <= '1'; -- move to state, then ALU switch can be on lowest bit
			else                  null;
			end if;
		end if;
	end process;
end architecture;

