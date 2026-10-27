# Validation de BCAD unifié

Les tests suivants ont été exécutés avec AddressSanitizer et UndefinedBehaviorSanitizer :

| Ensemble | Résultat |
| --- | --- |
| Modèle CI et interactions de l'éditeur | 280 vérifications réussies. |
| Hiérarchie, références, boîtiers et vues d'instance | 69 vérifications réussies. |
| Adaptateur LibLapin à prototypes C stricts | 43 vérifications réussies. |
| Buffers PCB/CI et simulation commune sur carte | 65 vérifications réussies. |
| Barre commune, recherche et favoris persistants | 76 vérifications réussies. |
| CPU W65C02S ciblé | 644484 vérifications réussies. |
| Corpus de bus W65C02S | 129536 cas réussis, aucun échec. |
| Prototypage / routage local | 11908 / 1136 vérifications réussies. |
| Catalogue et circuits fournis | 171 types et 35 circuits chargés et simulés. |

La suite BCAD vérifie aussi les sauvegardes interrompues, les erreurs d'E/S, les chargements transactionnels, 1747 entrées tronquées ou modifiées, les SRAM/EEPROM, les bus CPU et la libération des ressources graphiques.

Les nouveaux scénarios couvrent :

- des cellules sans contact externe et sans boîtier, les références imbriquées, la rotation, les cycles et les fichiers manquants ;
- des placements indépendants d'une même cellule, le raccordement réel de leurs masques et l'observation de leurs nœuds dans la simulation du parent ;
- la numérotation physique d'un DIP14 avec broches NC et le refus transactionnel d'une cellule sans boîtier sur PCB ;
- deux puces directement reliées et résolues ensemble, un contact bidirectionnel dans les deux sens et la conservation de charge lorsque son pilote passe en Z ;
- les buffers mixtes, les événements via les callbacks C natifs, le rendu PCB/CI, l'enregistrement, la fermeture d'un buffer modifié et la propagation d'une définition non enregistrée ;
- la séparation entre définition et observation, la navigation dans la hiérarchie et la commande d'un pas de la carte depuis une observation ;
- les chemins relatifs après Enregistrer sous et l'importation des fichiers `REDSILICON 1` ;
- F1/F2 pour les deux types de documents, F3/F4 pour les fichiers, les libellés Zoom in/out et le panneau Component commun ;
- l'ajout aux favoris sans création dans le circuit, l'absence de doublons, le retrait, les déplacements et échanges entre les deux côtés, les relâchements de souris au-dessus des colonnes et les recherches effacées à la sélection ;
- la conservation de la simulation lors d'une modification des favoris, leur sauvegarde PCB/CI, les chemins contenant espaces, guillemets et dièses, et le rejet transactionnel des emplacements invalides.

Les commandes habituelles sont :

```sh
make test
make test-ic
./bcad --test res/10_integrated/demo.bcadtest res/10_integrated/demo.rs
```

LeakSanitizer a été désactivé dans cet environnement, qui ne permet pas son inspection des processus. ASan et UBSan restent actifs et bloquants. Les commandes utilisées ici sont :

```sh
BCAD_DETECT_LEAKS=0 make test
REDSILICON_DETECT_LEAKS=0 make test-ic
```

Les huit fonctions de contexte exposées à LibLapin utilisent des paramètres `void*` et des relais vers les gestionnaires C++ typés, sans conversion de pointeurs de fonctions. Le contexte est initialisé à zéro avant affectation de ses callbacks. Le substitut de test utilise ces prototypes C, y compris des membres de contexte supplémentaires pour détecter les initialisations incomplètes. L'ancienne fenêtre autonome reste couverte pour la compatibilité ; l'application livrée utilise le contexte natif de BCAD.

La vraie LibLapin n'est pas installée dans cet environnement. La compilation complète de BCAD est vérifiée contre son substitut C strict avec `-Werror=missing-field-initializers` et `-Werror=cast-function-type`. La création de fenêtre et la livraison réelle des événements restent à vérifier sur la machine de destination. Le rendu logiciel des masques et de la hiérarchie est vérifié séparément.

Le modèle analogique n'a pas été comparé à des mesures physiques ou à un simulateur SPICE externe. Les composants PCB historiques conservent leurs modèles logiques ; la résolution analogique commune concerne les CI personnalisés et leurs connexions.

Le cadrage initial des CI tient compte des colonnes de favoris, et les libellés de la barre supérieure adaptent leur largeur aux fenêtres étroites. La compilation par défaut conserve un seul exécutable `bcad`, produit à partir des objets séparés ; une seconde invocation sans modification ne recompile pas les sources.
