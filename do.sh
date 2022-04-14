#/bin/bash
set -v
set -e
bios_file="Sega Saturn BIOS v1.00 (1994)(Sega)(JP)(M6)"

mkdir ./build -p


# build the patch
sh-elf-as --no-pad-sections "patch.asm" -o "build/patch.o"
node tools/ld_scripts/ld.js "build/patch.o" > "build/patch.ld"
sh-elf-ld -n --warn-section-align -T "build/patch.ld"  -s "build/patch.o"  -o "build/patch.elf"

# display patch disassembly
sh-elf-objdump -D build/patch.elf

# include the patch in the bios
./tools/patcher/build/patcher.elf ./build/patch.elf "${bios_file}.bin" "./build/${bios_file}.hack.bin"
