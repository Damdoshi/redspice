# Clic molette sur SW: la DEL suit le contact. Le condensateur reste toujours ouvert (sonde Z).

.chipsets:
true VCC [-15,-5]
SWITCH SW(open) [-6,-5]
RESISTOR R(330) [2,-5]
LED DEL [10,-5]
false GND [17,-5]
PROBE anode [10,0]
true VCC_cap [-15,8]
CAPACITOR C(47uF 16V) [-3,8]
PROBE apres_C [6,8]

.tracks:
fil_0 [b -15,-5 b -9,-5] {1-2}
fil_1 [b -3,-5 b -1,-5] {1-2}
fil_2 [b 5,-5 b 7,-5 b 8,-5 b 8,0] {1-2 2-3 3-4}
fil_3 [b 13,-5 b 17,-5] {1-2}
fil_4 [b -15,8 b -6,8] {1-2}
fil_5 [b 0,8 b 4,8] {1-2}

.links:
VCC:1 fil_0:1@1
SW:1 fil_0:2@2
SW:2 fil_1:1@1
R:1 fil_1:2@2
R:2 fil_2:1@1
DEL:1 fil_2:2@2
anode:1 fil_2:3@4
DEL:2 fil_3:1@1
GND:1 fil_3:2@2
VCC_cap:1 fil_4:1@1
C:1 fil_4:2@2
C:2 fil_5:1@1
apres_C:1 fil_5:2@2
