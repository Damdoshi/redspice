# Bunny CAD / BCAD

Bunny CAD est un éditeur et simulateur de circuits logiques décrits par fichiers texte.

Par défaut, le programme ouvre maintenant l'interface graphique. Le mode simulation/shell sans fenêtre reste disponible avec `--simulate`.

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

### Édition

- Clic gauche sur un composant + déplacement : sélectionner et déplacer le composant.
- Clic gauche sur un segment de piste + déplacement : sélectionner et déplacer toute la piste.
- Clic gauche sur un point de piste + déplacement : déplacer uniquement ce point de piste.
- Bouton `SEL` / `DRAW` ou touche `P` : basculer entre le mode sélection et le mode dessin.
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
- Clic droit ou `Échap` pendant le placement : annuler le composant en cours de placement.


### Pistes, couches et routage

- Une piste sans géométrie physique est affichée en vert fin : c'est le chevelu logique, utile pour montrer une liaison non routée.
- Une piste physique est affichée en rouge épais sur la couche composants et en bleu épais sur la couche pistes.
- Le routage se fait maintenant en **mode dessin** (`P` ou bouton `SEL`/`DRAW`).
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
- Un via est représenté automatiquement par deux nœuds superposés reliés par un segment entre couches différentes ; le changement de couche est marqué par une pastille jaune/blanche.
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
