# Modeles NPN et PNP: cliquer leur base. Rappel haut du NPN, rappel bas du PNP. B=1,C=2,E=3.

.chipsets:
NPN Q_npn [-13,0]
input base_npn(0) [-22,0]
RESISTOR R_npn(10k) [-13,-9]
true rappel_npn [-22,-9]
false emetteur_npn [-6,4]
PROBE collecteur_npn [-4,-2]
PNP Q_pnp [13,0]
input base_pnp(0) [4,0]
RESISTOR R_pnp(10k) [13,-9]
false rappel_pnp [4,-9]
true emetteur_pnp [20,4]
PROBE collecteur_pnp [22,-2]

.tracks:
fil_0 [b -22,0 b -16,0] {1-2}
fil_1 [b -22,-9 b -16,-9] {1-2}
fil_2 [b -10,-9 b -10,-2 b -6,-2] {1-2 2-3}
fil_3 [b -10,2 b -10,4 b -6,4] {1-2 2-3}
fil_4 [b 4,0 b 10,0] {1-2}
fil_5 [b 4,-9 b 10,-9] {1-2}
fil_6 [b 16,-9 b 16,-2 b 20,-2] {1-2 2-3}
fil_7 [b 16,2 b 16,4 b 20,4] {1-2 2-3}

.links:
base_npn:1 fil_0:1@1
Q_npn:1 fil_0:2@2
rappel_npn:1 fil_1:1@1
R_npn:1 fil_1:2@2
R_npn:2 fil_2:1@1
Q_npn:2 fil_2:2@2
collecteur_npn:1 fil_2:3@3
Q_npn:3 fil_3:1@1
emetteur_npn:1 fil_3:2@3
base_pnp:1 fil_4:1@1
Q_pnp:1 fil_4:2@2
rappel_pnp:1 fil_5:1@1
R_pnp:1 fil_5:2@2
R_pnp:2 fil_6:1@1
Q_pnp:2 fil_6:2@2
collecteur_pnp:1 fil_6:3@3
Q_pnp:3 fil_7:1@1
emetteur_pnp:1 fil_7:2@3
