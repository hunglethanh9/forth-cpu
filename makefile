## Top level make file
#
#	Richard James Howe
#
# TODO:
# 	* The user should just be able to type "make" and it
# 	will create *everything* in the project.
#

DOXYFILE=doxygen.conf

all: _forth _vhdl _doxygen

_welcome:
	@echo "H2 Computing system by Richard James Howe";

_forth:
	@echo "Assembling H2 Source";
	@make -C forth/
	@make -C forth/ run

_vhdl:
	@echo "Making the VHDL simulation and bitfile for the device";
	@make -C vhdl/ simulation synthesis implementation bitfile

_doxygen:
	@echo "Generating system documentation";
	@doxygen $(DOXYFILE)

clean:
	@rm -rf doxy/
	@make -C forth/ clean
	@make -C vhdl/ clean