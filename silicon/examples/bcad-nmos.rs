# Lancer depuis la racine du depot. Broches: 1=A, 2=VCC, 3=Y, 4=GND.
.chipsets:
input a(0) [-10,0]
true vcc [-10,-3]
false gnd [-10,3]
silicon u1(silicon/examples/nmos-inverter.rsi) [0,0]
output y [14,0]
.links:
a:1 u1:1
vcc:1 u1:2
u1:3 y:1
gnd:1 u1:4
