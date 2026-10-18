# Vecteurs W65C02S

`w65c02-vectors.bin.gz` contient 129 536 cas : les 512 premiers de chaque fichier
`wdc65c02/v1/XX.json` de SingleStepTests/65x02, révision
`2f6980a2d95757486c7bee24355c360e40e2a224`, sauf `$5C`, `$CB` et `$DB`.
La licence MIT d'origine est fournie dans `SingleStepTests-LICENSE.txt`.

Ces données ont été converties par `tests/w65c02_vectors.py --wdc-bus`.
20 334 cas ont des corrections explicites de lectures factices WDC, détaillées
dans `docs/W65C02S.md`. Les résultats de registres et mémoire restent ceux
d'origine. Les trois opcodes exclus sont vérifiés par les tests ciblés du cœur.

SHA-256 du fichier gzip :
`27ccc0b802a38ef2e6de5966a4d7d70413e1786788f11fcb4adcd953979c4f0d`.

Le flux binaire est une succession de cas, sans remplissage ni dépendance au
boutisme de la machine : longueur du nom sur un octet, nom ASCII, registres
initiaux (`PC` sur deux octets little-endian puis `S/A/X/Y/P`), mémoire initiale
(nombre d'entrées sur deux octets puis paires adresse 16 bits / valeur 8 bits),
nombre de cycles sur un octet, cycles (adresse 16 bits, donnée 8 bits, lecture
sur 8 bits : 1=lecture, 0=écriture), puis registres et mémoire finaux au même
format. Le lecteur refuse un fichier tronqué ou vide.

Images Klaus utilisées pour les validations facultatives, non redistribuées ici :

| Image | SHA-256 |
| --- | --- |
| `6502_functional_test.bin` | `fa12bfc761e6f9057e4cc01a665a7b800ff01ae91f598af1e39a1201d01953fd` |
| `65C02_extended_opcodes_test.bin` | `10a2a07fa240666fa610c46accebe8d42b1000feef3aae619da15a8d152869b2` |
