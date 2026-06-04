.chipsets:
4040 ctr [0,0]
clock pixclk(0) [-7,-4]
false rst [-7,16]
screen vga(40x30) [8,1]

.tracks:
__wire_0 []
__wire_1 []
__wire_2 []
__wire_3 []
__wire_4 []
__wire_5 []
__wire_6 []
__wire_7 []
__wire_8 []
__wire_9 []

.links:
pixclk:1 __wire_0:1
ctr:10 __wire_0:2
rst:1 __wire_1:1
ctr:11 __wire_1:2
ctr:1 __wire_2:1
vga:1 __wire_2:2
ctr:4 __wire_3:1
vga:2 __wire_3:2
ctr:9 __wire_4:1
vga:3 __wire_4:2
ctr:7 __wire_5:1
vga:4 __wire_5:2
ctr:6 __wire_6:1
vga:5 __wire_6:2
ctr:5 __wire_7:1
vga:6 __wire_7:2
ctr:3 __wire_8:1
vga:7 __wire_8:2
ctr:2 __wire_9:1
vga:8 __wire_9:2
