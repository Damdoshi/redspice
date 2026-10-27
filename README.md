# BCAD — circuits imprimés et circuits intégrés

BCAD conserve son éditeur de circuits imprimés et accueille désormais la conception NMOS dans le même espace de travail. Chaque buffer possède un type : **circuit imprimé** ou **circuit intégré**.

Un document CI peut être une simple cellule sans boîtier et sans contact extérieur. Il peut instancier d'autres cellules par référence, les déplacer, les tourner et les relier par ses couches conductrices. L'ajout de contacts externes et d'un boîtier rend le résultat utilisable sur une carte. La simulation développe toute la hiérarchie et calcule ses transistors ; les CI personnalisés d'une même carte partagent leur réseau électrique.

```sh
make clean
make -j16
./bcad
./bcad res/10_integrated/demo.rs
./bcad res/10_integrated/buffer4.rsi
```

La fenêtre utilise LibLapin et le compilateur `b++`. Le binaire principal est **bcad**. `silicon/` contient le modèle NMOS et son éditeur embarqué ; aucune seconde application graphique n'est nécessaire.

| Commande | Action |
| --- | --- |
| F1 / F2 | Nouveau circuit imprimé / nouveau circuit intégré (« chipset »). |
| F3 / Shift+F3 | Enregistrer / enregistrer sous. |
| F4 | Ouvrir un fichier ou retrouver un buffer déjà ouvert. |
| F5 / F6 | Centrer sur le pointeur / Home. |
| F7 / F8 | Zoom in / Zoom out. |
| F9 | Component : catalogue et recherche. |
| Ctrl+clic dans Component | Ajouter un composant aux favoris latéraux, sans le placer. |
| Clic / Ctrl+clic sur un favori | Utiliser le composant / retirer le favori. |
| Glisser un favori entre deux boutons | Déplacer le favori ; échanger avec une destination occupée. |
| Ctrl+Tab / Ctrl+Shift+Tab | Buffer suivant / précédent. |
| Ctrl+F9 | Choisir un autre fichier de cellule ou de puce à insérer. |
| Ctrl+Shift+F10, sur un PCB | Examiner l'intérieur de la puce sélectionnée en cours de simulation. |
| F10, dans un CI | Examiner la cellule sélectionnée. |
| Ctrl+F11, en observation | Remonter dans la hiérarchie, puis revenir au buffer source. |
| F11 / F12 | Un pas / lancer ou suspendre la simulation. |

Les vingt boutons favoris (dix de chaque côté) appartiennent au document et sont enregistrés avec lui. Le texte de recherche est effacé à chaque sélection. Les PCB utilisent `.rs` ou `.bc`, les CI `.rsi` ou `.ic` ; Open reconnaît les deux types.

Le [guide des circuits intégrés](silicon/docs/UTILISATION.md) détaille les couches, les cellules, les contacts, les boîtiers et les vues d'instance. La [documentation PCB antérieure](docs/BCAD_README.md) est conservée.

L'exemple `res/10_integrated/` contient une cellule inverseuse sans boîtier, deux niveaux de composition et une carte avec deux instances indépendantes. Son scénario s'exécute avec :

```sh
./bcad --test res/10_integrated/demo.bcadtest res/10_integrated/demo.rs
```

Le même exécutable fournit les commandes CI sans fenêtre :

```sh
./bcad --ic res/10_integrated/buffer4.rsi --check --simulate 1 A=1
```

Sans LibLapin, l'outil de calcul et d'export se construit séparément :

```sh
make ic-cli
./bcad-ic-cli res/10_integrated/buffer4.rsi --check --simulate 1 A=1
```

`make test` lance les tests BCAD et l'intégration PCB/CI. `make test-ic` vérifie le modèle, la hiérarchie et l'adaptateur C LibLapin. Voir les [résultats et limites de validation](silicon/docs/VALIDATION.md).

Le procédé NMOS est pédagogique : géométrie de grille, modèle électrique simplifié et composants R/C/I explicites. Il ne fournit pas un procédé de fabrication. Licence : [LICENSE](LICENSE).
# RedSilicon

Éditeur pédagogique de circuits intégrés NMOS dérivé de BCAD : dessin libre des couches, extraction des transistors, calcul des tensions, sondes et chronogrammes.

```sh
make
./redsilicon
```

L'interface utilise LibLapin avec le compilateur `b++`. Sans LibLapin :

```sh
make cli
./redsilicon-cli --example nmos-nand --simulate 1 A=1 B=1
make test
```

Le [guide d'utilisation](silicon/docs/UTILISATION.md) décrit les couches, les gestes, les quatre exemples et les limites du modèle. Les sources indépendantes du nouvel éditeur sont dans `silicon/`. Il s'agit d'un atelier de conception de puces, avec un procédé pédagogique, et non d'un outil prêt pour une fonderie.

L'ancien logiciel se construit avec `make bcad`. Il accepte maintenant `silicon u1(chemin.rsi) [x,y]` pour utiliser une puce conçue ici. L'exemple `silicon/examples/bcad-nmos.rs` fournit un câblage complet. La [documentation BCAD précédente](docs/BCAD_README.md) est conservée.

Licence : licence du dépôt dans [LICENSE](LICENSE).
