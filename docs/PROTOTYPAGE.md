# Primitives de prototypage

Ces composants servent à apprendre la logique et à dessiner des circuits dans
BCAD. Ils se trouvent dans le panneau **ADD** (`C` ou `Tab`). Les portes sont
des fonctions indépendantes, sans boîtier de circuit intégré ni alimentation
à câbler. Les composants discrets utilisent des modèles numériques simples.

## Catalogue et brochage

Les broches sont numérotées à partir de 1. Sauf indication contraire, la
broche 1 est à gauche et la broche 2 à droite.

| Type du fichier | Fonction | Symbole et broches |
| --- | --- | --- |
| `AND`, `NAND` | ET, NON ET | Rectangle européen `&`, bulle pour NAND ; 1=A, 2=B, 3=sortie |
| `OR`, `NOR` | OU, NON OU | Rectangle européen `≥1`, bulle pour NOR ; 1=A, 2=B, 3=sortie |
| `XOR`, `XNOR` | XOR, NON XOR | Rectangle européen `=1`, bulle pour XNOR ; 1=A, 2=B, 3=sortie |
| `BUF`, `NOT` | OUI, NON | Triangle américain, bulle pour NOT ; 1=entrée, 2=sortie |
| `PROBE` | Sonde logique | Une broche à gauche ; disque coloré et lettre de l'état |
| `DIODE` | Diode idéale numérique | 1=anode A, 2=cathode K, côté barre |
| `SWITCH` | Interrupteur maintenu | 1 et 2 ; contact bidirectionnel |
| `NPN`, `PNP` | Transistors bipolaires simplifiés | 1=base à gauche, 2=collecteur en haut à droite, 3=émetteur en bas à droite |
| `RESISTOR` | Résistance et rappel logique | Rectangle européen ; 1 et 2 interchangeables |
| `LED` | DEL rouge | 1=anode A, 2=cathode K |
| `PUSHBUTTON` | Poussoir carré à quatre pattes | Entraxes 3×3 pas de grille |
| `PUSHBUTTON_COMPACT` | Poussoir rectangulaire à quatre pattes | Entraxes 3×1 pas de grille |
| `CAPACITOR` | Condensateur chimique polarisé | 1=plus, à gauche ; 2=moins, à droite |

Les types des nouvelles primitives acceptent les minuscules. Les alias suivants
sont également reconnus dans les fichiers : `ET`, `NON_ET`, `OU`, `NON_OU`,
`OUI`, `BUFFER`, `NON`, `NON_XOR`, `NXOR`, `SONDE`, `INTERRUPTEUR`,
`RESISTANCE`, `DEL`, `BP`, `BOUTON`, `BP_COMPACT`, `CONDENSATEUR`,
`ELECTROLYTIC`. La sauvegarde utilise le type canonique du tableau.

## Sonde et états logiques

| État | Lettre | Couleur |
| --- | --- | --- |
| Faux | `0` | Rouge |
| Vrai | `1` | Vert |
| Indéfini | `U` | Gris |
| Broken / conflit | `B` | Jaune |
| Haute impédance | `Z` | Violet |

La sonde observe le réseau sans le piloter. Elle apparaît aussi dans les sorties
du mode shell. Une sonde complètement isolée donne `U` ; un contact ouvert
ou une source en haute impédance donne `Z` en l'absence d'autre pilote.
Deux pilotes directs opposés donnent `B`.

Les portes propagent `B`. Une entrée `Z` est considérée comme indéfinie par
les portes ; une entrée déterminante peut néanmoins fixer le résultat :
`0 ET U = 0`, `1 OU U = 1`. OUI et NON ne transmettent pas la haute impédance.

## Contacts et boutons

Un **clic molette** ouvre ou ferme un interrupteur. Son état initial est ouvert.
Le changement recalcule le circuit même quand la simulation est arrêtée, et
son état est enregistré dans le fichier : `SWITCH SW(closed) [0,0]` ou
`SWITCH SW(open) [0,0]`. Les valeurs `0`/`1` et `ouvert`/`ferme` sont aussi
acceptées au chargement.

Pour les poussoirs, **maintenir le clic molette** ferme le contact ; relâcher
le bouton de la souris le rouvre, même si le pointeur est sorti du composant
ou si l'on a changé d'onglet. Les poussoirs sont toujours relâchés au chargement.

Sur les deux empreintes, 1 est en haut à gauche, 2 en bas à gauche, 3 en bas
à droite et 4 en haut à droite. Les pattes **1–2** sont toujours reliées, de même
que **3–4**. L'appui relie les deux côtés. Par rapport à l'origine du composant :

| Poussoir | Patte 1 | Patte 2 | Patte 3 | Patte 4 |
| --- | --- | --- | --- | --- |
| Carré | (0, 0) | (0, 3) | (3, 3) | (3, 0) |
| Compact | (0, 0) | (0, 1) | (3, 1) | (3, 0) |

L'écartement des colonnes reprend les trois pas du boîtier DIP de BCAD.
Sur le compact, deux pattes d'une colonne sont sur des points de grille
consécutifs, sans point intermédiaire.

## Modèles des composants discrets

- **Résistance** : liaison bidirectionnelle faible. Un niveau appliqué directement
  au réseau est prioritaire sur un niveau reçu à travers une résistance.
  Cela permet les rappels au niveau haut ou bas. Deux rappels opposés de même
  force donnent `B`. La valeur affichée (`1k` par défaut) ne change pas la force.
- **Diode** : laisse passer un niveau haut de l'anode vers la cathode et un
  niveau bas de la cathode vers l'anode. Elle permet par exemple un OU à diodes
  avec résistance de rappel au niveau bas.
- **NPN** : base à 1, contact collecteur–émetteur fermé ; base à 0, ouvert.
- **PNP** : base à 0, contact collecteur–émetteur fermé ; base à 1, ouvert.
  Les deux modèles ont un canal C–E bidirectionnel idéal. Une base `U` ou `Z`
  rend le canal indéterminé ; une base `B` lui transmet le conflit.
- **DEL** : témoin allumé lorsque A=1 et K=0. Ses bornes ne conduisent pas :
  elle observe leurs niveaux sans charger le réseau.
- **Condensateur chimique** : circuit ouvert permanent. Sa polarité et sa valeur
  (`10uF` par défaut) servent au dessin et sont sauvegardées.

Il n'y a pas de calcul de tension, de courant, de gain, de seuil ou de
charge/décharge. Les valeurs de résistance et de capacité sont des étiquettes,
par exemple `RESISTOR R1(330)` et `CAPACITOR C1(47uF 16V)`.

## Circuits d'exemple

Ouvrir les fichiers de [res/00_prototypage](../res/00_prototypage) :

- `01_portes.rs` : les huit fonctions et leurs entrées interactives ;
- `02_contacts_del.rs` : interrupteur, résistance, DEL et condensateur ouvert ;
- `03_boutons.rs` : les deux empreintes avec résistances de rappel ;
- `04_transistors.rs` : NPN et PNP commandés par une entrée interactive ;
- `05_diodes_ou.rs` : OU câblé à deux diodes ;
- `06_couleurs.rs` : les cinq états visibles côte à côte.

## Vérification et mise à jour

Après application du patch sur la version W65C02S, faire une reconstruction
complète : `make clean && make`. Le résolveur de réseaux passifs ajoute des
méthodes virtuelles à l'interface des composants.

`make test` couvre les tables de vérité à cinq états, les réseaux passifs et
leurs conflits, les deux empreintes, les sauvegardes et les interactions souris,
ainsi que les vérifications de fiabilité et W65C02S existantes. Les tests utilisent
une surface LibLapin factice ; ils ne testent pas une vraie fenêtre.

Après compilation des tests, `.test-build/prototyping --svg /chemin/absolu/symboles.svg`
produit une planche vectorielle avec la même géométrie que l'éditeur.
