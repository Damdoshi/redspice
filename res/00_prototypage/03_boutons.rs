# Maintenir le clic molette sur un poussoir: la sonde passe de 1 a 0. Relacher meme hors du bouton.

.chipsets:
PUSHBUTTON BP_carre [-7,-7]
true VCC_carre [-15,-13]
RESISTOR R_carre(10k) [-7,-13]
false GND_carre [-12,-4]
PROBE s_carre [6,-4]
PUSHBUTTON_COMPACT BP_rectangle [-7,5]
true VCC_rectangle [-15,-1]
RESISTOR R_rectangle(10k) [-7,-1]
false GND_rectangle [-12,6]
PROBE s_rectangle [6,6]

.tracks:
fil_0 [b -15,-13 b -10,-13] {1-2}
fil_1 [b -12,-4 b -7,-4] {1-2}
fil_2 [b -4,-13 b -1,-13 b -1,-4 b -4,-4 b 4,-4] {1-2 2-3 3-4 3-5}
fil_3 [b -15,-1 b -10,-1] {1-2}
fil_4 [b -12,6 b -7,6] {1-2}
fil_5 [b -4,-1 b -1,-1 b -1,6 b -4,6 b 4,6] {1-2 2-3 3-4 3-5}

.links:
VCC_carre:1 fil_0:1@1
R_carre:1 fil_0:2@2
GND_carre:1 fil_1:1@1
BP_carre:2 fil_1:2@2
R_carre:2 fil_2:1@1
BP_carre:3 fil_2:2@4
s_carre:1 fil_2:3@5
VCC_rectangle:1 fil_3:1@1
R_rectangle:1 fil_3:2@2
GND_rectangle:1 fil_4:1@1
BP_rectangle:2 fil_4:2@2
R_rectangle:2 fil_5:1@1
BP_rectangle:3 fil_5:2@4
s_rectangle:1 fil_5:3@5
