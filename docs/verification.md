# Vérifications du démarrage

Vérifications effectuées le 30 octobre 2026 sous Linux x86_64, avec GCC 14.2.0,
GNU Make 4.4.1 et GNU readelf (Binutils) 2.44. Elles concernent le démarrage
présenté dans le [README](../README.md), pas une validation complète du linker.
Aucune source C ni aucun Makefile n’a été modifié pour ces essais.

## Commandes exécutées

Le dépôt a été cloné depuis l’URL HTTPS indiquée dans le README. Depuis sa racine :

```sh
make -C elf_reader clean
make -C elf_fusion clean
make -C elf_reader CC=gcc
make -C elf_fusion CC=gcc
./elf_reader/elf_reader
./elf_reader/elf_reader -h -S -x 4 -s -r TESTS/reader/exemple1.o
./elf_fusion/elf_fusion TESTS/fusion/file1.o TESTS/fusion/file2.o
./elf_fusion/elf_fusion TESTS/fusion/file1.o TESTS/fusion/file2.o /tmp/editeur-fused.o
readelf -h -S -s -r /tmp/editeur-fused.o
```

Ces commandes ont retourné 0. Le lecteur sans arguments affiche sa syntaxe.
L’exemple de lecture affiche bien ELF32, ARM, big-endian, les sections, le dump,
les symboles et les relocations. La fusion affiche ses étapes ; la variante
avec sortie crée un objet accepté par `readelf`, sans diagnostic sur stderr
lors de cet essai. Cela ne garantit pas la justesse du contenu : des relocations
y apparaissent comme `R_ARM_NONE`, et les défauts relevés dans les sources sont
exposés dans les [limitations](../README.md#limitations-et-travaux-incomplets).

L’essai du script existant a également retourné 0 :

```sh
cd TESTS/reader
bash exemple1.sh
```

Il affiche la sortie du lecteur, sans comparaison automatique avec `readelf`.
Les commandes `clean` ont ensuite été réexécutées pour retirer les produits de
compilation. Les fichiers de sortie des essais ont été placés dans `/tmp` ;
les objets et résultats historiques suivis dans `TESTS/` n’ont pas été modifiés.

## Ce qui n’a pas été validé

- La compilation par défaut a été tentée avec `make -C elf_reader clean all` :
  échec car `clang` n’est pas installé. Les commandes GCC ci-dessus ont ensuite
  réussi après nettoyage.
- `arm-none-eabi-gcc`, `arm-none-eabi-ld` et `arm-none-eabi-readelf` ne sont pas
  installés. La régénération des objets ARM et la comparaison au linker ARM
  n’ont donc pas été exécutées. Les scripts de fusion et d’écriture ne sont
  pas annoncés comme passants.
- Aucun essai d’exécution sur ARM, d’ELF64, de little-endian ou de performance
  n’a été réalisé.

## Liens et conservation des documents

Les liens Markdown locaux du README et des documents de `docs/` ont été vérifiés
(chemins, images et ancres). L’URL GitHub du dépôt est accessible et le clonage
a réussi. La référence historique `elf.pdf`, absente du dépôt, est explicitement
signalée dans le guide et l’archive.

Le corps de l’ancien README est conservé dans le
[rapport universitaire](rapport-universitaire.md) ; seuls le titre, une note de
contexte et le chemin de l’illustration ont changé. Les deux images d’origine
ont été déplacées dans `docs/images/` sans modification de leur contenu.
