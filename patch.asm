# ignored ...
.section .text
.global start
start:
.word  0x1000


# patchs start
.include "ip_bin_check_bypass.asm"
