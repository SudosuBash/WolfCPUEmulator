movw r2,0xffff   0220ffff
ml r2,16         06200110
movw r2,0xfe3f   02303ffe

label:
movw r1,0xffff  02 10 FF FF
ml r1,16        06 00 00 10

add r2,1        04204100
movw r1,0x2     02 10 02 00
movb r3,[r2]    82429000
movb [r1],r3    82 04 A0 00

movw r1,0       02 10 00 00
movb r3,1       02500100
movb [r1],r3    8204A000

movb r3,[r2]    82429000
subn r3,0       044C0000
jne ffdc        4006dcff
jmp ffcc        4000ccff

dbstr "Wolf BIOS Version 1.0",0
# 0x57 0x6f 0x6c 0x66 0x20 0x42 0x49 0x4f 0x53 0x20 0x56 0x65 0x72 0x73 0x69 0x6f 0x6e 0x20 0x31 0x2e 0x30