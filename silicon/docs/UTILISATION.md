# Circuits intégrés dans BCAD

BCAD ouvre des buffers de deux types : circuits imprimés (PCB) et circuits intégrés (CI). Les documents PCB gardent leur format et leur éditeur. Les documents CI contiennent des masques NMOS, des composants électriques et des instances de cellules. Les deux types se partagent les commandes de fichiers et la liste des buffers.

## Démarrer et changer de buffer

```sh
make clean
make -j16
./bcad
./bcad res/10_integrated/demo.rs
./bcad res/10_integrated/buffer4.rsi
```

`F1` crée un PCB ; `F2` crée un CI vide, sans boîtier. `F4` ouvre les deux types de fichiers dans le même sélecteur et affiche les buffers déjà ouverts, avec leur type : PCB, IC ou OBS pour une observation. Le bouton Files séparé a été retiré. `Ctrl+Tab` et `Ctrl+Shift+Tab` changent de buffer. Chaque buffer garde sa caméra et sa simulation.

`F3` enregistre ; `Shift+F3` choisit un autre chemin et adopte ce fichier ; `Ctrl+Shift+F3` enregistre une copie ; `Shift+F1` ou `Ctrl+W` ferme le buffer courant. Les commandes de fermeture et de sortie passent par la demande d'enregistrement commune lorsqu'un document est modifié. Une observation est une vue en lecture seule et ne crée pas de modification à enregistrer.

Les fichiers CI utilisent l'extension `.rsi` (ou `.ic`) et l'en-tête `BCAD_IC 2`. Les anciens fichiers `REDSILICON 1` restent lisibles. Pour préserver leur ancien usage sur PCB, leurs contacts reçoivent automatiquement un boîtier DIP lors de l'importation. Le PCB garde son format `.rs` / `.bc`.

## Cellule, contact et boîtier

Ces notions sont indépendantes :

| Élément | Rôle |
| --- | --- |
| Cellule | Document CI réutilisable. Elle peut ne contenir que des formes, sans contact ni boîtier. |
| Borne interne (`terminal`) | Repère nommé sur un conducteur. Elle aide à relier, stimuler et observer une cellule ; elle ne crée aucune patte de boîtier. |
| Contact externe | Objet Entrée, Sortie, Bidirectionnel, Alimentation ou Masse posé sur un conducteur. Il peut être associé à une broche du boîtier. |
| Boîtier | Métadonnées du document CI définissant son nom, sa largeur et ses broches. Il permet de placer ce CI sur un PCB. |
| Instance | Référence vers un autre document CI, avec nom, position et rotation. Chaque placement possède son propre état électrique. |

Une cellule peut être utilisée dans un autre CI sans contact externe. Les conducteurs de l'instance sont visibles dans le parent ; les pistes du parent les rejoignent selon les mêmes règles de couches que les formes locales. Les bornes internes sont pratiques pour repérer ces raccordements, mais ne sont pas obligatoires.

`F9` ouvre le catalogue **Component**, avec les outils CI, les définitions ouvertes et les fichiers de cellules du dossier courant. Choisir une cellule dans ce catalogue, puis cliquer dans le dessin pour la placer. `Ctrl+F9` ou le bouton **Insérer une cellule** ouvre aussi le sélecteur de fichiers pour choisir une autre définition. L'objet reste une référence : déplacer, dupliquer ou tourner l'instance transforme ses masques sans en copier la définition dans le fichier parent. `Ctrl+F7` renomme l'instance sélectionnée ; ses noms doivent être uniques dans le parent.

Les groupes d'objets restent des annotations pratiques ; les instances constituent la hiérarchie réutilisable. Les références peuvent s'imbriquer. Une référence manquante ou un cycle dans la hiérarchie produit une erreur explicite.

Pour préparer une puce à placer sur un PCB :

1. Poser des contacts externes sur les conducteurs à exposer, y compris VCC et GND si le circuit en a besoin.
2. Avec `Ctrl+F12` ou **Ajouter un boîtier**, saisir `DIP` pour une numérotation automatique, ou `DIP:14` pour réserver quatorze broches.
3. Sélectionner chaque contact puis `Ctrl+F8` pour changer son numéro de broche. Les numéros doivent être distincts et appartenir au boîtier. Les emplacements non affectés restent NC.
4. Enregistrer le CI. Dans un buffer PCB, `Ctrl+F9` insère son boîtier et donne accès à ses broches numérotées.

Saisir `none` dans la boîte du boîtier retire celui-ci. Le document redevient utilisable comme cellule. Le boîtier actuellement dessiné sur PCB est un DIP à deux rangées ; le nom du boîtier ne sélectionne pas une bibliothèque d'empreintes physiques.

Dans une sous-cellule, les contacts externes sont traités comme des points d'observation : ils ne créent ni alimentation implicite ni broches supplémentaires pour le parent. Le parent expose uniquement ses propres contacts. Une puce sur PCB reçoit VCC et GND par le câblage de la carte.

## Dessiner les couches NMOS

Les coordonnées sont des carrés de grille sans unité physique de fabrication.

| Couche | Effet électrique |
| --- | --- |
| `active` | Diffusion conductrice. |
| `poly` | Polysilicium conducteur ; une traversée complète de l'actif crée un NMOS et coupe la continuité ohmique de l'actif. |
| `metal1` | Première couche métallique. |
| `metal2` | Seconde couche métallique, isolée de la première sans via. |
| `contact` | Relie metal1 à une région active ou poly présente sous le contact. |
| `via` | Relie metal1 et metal2. |

Un bord commun sur une même couche crée une connexion ; le contact par le seul coin n'en crée pas. Le croisement de couches ne les relie pas automatiquement. Un contact doit recouvrir les matériaux qu'il raccorde.

Choisir une couche active l'outil Rectangle. Glisser avec le bouton gauche dessine une région ; contact et via se posent par clic. Le `O` de la palette masque ou réaffiche la couche sans changer son extraction. Pour un NMOS, dessiner une bande d'actif traversée complètement par du poly ; conserver un accès de part et d'autre du canal. L'extracteur calcule ses trois nœuds et son rapport W/L. Un canal ambigu est signalé.

Les contacts externes, bornes internes et sondes désignent le conducteur **de la couche sélectionnée** à leur position. Choisir metal1 avant de poser un contact sur une piste metal1. Un point sans conducteur produit une erreur. Les noms des contacts externes utilisent lettres ASCII, chiffres et underscore et sont uniques dans leur document.

Les outils Résistance, Capacité et Source I se tracent entre deux conducteurs distincts. Le bouton **Valeur / unités SI** de l'inspecteur édite leur valeur : ohms, farads ou ampères (`1e-9` = 1 nF). La source I va du premier point vers le second. Ces éléments sont explicites ; les masques ne génèrent pas de capacités parasites cachées.

## Manipuler et examiner

| Geste ou touche dans un CI | Action |
| --- | --- |
| Sélection + clic / Shift+clic | Sélectionner / ajouter ou retirer un objet. |
| Sélection + glisser dans le vide | Sélection rectangulaire. |
| Glisser une sélection / flèches | Déplacer les objets. |
| Ctrl+A / Ctrl+D | Tout sélectionner / dupliquer. |
| Ctrl+G | Nommer un groupe d'objets. |
| Page Up / Page Down | Tourner de 90°, y compris les instances. |
| Suppr / Ctrl+Z / Ctrl+Y | Supprimer / annuler / refaire (64 modifications). |
| Molette / milieu + glisser dans le vide | Zoomer / déplacer la caméra. |
| F5 | Centrer la caméra sur le pointeur. |
| Home ou F6 | Cadrer le dessin. |
| F7 / F8 | Zoom in / Zoom out autour du pointeur. |
| F9 / F10 | Ouvrir Component / observer la cellule sélectionnée. |
| Ctrl+F5 / Ctrl+F6 | Reconstruire le réseau / remettre la simulation à zéro. |
| Ctrl+F7 | Renommer l'objet sélectionné. |
| Ctrl+F8 / Ctrl+F12 | Numéro du contact / boîtier. |
| Ctrl+F10 | Ouvrir la définition de la cellule sélectionnée, ou de l'observation courante. |
| Ctrl+F11 | Remonter depuis une observation. |
| F11 / F12 | Un pas / lancer ou suspendre la simulation. |

Les boutons de l'inspecteur modifient la valeur, ajoutent une trace et activent un arrêt sur changement du nœud sélectionné. Cliquer un diagnostic centre sa position. Les tensions s'affichent auprès des contacts et sondes : rouge = 0, vert = 1, gris = U, violet = Z, jaune = B.

## Observer une instance en fonctionnement

Sur un PCB, sélectionner un CI personnalisé et presser `Ctrl+Shift+F10`. Un buffer OBS montre les masques, les tensions et les traces de **cette instance**. `Ctrl+Shift+F11` ouvre plutôt sa définition éditable.

Dans un CI ou une observation, sélectionner une instance puis `F10` pour descendre dans cette cellule. `Ctrl+F11` remonte d'un niveau et revient ensuite à la carte ou au buffer CI propriétaire de la simulation. Deux instances d'une même cellule partagent leur définition mais gardent leurs propres nœuds et leur état.

Les buffers OBS utilisent le solveur de leur source. La carte ou le CI propriétaire continue donc de tourner lorsqu'on observe l'intérieur ; `F11` et `F12` dans l'observation commandent cette même simulation. Si le buffer propriétaire est fermé, la vue conserve le dernier état disponible. Les modifications de géométrie passent par le buffer de définition, accessible avec `Ctrl+F10`.

Les modifications d'une définition ouverte sont publiées aux parents et aux puces de la carte, même avant enregistrement. Une modification structurelle valide reconstruit la simulation. Si une nouvelle définition invalide un CI câblé, BCAD conserve sa dernière définition valide et suspend la carte concernée. Après correction, relancer la simulation. Les dépendances sont également vérifiées au changement de buffer ; les fichiers externes ne sont pas surveillés en permanence.

## Favoris sur les côtés

Les dix emplacements de gauche et les dix de droite appartiennent au document courant, PCB ou CI. Leur disposition est enregistrée dans le même fichier que le circuit ; elle n'est pas un réglage global de BCAD.

- **Ctrl+clic dans Component** ajoute le composant au premier emplacement libre, sans créer d'objet dans le circuit. Le même composant ne peut pas occuper plusieurs favoris.
- **Clic sur un favori** démarre son placement (PCB), sélectionne son outil de dessin (CI), ou prépare une cellule à placer au prochain clic dans le dessin.
- **Ctrl+clic sur un favori** le retire.
- **Glisser-déposer entre deux boutons**, y compris d'un côté à l'autre, déplace le favori. Une destination occupée échange les deux boutons. Un dépôt en dehors des boutons annule le déplacement.

Une sélection dans Component efface la recherche et remet son défilement au début. Une sélection normale ferme le panneau ; un ajout aux favoris le laisse ouvert pour poursuivre le choix.

Les fichiers de cellules favoris sont référencés relativement au document quand c'est possible, y compris après Enregistrer sous. Les anciens circuits sans favoris ouvrent deux colonnes vides. Les modifications des favoris rendent le document modifié, sans réinitialiser sa simulation. Les favoris d'une observation se modifient dans le buffer de définition.

## Simuler

La hiérarchie est développée en réseau électrique avant résolution : toutes les régions, transistors et composants R/C/I des cellules participent au calcul. Entre deux boîtiers personnalisés sur la même carte, les broches directement câblées réunissent les nœuds internes dans le même solveur. Les contacts bidirectionnels peuvent conduire dans les deux sens ; les résistances de rappel de la carte conservent leur caractère faible.

Les autres composants PCB gardent le modèle logique de BCAD et présentent des niveaux 0/5 V au réseau des CI. Leur fonctionnement participe à la simulation de la carte, mais leurs propres modèles n'acquièrent pas pour autant une description analogique au transistor. Les composants personnalisés ne sont pas remplacés par une table de vérité.

Un pas vaut 1 µs. Le fonctionnement continu est cadencé par les images, sans garantie de temps réel. Dans une définition CI autonome, le clic milieu sur une entrée ou une borne interne fait défiler `0 → 1 → Z → U`. Les contacts VCC et GND de ce banc autonome imposent respectivement 5 V et 0 V. Sur une carte, c'est le câblage extérieur qui les alimente.

Le calcul utilise une résolution nodale et un modèle NMOS continu simplifié. Les capacités explicites utilisent Euler implicite après le calcul initial du point de fonctionnement. Un changement de stimulus conserve leur charge ; une reconstruction géométrique ou topologique réinitialise le solveur. Une non-convergence produit U et arrête le lancement continu.

Les seuils logiques affichés sont 1,5 V pour le maximum bas et 3,5 V pour le minimum haut. Entre les deux, le nœud est U. Z indique l'absence de chemin conducteur reconnu vers un pilote ou de charge connue. B signale notamment des pilotes externes opposés directement raccordés. Ces états ne constituent pas un modèle d'échauffement ou de destruction.

Les chronogrammes montrent les cinq premières traces cochées. Le solveur conserve au plus 4096 échantillons, en retirant les plus anciens par blocs.

## Exemple complet et commandes sans fenêtre

`res/10_integrated/` contient :

- `cell-inverter.rsi` : inverseur NMOS avec bornes internes, sans boîtier ni contact extérieur ;
- `buffer2.rsi` : deux références à cette cellule, raccordées par du métal ;
- `buffer4.rsi` : deux références à `buffer2`, avec contacts numérotés et boîtier DIP8 ;
- `demo.rs` : deux instances de `buffer4` sur une carte, avec entrées et sorties indépendantes ;
- `demo.bcadtest` : vérification des sorties avant et après inversion des entrées.

```sh
./bcad --test res/10_integrated/demo.bcadtest res/10_integrated/demo.rs
./bcad --ic res/10_integrated/buffer4.rsi --check --simulate 1 A=1
./bcad --ic res/10_integrated/buffer4.rsi --render buffer.ppm --spice buffer.cir
```

`--csv trace.csv` exporte l'historique du calcul. Le sous-circuit SPICE contient le réseau développé et un modèle NMOS niveau 1, sans alimentation cachée. Les broches suivent l'ordre des contacts du document ; les numéros physiques appartiennent au boîtier BCAD et ne changent pas l'ordre des arguments SPICE.

Sans LibLapin, `make ic-cli` construit `bcad-ic-cli`, qui accepte les mêmes arguments que `bcad --ic`. Les exemples simples restent dans `silicon/examples/` : inverseur, NAND, NOR et échantillonneur avec capacité explicite.

Les références enregistrées sont relatives au fichier parent quand c'est possible et sont recalculées lors d'un Enregistrer sous. Déplacer un projet complet avec ses dépendances préserve donc ses références. Les écritures synchronisent un fichier temporaire puis le renomment ; un échec avant publication conserve l'ancien fichier. La grammaire PCB actuelle exige des chemins de définition sans espaces.

## Portée du modèle

Les budgets sont de 4096 objets développés, 200000 carrés de matériau et 1024 nœuds pour une simulation ; un boîtier comporte au plus 64 broches. Sur une carte, les CI personnalisés partagent le budget de nœuds. Le nombre d'instances est borné et les cycles sont interdits.

Le NMOS est à enrichissement, Vth = 1 V, avec un modèle élémentaire de canal long et des charges résistives. Les transistors à déplétion, l'effet de substrat, les fuites physiques, le partage de charge réaliste, les dimensions en microns, les règles de fonderie et GDS ne sont pas implémentés. Les contrôles géométriques assurent la cohérence du modèle pédagogique et ne sont pas un DRC de fabrication.

Le code conserve des expérimentations bipolaires antérieures au recentrage sur NMOS. Elles ne sont pas exposées dans la palette et ne constituent pas une simulation fidèle de TTL ou de Ferranti.

Voir [VALIDATION.md](VALIDATION.md) pour les commandes de test et la distinction entre les tests de l'API C LibLapin et la validation graphique sur une installation réelle.
