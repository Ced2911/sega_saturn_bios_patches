# Remove ip.bin check
patch bios, ip.bin check to boot all region disc

### Original version:
```c
...
if (memcmp(0x20006200, 0x06002100, 0x340) != 0) {
    return -4;
}
if (!check_region...) {
    return -8;
}
...
```

### Patched version:
bypass the ip.bin and region check by jumping from 0x1b04 to 0x1bd6
