# OU par diodes avec rappel au niveau bas. Clic molette sur a ou b: s = a OU b.

.chipsets:
input a(0) [-14,-5]
input b(0) [-14,5]
DIODE Da [-5,-5]
DIODE Db [-5,5]
RESISTOR R(10k) [-2,11]
false GND [-11,11]
PROBE s [10,0]

.tracks:
fil_0 [b -14,-5 b -8,-5] {1-2}
fil_1 [b -14,5 b -8,5] {1-2}
fil_2 [b -11,11 b -5,11] {1-2}
fil_3 [b -2,-5 b 3,-5 b 3,0 b 3,5 b -2,5 b 3,11 b 1,11 b 8,0] {1-2 2-3 3-4 4-5 4-6 6-7 3-8}

.links:
a:1 fil_0:1@1
Da:1 fil_0:2@2
b:1 fil_1:1@1
Db:1 fil_1:2@2
GND:1 fil_2:1@1
R:1 fil_2:2@2
Da:2 fil_3:1@1
Db:2 fil_3:2@5
R:2 fil_3:3@7
s:1 fil_3:4@8
