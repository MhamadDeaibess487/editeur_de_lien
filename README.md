# Éditeur de liens ELF32 ARM

Projet universitaire de programmation en **C** consacré à la lecture du format
ELF et à la phase de fusion d’un éditeur de liens. Deux programmes en ligne de
commande permettent d’inspecter des objets ARM ELF32 big-endian et d’expérimenter
la fusion de deux objets relogeables.

Le lecteur est utilisable sur les exemples fournis. Le fusionneur reste
**expérimental** : il produit un objet, mais son contenu n’est pas encore fiable.
Ce projet ne produit pas un exécutable prêt à lancer sur ARM.

L’ancien titre **Prog5Projet** correspond au nom utilisé dans le README
universitaire d’origine. Le [rapport universitaire](docs/rapport-universitaire.md)
conserve le suivi, les explications techniques, les illustrations, les crédits
et les références de ce travail.

## Fonctionnalités présentes

- `elf_reader` : affichage de l’en-tête ELF, de la table des sections, du contenu
  hexadécimal d’une section par son indice, des symboles et des relocations.
- `elf_fusion` : construction d’un mappage des sections des deux fichiers,
  concaténation des sections de même nom et type parmi les types traités,
  ajout de sections, fusion des symboles locaux et globaux et tentative de
  remappage des relocations. Les étapes intermédiaires sont affichées.
- Écriture d’un objet ELF lorsque le troisième argument de `elf_fusion` est fourni.
  Cette partie comporte des défauts connus, décrits ci-dessous.

## Démarrage rapide

### Prérequis

Utiliser un environnement Linux/Unix avec Git, GNU Make, GCC et les en-têtes de
la bibliothèque C. Les essais documentés ont été réalisés sous Linux x86_64
(little-endian). Aucun matériel ARM ni émulateur n’est nécessaire pour lire les
objets déjà présents dans le dépôt.

Les Makefiles choisissent Clang par défaut ; les commandes ci-dessous imposent
GCC, disponible dans l’environnement de vérification. GNU `readelf` (paquet
`binutils` sur Debian/Ubuntu) est utile pour une inspection indépendante.
La chaîne `arm-none-eabi-gcc` / `arm-none-eabi-ld` est seulement nécessaire pour
régénérer les exemples ARM ou comparer la fusion au linker ARM.

### Récupérer et compiler

```sh
git clone https://github.com/MhamadDeaibess487/editeur_de_lien.git
cd editeur_de_lien
make -C elf_reader CC=gcc
make -C elf_fusion CC=gcc
```

Les exécutables sont `elf_reader/elf_reader` et `elf_fusion/elf_fusion`.
Il n’y a pas de Makefile à la racine. Toutes les commandes suivantes partent
de la racine du dépôt.

### Essayer le lecteur

```sh
./elf_reader/elf_reader -h -S -x 4 -s -r TESTS/reader/exemple1.o
```

L’objet est inclus dans le dépôt. L’en-tête doit notamment afficher
`ELFCLASS32`, `ELFDATA2MSB`, `ET_REL` et `EM_ARM ARM`, puis les sections,
le dump de la section 4, les symboles et les relocations.

```text
./elf_reader/elf_reader <options> <fichier.o>
```

| Option | Affichage |
| --- | --- |
| `-h` | En-tête ELF (ce n’est pas une option d’aide) |
| `-S` | Table des en-têtes de sections |
| `-x <indice>` | Dump hexadécimal d’une section, indice décimal obtenu avec `-S` |
| `-s` | Table des symboles |
| `-r` | Relocations ; noms décodés pour R_ARM_ABS32, R_ARM_CALL et R_ARM_JUMP24, autres types numériques |

Les options peuvent être combinées ; placer le fichier en dernier.
Le lecteur sans arguments affiche sa syntaxe.

### Essayer la fusion expérimentale

Afficher les calculs de fusion sans écrire de fichier :

```sh
./elf_fusion/elf_fusion TESTS/fusion/file1.o TESTS/fusion/file2.o
```

Écrire le résultat dans un fichier temporaire et l’inspecter :

```sh
./elf_fusion/elf_fusion TESTS/fusion/file1.o TESTS/fusion/file2.o /tmp/editeur-fused.o
readelf -h -S -s -r /tmp/editeur-fused.o
```

La syntaxe est `./elf_fusion/elf_fusion <objet1> <objet2> [sortie]`.
La sortie choisie est écrasée si elle existe. Utiliser un nouveau chemin,
jamais celui d’un objet d’entrée. L’objet produit reste relogeable : il ne
s’exécute pas comme un programme. Un code de sortie 0 ou une lecture réussie
par `readelf` ne garantit pas une fusion correcte.

### Nettoyer la compilation

```sh
make -C elf_reader clean
make -C elf_fusion clean
```

## Organisation et composants

| Chemin | Rôle |
| --- | --- |
| [elf_reader/](elf_reader/) | CLI `elf_reader.c` ; lecture de l’en-tête (`header_reader.c`), des sections (`header_section_reader.c`, `section_reader.c`), des symboles (`symbole_table.c`) et des relocations (`relocation_reader.c`). Types ELF32 dans `elf.h` et `units_elf32.h`. |
| [elf_fusion/](elf_fusion/) | CLI `elf_fusion.c` ; mappage des sections (`section_fusion.c`), fusion des symboles (`symbole_fusion.c`), relocations (`relocation_fusion.c`) et écriture (`elf_writer.c`, composant interne sans exécutable séparé). Réutilise les sources du lecteur. |
| [TESTS/](TESTS/) | Objets fournis, sources assembleur et scripts shell pour le lecteur, la fusion et l’écriture ; sorties de comparaison historiques. |
| [code_fourni/](code_fourni/) | Archive et squelette pédagogique fournis, avec exemples et outils Autotools ; leur procédure d’installation ne concerne pas les deux Makefiles ci-dessus. |
| [docs/](docs/) | Rapport universitaire archivé, illustrations et compte rendu des vérifications. |

## Limitations et travaux incomplets

- **Format ciblé : ELF32 ARM big-endian.** Les structures sont exclusivement
  ELF32. Le lecteur d’en-tête inverse systématiquement les octets sans choisir
  le traitement d’après `EI_DATA` : les objets little-endian générés par défaut
  par de nombreux outils ne conviennent pas. ELF64 n’est pas pris en charge,
  même si son identifiant peut être affiché.
- **Entrées peu contrôlées.** Plusieurs retours de lecture, indices, allocations
  et ouvertures ne sont pas vérifiés. Un fichier absent, tronqué ou incompatible
  peut provoquer un plantage. L’affichage des symboles ne traite que la première
  table `SHT_SYMTAB` ; la visibilité est toujours affichée `DEFAULT`. Les addends
  `RELA` sont lus, mais ne sont pas affichés par `-r`.
- **Fusion partielle.** Le mappage des sections du second objet traite
  `SHT_PROGBITS`, `SHT_NOBITS`, `SHT_ARM_ATTRIBUTES`, `SHT_REL` et `SHT_RELA`.
  Les symboles `WEAK` n’ont pas de règle de fusion dédiée. Les doublons globaux
  définis sont détectés dans la routine de fusion, mais son échec n’est pas
  correctement propagé par le programme principal.
- **Relocations incomplètes.** Les entrées du premier objet sont recopiées sans
  remapper leurs indices de symboles. Le chemin `RELA` teste `SHT_REL` pour le
  second objet et le programme principal transmet le compteur `REL` au lieu
  du compteur `RELA`. Il ne faut donc pas considérer ce chemin comme fonctionnel.
- **Écriture incorrecte dans certains cas.** Le rapport signale des noms de
  symboles incohérents avec GNU `readelf`. Le code calcule les offsets par somme
  des tailles sans respecter `sh_addralign` et traite les sections `NOBITS`
  comme des données à copier. Les boucles d’écriture des relocations écrivent
  les structures de gestion des tables plutôt que leurs entrées. L’allocation
  de la nouvelle table des noms de sections omet aussi les terminateurs ajoutés,
  ce qui peut entraîner des dépassements mémoire lors de l’ajout de sections.
- **Pas d’édition de liens finale.** Le projet vise la fusion relogeable ; il
  n’implémente ni chargement du résultat ni génération d’un exécutable autonome.

## Vérifications et scripts existants

Le [compte rendu des vérifications](docs/verification.md) indique les commandes
réellement exécutées et leurs limites.

Les scripts de `TESTS/reader/` s’exécutent depuis ce dossier (par exemple
`bash exemple1.sh`) et affichent surtout la sortie du lecteur. Ceux qui
recompilent les sources `.s` utilisent `arm-none-eabi-gcc` sans option big-endian :
ils nécessitent une adaptation avant de servir à valider le format ciblé.
Les scripts de fusion et d’écriture utilisent `arm-none-eabi-ld -EB -r` et
peuvent remplacer les objets et sorties de comparaison suivis par Git.
Plusieurs scripts affichent les différences sans retourner un échec : leur
statut 0 n’est pas une preuve d’équivalence. Il n’existe pas de lanceur
`test.sh` global dans l’état actuel du dépôt (le fichier `test` est vide).

## Crédits et références

Travail de **Deaibess, Saab, Le, Jaber et Hijazi** ; la répartition détaillée
est conservée dans le [rapport](docs/rapport-universitaire.md).
Le code pédagogique fourni crédite **Guillaume Huard** : voir
[AUTHORS](code_fourni/elf_linker-1.0/AUTHORS),
[COPYING](code_fourni/elf_linker-1.0/COPYING) et les notices dans les sources,
notamment `util.c`. La licence fournie est la GNU GPL version 2, avec les
conditions indiquées dans les notices des fichiers concernés.
La référence historique `elf.pdf` est mentionnée dans le rapport mais ce
fichier n’est pas inclus dans le dépôt.
