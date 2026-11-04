# NIIET MCU K1874VE7T extension
# Makefile version 5.0 Q3-2024
# Mikhail Shomov shomovmy@iss-reshetnev.ru

# System

.PHONY : build_sw .compile .link load_sw help clean-soft

# Variables

COM ?= COM3
BUS ?= 16
BINARY ?= bin/cmc96/main.bin

# Internal targets

.compile :
	mkdir -p bin/cmc96
	mcc96 @src/sw/compile.opt $(wildcard src/sw/*.c)

.link : .compile
	mclink @src/sw/link.opt -O "$(CMC96_INCLUDE);$(CMC96_LIB)" $(wildcard bin/cmc96/*.mco)
	tools/cmc96/hex2bin.exe bin/cmc96/main.HEX

# External targets

help :: 
	printf "\t NIIET MCU K1874VE7T extension\n"
	printf "\t\t checkup\t - check environment\n"
	printf "\t\t build_sw\t - build the software and load to the MCU [COM=%%COM#%%] [BUS={8/16}]\n"
	printf "\t\t load_sw\t - load exist software to the MCU [BINARY=%%path to binary%%]\n"
	printf "\t\t clean-soft\t - remove all build artifacts\n"
	printf "\t\t clean\t\t - remove all build artifacts\n"

checkup ::
	@if [[ "$(origin CMC96_INCLUDE)" == "undefined" ]]; then \
		echo "CMC96_INCLUDE variable undefined, may assign to %Codemaster install dir%\%Codemaster version%\CMC-96\Inc"; \
	fi;
	@if [[ "$(origin CMC96_LIB)" == "undefined" ]]; then \
		echo "CMC96_LIB variable undefined, may assign to %Codemaster install dir%\%Codemaster version%\CMC-96\Lib"; \
	fi;
	@type mcc96 2> /dev/null || echo "K1874VE7T compiler (mcc96) not found"
	@type mclink 2> /dev/null || echo "K1874VE7T linker (mclink) not found"
	@type tools/cmc96/hex2bin.exe 2> /dev/null || echo "tools/cmc96/hex2bin.exe not found"
	@type tools/cmc96/mcu_loader.py 2> /dev/null || echo "tools/cmc96/mcu_loader.py not found"
	@type python 2> /dev/null || echo "python not found"

build_sw : .compile .link load_sw

load_sw : $(BINARY)
	python tools/cmc96/mcu_loader.py --com $(COM) --bus $(BUS) $(BINARY)

clean-soft ::
	rm -rf bin/cmc96/{,.[!.],..?}

clean :: clean-soft
