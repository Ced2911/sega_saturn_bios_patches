# patch bios, ip.bin check to boot all region disc, allow custom message to be set
# remove "Game disc unsuitable for this system"
# @ 0x1b04

.section .patch0x1b04_s

ip_bin_check_bypass:
	bra . + (0x1bd6 - 0x1b04)
    nop

.section .patch0x1b04_e
