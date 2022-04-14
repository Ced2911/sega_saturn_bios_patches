# orignal code decompress executable 
# from: 0x7000 size: 0x40000
# to: 0x06010000

.section .patch0x1728_s

load_bootanim:
    mov.l   load_bootanim_trampoline,r0
    jsr @r0
    nop
    .align 4
load_bootanim_trampoline:     .long 0x7000
.section .patch0x1728_e

.section .patch0x7000_s
load_bootanim_trampoline_code:
# disable interrupt
    mov     #0xf,r0
    shll2   r0
    shll2   r0
    ldc     r0,sr
# setup copy
    mov.l   boot_data_dest, r4
    mov.l   boot_data_start_ptr, r5
    mov.l   boot_data_len, r6
# copy
memcpy:
    mov.l   @r5+,r3
    mov.l   r3,@r4
    dt      r6
    bf/s    memcpy
    add     #4,r4
    nop 

copy_end:
# boot anim
    mov.l   boot_data_dest,r0
    jsr @r0
    nop

.align 4
boot_data_dest: .long 0x06004000
boot_data_start_ptr: .long boot_data_start
boot_data_end_ptr: .long boot_data_end
boot_data_len: .long (boot_data_end-boot_data_start)/4

boot_data_start: 
    .incbin "bootloader.bin"
boot_data_end:
    nop

.section .patch0x7000_e
