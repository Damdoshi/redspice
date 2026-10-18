# Huit portes pedagogiques. Clic molette sur chaque entree pour essayer les tables de verite.

.chipsets:
AND g_and [-13,-12]
input a_and(0) [-20,-13]
PROBE s_and [-6,-12]
input b_and(1) [-20,-11]
NAND g_nand [13,-12]
input a_nand(0) [6,-13]
PROBE s_nand [20,-12]
input b_nand(1) [6,-11]
OR g_or [-13,-4]
input a_or(0) [-20,-5]
PROBE s_or [-6,-4]
input b_or(1) [-20,-3]
NOR g_nor [13,-4]
input a_nor(0) [6,-5]
PROBE s_nor [20,-4]
input b_nor(1) [6,-3]
XOR g_xor [-13,4]
input a_xor(0) [-20,3]
PROBE s_xor [-6,4]
input b_xor(1) [-20,5]
XNOR g_xnor [13,4]
input a_xnor(0) [6,3]
PROBE s_xnor [20,4]
input b_xnor(1) [6,5]
BUF g_buf [-13,12]
input a_buf(0) [-20,12]
PROBE s_buf [-6,12]
NOT g_not [13,12]
input a_not(0) [6,12]
PROBE s_not [20,12]

.tracks:
fil_0 [b -20,-13 b -16,-13] {1-2}
fil_1 [b -10,-12 b -8,-12] {1-2}
fil_2 [b -20,-11 b -16,-11] {1-2}
fil_3 [b 6,-13 b 10,-13] {1-2}
fil_4 [b 16,-12 b 18,-12] {1-2}
fil_5 [b 6,-11 b 10,-11] {1-2}
fil_6 [b -20,-5 b -16,-5] {1-2}
fil_7 [b -10,-4 b -8,-4] {1-2}
fil_8 [b -20,-3 b -16,-3] {1-2}
fil_9 [b 6,-5 b 10,-5] {1-2}
fil_10 [b 16,-4 b 18,-4] {1-2}
fil_11 [b 6,-3 b 10,-3] {1-2}
fil_12 [b -20,3 b -16,3] {1-2}
fil_13 [b -10,4 b -8,4] {1-2}
fil_14 [b -20,5 b -16,5] {1-2}
fil_15 [b 6,3 b 10,3] {1-2}
fil_16 [b 16,4 b 18,4] {1-2}
fil_17 [b 6,5 b 10,5] {1-2}
fil_18 [b -20,12 b -16,12] {1-2}
fil_19 [b -10,12 b -8,12] {1-2}
fil_20 [b 6,12 b 10,12] {1-2}
fil_21 [b 16,12 b 18,12] {1-2}

.links:
a_and:1 fil_0:1@1
g_and:1 fil_0:2@2
g_and:3 fil_1:1@1
s_and:1 fil_1:2@2
b_and:1 fil_2:1@1
g_and:2 fil_2:2@2
a_nand:1 fil_3:1@1
g_nand:1 fil_3:2@2
g_nand:3 fil_4:1@1
s_nand:1 fil_4:2@2
b_nand:1 fil_5:1@1
g_nand:2 fil_5:2@2
a_or:1 fil_6:1@1
g_or:1 fil_6:2@2
g_or:3 fil_7:1@1
s_or:1 fil_7:2@2
b_or:1 fil_8:1@1
g_or:2 fil_8:2@2
a_nor:1 fil_9:1@1
g_nor:1 fil_9:2@2
g_nor:3 fil_10:1@1
s_nor:1 fil_10:2@2
b_nor:1 fil_11:1@1
g_nor:2 fil_11:2@2
a_xor:1 fil_12:1@1
g_xor:1 fil_12:2@2
g_xor:3 fil_13:1@1
s_xor:1 fil_13:2@2
b_xor:1 fil_14:1@1
g_xor:2 fil_14:2@2
a_xnor:1 fil_15:1@1
g_xnor:1 fil_15:2@2
g_xnor:3 fil_16:1@1
s_xnor:1 fil_16:2@2
b_xnor:1 fil_17:1@1
g_xnor:2 fil_17:2@2
a_buf:1 fil_18:1@1
g_buf:1 fil_18:2@2
g_buf:2 fil_19:1@1
s_buf:1 fil_19:2@2
a_not:1 fil_20:1@1
g_not:1 fil_20:2@2
g_not:2 fil_21:1@1
s_not:1 fil_21:2@2
