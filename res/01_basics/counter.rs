.chipsets:
output a [39,24]
output b [33,32]
output c [34,31]
clock clock [45,32]
4040 counter [61,22]
output d [42,31]
output e [37,16]
output f [36,37]
output g [42,41]
output h [39,36]
output i [64,21]
output j [65,40]
output k [46,44]
output l [44,35]
input reset [43,32]

.tracks:
__wire_0 []
__wire_1 []
__wire_10 []
__wire_11 []
__wire_12 []
__wire_13 []
__wire_2 []
__wire_3 []
__wire_4 []
__wire_5 []
__wire_6 []
__wire_7 []
__wire_8 []
__wire_9 []

.links:
clock:1 __wire_0:1
counter:10 __wire_0:2
reset:1 __wire_1:1
counter:11 __wire_1:2
i:1 __wire_10:1
counter:12 __wire_10:2
j:1 __wire_11:1
counter:14 __wire_11:2
k:1 __wire_12:1
counter:15 __wire_12:2
l:1 __wire_13:1
counter:1 __wire_13:2
a:1 __wire_2:1
counter:9 __wire_2:2
b:1 __wire_3:1
counter:7 __wire_3:2
c:1 __wire_4:1
counter:6 __wire_4:2
d:1 __wire_5:1
counter:5 __wire_5:2
e:1 __wire_6:1
counter:3 __wire_6:2
f:1 __wire_7:1
counter:2 __wire_7:2
g:1 __wire_8:1
counter:4 __wire_8:2
h:1 __wire_9:1
counter:13 __wire_9:2

.misc:
camera [59.3372,26.9993]
zoom 18
