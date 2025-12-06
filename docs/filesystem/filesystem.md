# RAMFS et VFS dans le projet xv6 (Couche logiciel bas niveau)

## 1. Contexte du projet

Ce projet est réalisé dans le cadre du module de **couche logiciel bas niveau** et repose sur l’utilisation de **xv6**, un système d’exploitation pédagogique inspiré d’Unix.

xv6 fournit :

- un noyau minimaliste fonctionnel
- un système de gestion mémoire
- des appels système
- un ordonnanceur
- un système de fichiers de référence

L’objectif n’est pas d’utiliser directement le système de fichiers fourni par xv6, mais de **développer notre propre système de fichiers en mémoire vive (RAMFS)** afin de comprendre les mécanismes internes d’un sous-système de fichiers.

Le projet s’inscrit donc dans une démarche « noyau » : architecture, abstraction, gestion des ressources et cohérence globale.

---

## 2. Qu’est-ce qu’un RAMFS ?

Un **RAMFS (RAM File System)** est un système de fichiers entièrement stocké en mémoire vive.

Contrairement aux systèmes de fichiers classiques (ext4, FAT, NTFS…) qui stockent les données sur disque, le RAMFS :

- crée ses fichiers en RAM
- manipule directement des structures mémoire
- ne possède aucune persistance
- est extrêmement rapide
- perd toutes les données à l’arrêt du système

### Propriétés principales :

| Propriété | RAMFS |
|------------|--------|
| Support de stockage | Mémoire vive |
| Persistance | Aucune |
| Performance | Très élevée |
| Durée de vie des données | Limitée à l’exécution |
| Usage typique | Debug, OS embarqué, systèmes temporaires |

---

## 3. Pourquoi implémenter un RAMFS ?

Le RAMFS est un excellent support pédagogique car il permet de :

- comprendre la structure interne d’un système de fichiers
- éviter les contraintes matérielles (disque, drivers)
- tester une architecture FS propre
- travailler sur la gestion mémoire
- construire un modèle réaliste mais simple

Il impose de gérer soi-même :

- les inodes
- les répertoires
- les blocs
- les descripteurs de fichiers
- les erreurs système

---

## 4. Rôle de xv6 dans le projet

xv6 n’est pas utilisé comme un simple OS utilisateur, mais comme :

- un noyau hôte
- un support d’exécution
- une référence conceptuelle

Le RAMFS s’intègre comme une **couche logicielle indépendante** et pourrait être utilisée comme n’importe quel FS.

L’objectif est de travailler **avec un vrai noyau**, pas un framework simulé.

xv6 sert donc de :

- base d’exécution
- environnement expérimental
- support des abstractions noyau

---

## 5. Le VFS : rôle central

Le **VFS (Virtual File System)** est une interface d’abstraction.

Il sépare :

- l’API publique (`open`, `read`, `write`, etc...)
- l’implémentation réelle (RAMFS ici)

L’application ne connaît pas le système de fichiers réel, elle passe uniquement par le VFS.

### Avantages du VFS :

- découplage FS / API
- architecture modulaire
- possibilité de changer de FS
- verrouillage des accès
- uniformisation des comportements

---

## 6. Architecture générale

L’architecture logique du projet est organisée en couches clairement séparées.

Application utilisateur -> VFS -> RAMFS -> Mémoire RAM


### Rôle du VFS

Le VFS agit comme un **point central de contrôle** entre :

- l’application
- la logique système
- le stockage réel en mémoire

Il garantit :

- une interface uniforme
- une séparation claire entre API et implémentation
- une architecture modulaire
- une extension future possible vers d’autres systèmes de fichiers

Le VFS ne contient pas les données, il orchestre leur accès.

---

## 7. Interface VFS

L’interface utilisée est définie dans le fichier `vfs.h`.

Elle expose une API volontairement proche de POSIX afin de :

- simplifier l’utilisation
- renforcer la cohérence
- simuler un environnement système réel


### 7.1 Constantes

```c
#define BLOCK_SIZE      (512)
#define FILE_NAME_SIZE  (24)
#define DIR_NAME_SIZE   (FILE_NAME_SIZE)
#define INODE_ID_SIZE   (8)
#define FILE_REF_SIZE   (INODE_ID_SIZE + FILE_NAME_SIZE)
#define NB_FILES_IN_DIR (BLOCK_SIZE / FILE_REF_SIZE)
```

Ces constantes imposent :
- une structure en blocs fixes
- une taille maximale de noms
- une capacité bornée par répertoire
- un couplage inode / nom pour chaque entrée

Ce fonctionnement est volontairement proche des FS classiques.

### 7.2 Gestion des erreurs

```c
typedef enum {
    VFS_OK,
    VFS_INVALID_PATH,
    VFS_INVALID_FD,
    VFS_INVALID_PATH_DIR,
    VFS_INVALID_PATH_FILE,
    VFS_MAX_FD_REACHED,
    VFS_MEMORY_FULL,
    VFS_UNKWON_ERROR
} vfs_error;

```

Chaque code d’erreur correspond à une situation système réelle :

| Code                 | Signification        |
| -------------------- | -------------------- |
| `VFS_INVALID_PATH`   | chemin incorrect     |
| `VFS_INVALID_FD`     | descripteur invalide |
| `VFS_MAX_FD_REACHED` | limite atteinte      |
| `VFS_MEMORY_FULL`    | mémoire saturée      |
| `VFS_UNKWON_ERROR`   | état critique        |

Aucune exception : tout passe par retour d’état, comme dans un noyau réel.



### 7.3 Types abstraits

```c
typedef struct filedescriptor_s fd_t;
typedef char *path_t;
```

- `fd_t` représente le descripteur système associé à un fichier ouvert.
- `path_t` correspond à un chemin absolu.

Ces abstractions permettent de découpler l’API de l’implémentation interne.

---

## 8. Gestion des fichiers

L’API fichiers permet de manipuler les fichiers via le VFS.

| Fonction      | Rôle            |
| ------------- | --------------- |
| `create_file` | crée un fichier |
| `delete_file` | supprime        |
| `open_file`   | ouvre           |
| `close_file`  | libère          |
| `read_file`   | lit             |
| `write_file`  | écrase          |
| `append_file` | ajoute en fin   |

Tous les accès passent obligatoirement par le VFS.

---


## 9. Gestion des répertoires

| Fonction     | Description |
| ------------ | ----------- |
| `create_dir` | crée        |
| `delete_dir` | supprime    |
| `read_dir`   | liste       |


Le modèle est volontairement simple :

- pas de permissions avancées
- pas de liens symboliques
- pas de hiérarchie complexe

Mais architecture saine et fonctionnelle.

### Logique Système

Ce projet repose sur une séparation stricte des rôles entre :

- l’application
- le VFS
- le RAMFS
- la mémoire

C’est exactement la logique utilisée par :

- Linux (VFS)
- Windows (I/O Manager)
- BSD
- les systèmes embarqués modernes

On ne développe pas un exercice,
mais une mini architecture noyau.


## 10. Conclusion

Ce projet met en œuvre bien plus qu’un simple système de fichiers en mémoire. 
Il propose une approche réaliste de l’architecture d’un sous-système noyau en s’appuyant sur une séparation claire entre l’API, le VFS et l’implémentation RAMFS.

Le choix d’un système de fichiers en mémoire permet de se concentrer sur les mécanismes fondamentaux : la gestion des structures internes, la résolution de chemins, les descripteurs de fichiers et l’organisation logique des répertoires, sans introduire la complexité matérielle liée aux périphériques de stockage.

L’introduction d’une couche VFS impose une architecture propre et modulaire, proche de celle utilisée dans les systèmes d’exploitation modernes. Cela permet non seulement de simuler le fonctionnement réel d’un OS, mais aussi de rendre l’implémentation évolutive et indépendante du support de stockage.

Ce projet constitue ainsi une première immersion dans la logique noyau : rigueur, structuration, responsabilité du code et maîtrise complète du cycle de vie des données. 
Il ne s’agit pas seulement de manipuler des fichiers, mais de comprendre comment un système d’exploitation organise, protège et orchestre ses ressources.

Ce travail dépasse donc largement le cadre d’un exercice académique. 
Il s’inscrit dans une démarche d’ingénierie système et pose des bases solides pour aborder des sujets plus complexes comme les systèmes de fichiers persistants, la gestion de cache ou la synchronisation noyau.



