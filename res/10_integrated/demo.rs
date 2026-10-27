# Deux instances du meme CI, chacune avec son etat interne.
.chipsets:
input a(0) [-12,-5]
input b(1) [-12,8]
true vcc [-12,-10]
false gnd [-12,14]
silicon first(buffer4.rsi) [0,-5]
silicon second(buffer4.rsi) [0,8]
output y_a [15,-3]
output y_b [15,10]
.links:
a:1 first:1
b:1 second:1
vcc:1 first:8
vcc:1 second:8
gnd:1 first:4
gnd:1 second:4
first:3 y_a:1
second:3 y_b:1
