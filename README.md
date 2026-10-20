# Bunny CAD / BCAD

Bunny CAD est un éditeur et simulateur de circuits logiques décrits par fichiers texte.

Par défaut, le programme ouvre maintenant l'interface graphique. Le mode simulation/shell sans fenêtre reste disponible avec `--simulate`.

`make test` lance les vérifications de fiabilité avec sanitizers, sans installer
LibLapin. Les garanties de sauvegarde et les limites de ces tests sont décrites
dans [RELIABILITY.md](RELIABILITY.md).

Les nouvelles [primitives de prototypage](docs/PROTOTYPAGE.md) comprennent huit
portes pédagogiques, une sonde à cinq couleurs, diode, interrupteur, NPN/PNP,
résistance, DEL, deux poussoirs à quatre pattes et condensateur chimique.
Six circuits prêts à ouvrir se trouvent dans `res/00_prototypage/`.

## Lancement

```sh
./bcad circuit.bc
./bcad circuit.bc entree=1 autre_entree=0
./bcad circuit.bc --simulate
```

`--screen` est encore accepté pour compatibilité, mais il n'est plus nécessaire.

## Contrôles de l'interface graphique

### Vue

- Molette souris : zoomer / dézoomer autour de la position de la souris. Le zoom arrière est capé à 1/4 du zoom par défaut pour éviter les ralentissements extrêmes.
- `+` du pavé numérique, ou `=` : zoomer autour de la souris.
- `-` du pavé numérique, ou `-` : dézoomer autour de la souris.
- `Espace` + clic gauche + déplacement souris : déplacer la vue.
- `ZQSD` : déplacer la vue au clavier.
- `WASD` : déplacer la vue au clavier, alternative clavier QWERTY.
- Flèches directionnelles : déplacer la vue.
- `Shift` + déplacement clavier : déplacement plus rapide.
- `Home` / `Origine` : réinitialiser le zoom au niveau par défaut autour de la souris.

### Simulation

- Bouton `SAVE!` ou `Ctrl+S` : sauvegarder le circuit.
- Bouton `STEP!` ou `N` : exécuter un pas de simulation.
- Bouton `RUN` ou `R` : lancer la simulation continue.
- Bouton `STOP!` ou `T` : arrêter la simulation continue.
- Clic molette sur une entrée : basculer sa valeur entre 0 et 1.
- Clic molette sur un interrupteur : ouvrir / fermer le contact.
- Clic molette maintenu sur un poussoir : appuyer ; relâcher pour rouvrir le contact.
- Ces commandes recalculent immédiatement le circuit, même en pause.

### Édition

- Clic gauche sur un composant + déplacement : sélectionner et déplacer le composant.
- Clic gauche sur un segment de piste + déplacement : sélectionner et déplacer toute la piste.
- Clic gauche sur un point de piste + déplacement : déplacer uniquement ce point de piste.
- Bouton `SEL` / `DRAW` ou touche `P` : basculer entre le mode sélection et le mode dessin.
- `Page Up` : tourner les composants sélectionnés de 90° dans le sens antihoraire ; `Page Down` : tourner de 90° dans le sens horaire. Chaque composant tourne autour de son point d'ancrage, avec ses broches. Le reroutage local tente de raccorder les broches restées proches du cuivre ; le chevelu indique les raccordements à reprendre.
- Clic gauche maintenu depuis une zone vide + déplacement : dessiner un rectangle de sélection multiple.
- Après une sélection, un cadre reste visible autour des éléments sélectionnés.
- Clic gauche + déplacement n’importe où à l’intérieur du cadre de sélection : déplacer toute la sélection, même si elle ne contient qu’un seul élément.
- `Suppr` : supprimer tous les éléments sélectionnés.
- Clic droit sur un unique élément sélectionné : ouvrir le menu contextuel.
- Menu contextuel `Renommer` : saisir un nouveau nom puis valider avec `Entrée`.
- Menu contextuel `Supprimer` : supprimer l'élément sélectionné.
- `Échap` : fermer le renommage, le panneau d’ajout ou le menu contextuel ; sinon quitter l'interface graphique.

### Ajout de composants

- Bouton `ADD` ou touche `C` / `Tab` : ouvrir / fermer le panneau d'ajout de composants.
- Texte au clavier : filtrer les composants par nom dans le panneau.
- `Backspace` : effacer un caractère du filtre.
- Flèche haut / bas, `PageUp`, `PageDown` ou molette souris : naviguer dans les composants filtrés.
- Quand le panneau est ouvert, la molette et les flèches servent au panneau et ne zooment/déplacent pas la vue.
- Clic gauche sur un composant du panneau, ou `Entrée` sur le composant sélectionné : choisir ce type de composant.
- Après choix, le panneau se ferme et le composant apparaît sous le curseur, aligné sur la grille.
- Clic gauche : poser le composant avec un nom généré automatiquement.
- `Shift` + clic gauche : poser un exemplaire et garder un nouvel exemplaire du même type sous le curseur.
- Pendant le placement, `Page Up` / `Page Down` tournent aussi le composant sous le curseur ; les exemplaires successifs posés avec `Shift` gardent cette orientation.
- Clic droit ou `Échap` pendant le placement : annuler le composant en cours de placement.


### Pistes, couches et routage

L'orientation est conservée dans les sauvegardes : une position peut s'écrire `[x,y,90]`, `[x,y,180]` ou `[x,y,270]` (degrés horaires). La forme habituelle `[x,y]` reste valable et correspond à 0°.

- Une liaison non routée est affichée en gris pointillé : c'est le chevelu logique. Le vert des bornes est réservé à l'état vrai.
- Un départ de routage sans segment physique reste solidaire de la borne du composant : déplacer une entrée ne laisse plus de pastille derrière elle.
- Au relâchement d'un petit déplacement, le reroutage local tente de réparer les pistes déjà dessinées. Il traite les broches déplacées de cinq pas de grille maximum par axe, conserve le cuivre éloigné et les vias, et cherche un passage horizontal, vertical ou à 45° autour des pistes et pastilles des autres réseaux.
- Le reroutage local est actif au lancement. `Ctrl+F10`, ou le bouton `AUTO ON` / `AUTO OFF` dans le menu `Ctrl`, permet de le désactiver ou de le réactiver pour la session.
- Le calcul a lieu après le déplacement ou la rotation, avec un budget limité pour garder l'interface fluide. Les grands déplacements, les passages bloqués ou les recherches interrompues laissent un chevelu gris entre les bornes et le cuivre à reprendre. Les pistes sélectionnées ou attachées au curseur gardent leur géométrie manuelle.
- Les points de piste sont de petits points rouges ou bleus selon la couche ; les pastilles des composants gardent leurs couleurs d'état.
- Une piste physique est affichée en rouge épais sur la couche composants et en bleu épais sur la couche pistes.
- Le routage se fait maintenant en **mode dessin** (`P` ou bouton `SEL`/`DRAW`).
- En mode sélection, maintenir `Ctrl` permet de router avec les mêmes commandes, sans changer de mode.
- Le menu alternatif `Ctrl`, le menu `Ctrl` + `Shift` et leurs raccourcis restent accessibles : cliquer dans la barre déclenche sa commande, cliquer dans le circuit permet de router.
- Relâcher `Ctrl` conserve la piste active et son aperçu. Le glisser gauche déplace alors la caméra, même depuis un composant ; maintenir de nouveau `Ctrl` permet de continuer le tracé. Un clic droit détache la piste.
- Dès qu'une piste est active, un aperçu suit le curseur depuis le dernier point posé, dans la couleur de la couche et avec le même alignement que le prochain clic. Cet aperçu ne modifie pas le circuit et se masque pendant le déplacement de la caméra.
- En mode dessin, clic gauche dans le vide : créer une piste physique ou ajouter un nœud à la piste active.
- En mode dessin, clic gauche sur un nœud de piste sans piste active : s’attacher à ce nœud pour démarrer une branche.
- En mode dessin, clic gauche sur un nœud de piste avec une piste active : créer la jonction puis se détacher automatiquement. Si le nœud appartient à une autre piste, les deux pistes sont fusionnées physiquement ; s’il appartient à la piste active, un segment interne est ajouté.
- En mode dessin, clic gauche sur un segment de piste : insérer un nœud sur ce segment et s’y attacher, ou créer la jonction puis se détacher si une piste était déjà active.
- En mode dessin, clic gauche sur une patte de composant sans piste active : reprendre la piste déjà attachée à cette patte, ou créer une nouvelle piste si la patte est libre. Avec une piste active : router jusqu’à cette patte, l’attacher, fusionner avec la piste déjà attachée si nécessaire, puis se détacher automatiquement.
- En mode dessin, clic droit avec une piste active : se détacher de la piste active sans la supprimer.
- En mode dessin, clic droit sans piste active sur un segment de piste : supprimer la branche physique à partir de ce segment, en s’arrêtant aux embranchements.
- En mode dessin, clic droit sans piste active sur une piste hors segment : supprimer toute cette piste.
- En mode dessin, `Espace` + clic gauche + déplacement souris continue à déplacer la vue.
- `V` ou double-clic sur le nœud actif en mode dessin : placer un via et continuer le routage sur l'autre couche. Cela fonctionne aussi si le nœud actif est sous une patte de composant : clique une première fois sur la patte pour t’y attacher, puis double-clique au même endroit ou appuie sur `V` pour partir de cette patte sur l’autre face.
- Un via est représenté automatiquement par deux nœuds superposés reliés par un segment entre couches différentes ; le changement de couche est marqué par une pastille grise à fin contour blanc avec un trou central.
- Deux pistes physiques distinctes qui se touchent ou se croisent sur la même couche clignotent pour signaler une erreur de routage.

## Mode shell / simulation sans écran

Le mode sans fenêtre s'active avec `--simulate`. Après un premier calcul et affichage des sorties, le shell interactif reste disponible.

Commandes du shell :

- `simulate` : exécuter un pas de simulation.
- `display` : afficher les sorties.
- `sdn N` : simuler et afficher `N` pas.
- `map` : afficher la carte des entrées/sorties.
- `dump` : afficher l'état complet des composants.
- `loop` : simuler en boucle jusqu'à interruption.
- `nom=0` ou `nom=1` : fixer une entrée.
- `help` : afficher l'aide.
- `exit` : quitter.
## Composants

### Logique

* 4001 - Quad 2 Inputs NOR Gates
* 4008 - 4Bits adder
* 4011 - Quad 2 Inputs NAND Gates
* 4013 - Dual D-Latch
* 4017 - 10Bits Johnson Counter
* 4030 - Quad 2 Inputs XOR Gates
* 4040 - 12Bits Binary Counter
* 4069 - Six NOT Gates
* 4071 - Quad 2 Inputs OR Gates
* 4081 - Quad 2 Inputs AND Gates
* 4094 - 8Bits Shift Register
* 4514 - 4Bits Decoder

### Mémoires

* 2717 - 2KB Read-only Memory

### Elements de sipulation

* Clock input (varies from 0 to 1 and 1 to 0, starting from the value you set)
* True (Is 1)
* False (Is 0)
* Input (value is the one you set)
* Output (its value is printed on terminal when requested)
* Terminal (a fake component provided to circuits so they can write on terminal)
