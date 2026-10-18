# Les cinq couleurs: rouge=0, vert=1, gris=U, jaune=B (conflit), violet=Z (haute impedance).

.chipsets:
input source_faux(0) [-8,-12]
PROBE faux [6,-12]
input source_vrai(1) [-8,-6]
PROBE vrai [6,-6]
input source_indefini [-8,0]
PROBE indefini [6,0]
input source_haute_impedance(Z) [-8,12]
PROBE haute_impedance [6,12]
input conflit_bas(0) [-8,6]
input conflit_haut(1) [-8,9]
PROBE broken [6,6]

.tracks:
fil_0 [b -8,-12 b 4,-12] {1-2}
fil_1 [b -8,-6 b 4,-6] {1-2}
fil_2 [b -8,0 b 4,0] {1-2}
fil_3 [b -8,12 b 4,12] {1-2}
fil_4 [b -8,6 b 0,6 b 4,6 b -8,9 b 0,9] {1-2 2-3 4-5 5-2}

.links:
source_faux:1 fil_0:1@1
faux:1 fil_0:2@2
source_vrai:1 fil_1:1@1
vrai:1 fil_1:2@2
source_indefini:1 fil_2:1@1
indefini:1 fil_2:2@2
source_haute_impedance:1 fil_3:1@1
haute_impedance:1 fil_3:2@2
conflit_bas:1 fil_4:1@1
conflit_haut:1 fil_4:2@4
broken:1 fil_4:3@3
