# Partie 3 : RAM FS

## Objectif

L’objectif de ce système de fichiers en mémoire (RAM FS) est de fournir une abstraction simple et légère permettant de manipuler des fichiers et des dossiers directement en RAM, sans dépendre d’un support de stockage persistant.

Ce RAM FS est destiné à être utilisé **au sein du kernel**, notamment pour :
- stocker des fichiers temporaires,
- tester les primitives de gestion de fichiers,
- fournir une première couche de système de fichiers avant l’implémentation d’un FS persistant.

---

## Fonctionnalités du FS

Le RAM FS implémenté fournit les fonctionnalités suivantes :

### Gestion des inodes
- Tableau statique d’inodes en mémoire (`RAM_inodes_table`)
- Nombre maximal d’inodes fixé à la compilation (`FS_NPAGES`)
- Chaque inode représente soit :
  - un fichier (`FILE`)
  - un dossier (`FOLDER`)
  - une entrée libre (`EMPTY`)

### Arborescence hiérarchique
- Organisation des fichiers sous forme d’un **arbre**
- Chaque inode contient :
  - un pointeur vers son père
  - un pointeur vers son premier fils
  - un pointeur vers son frère suivant
- Le premier inode (`inode 0`) est réservé au **répertoire racine** `/`

### Initialisation du RAMS

- Réservation des pages mémoires nécessaires pour chaque inode du tableau et initailisation de chaque inode, ainsi que la création de la racine.
- Toutes les inodes (sauf l'inode 0) sont de type `EMPTY`.

```c
void init_fs() {

  //reserve FS_NPAGES for each inode and set root inode
  for (unsigned i = 0; i < FS_NPAGES; i++) {
    unsigned long block = allocate_kernel_page();
    if (i == 0) {
      strcpy(RAM_inodes_table[i].name, "/");
      RAM_inodes_table[i].type = FOLDER;
    } else {
      RAM_inodes_table[i].type = EMPTY;
    }

    RAM_inodes_table[i].id = i;
    RAM_inodes_table[i].data_block = block;

    RAM_inodes_table[i].brother = (void*)0;
    RAM_inodes_table[i].father = (void*)0;
    RAM_inodes_table[i].first_son = (void*)0;
    RAM_inodes_table[i].size = 0;
    RAM_inodes_table[i].ref = 0;
  }
}
```

### Création et suppression
- Création de fichiers et dossiers via `alloc_inode` en prenant le premier inode disponible dans le tableau pour le reserver

```c
inode_id alloc_inode(const char name[MAX_SIZE_NAME], inode *father, TYPE_FILE type) {

  for (int i = 0; i < FS_NPAGES; i++) {
    if (RAM_inodes_table[i].type == EMPTY) {
      RAM_inodes_table[i].type = type;
      strcpy(RAM_inodes_table[i].name, name);

      if (father != 0) {
        RAM_inodes_table[i].father = father;
        int brother = find_last_child(father);
        if (brother == -1) {
          father->first_son = &RAM_inodes_table[i];
        } else {
          RAM_inodes_table[brother].brother = &RAM_inodes_table[i];
        }
      } else {
        RAM_inodes_table[i].father = 0;
      }
      return i;
    }
  }
  return -1;
}
```

- Suppression de l'inode via `free_inode` en changeant les éléments de la structure sans changer la hierarchie des autres fichiers/dossiers

```c
int free_inode(inode_id inode) {
    if (inode < 0 || inode >= FS_NPAGES) {
        return -1;
    }

    if (RAM_inodes_table[inode].type == FOLDER && RAM_inodes_table[inode].first_son != (void*)0) {
        return -2; //impossible to destroy a folder with element inside
    }
    
    struct RAM_inode* brother = RAM_inodes_table[inode].brother;
    struct RAM_inode* father = RAM_inodes_table[inode].father;

    if (father->first_son == &RAM_inodes_table[inode]) {
        father->first_son = brother;
    } else {
        struct RAM_inode* next_child = father->first_son;
        while(next_child != (void*)0) {
          if (next_child->brother == &RAM_inodes_table[inode]) {
              next_child->brother = brother;
              break;
          }
          next_child = next_child->brother;
        }
    }

    RAM_inodes_table[inode].brother = (void*)0;
    RAM_inodes_table[inode].father = (void*)0;
    RAM_inodes_table[inode].size = 0;
    RAM_inodes_table[inode].ref = 0;
    RAM_inodes_table[inode].type = EMPTY;

    return 0;
}
```

- Suppression d’un dossier impossible tant qu’il contient des éléments

### Lecture et écriture
- Lecture partielle avec une position donnée (`pos`) pour accéder au premier caractère à lire du bloc via la fonction `read_inode()`

```c
int read_inode(inode_id inode, int pos, int size, char* buff_dest) {
  unsigned long size_to_read = size;

  if (inode < 0 || inode >= FS_NPAGES)
    return -1;

  if (RAM_inodes_table[inode].type != FILE)
    return -2;

  if (pos + size >= RAM_inodes_table[inode].size)
    size_to_read = RAM_inodes_table[inode].size - pos;


  unsigned long start_pos = RAM_inodes_table[inode].data_block + pos;
  unsigned long end_pos = start_pos + size;

  if (end_pos >= RAM_inodes_table[inode].data_block + FS_NPAGES)
    size_to_read = (RAM_inodes_table[inode].data_block + FS_NPAGES) - start_pos;
  
  memcpy(start_pos, (unsigned long)buff_dest, size_to_read);
  
  return size_to_read;
}
```
- Taille maximale d’un fichier limitée à une page mémoire (`FILE_MAX_SIZE`)

- écriture 
//à rajouter

- Supréssion totale du contenu du fichier
//à rajouter

### Gestion mémoire
- Chaque inode possède un bloc mémoire dédié (tableau global -> donc en section .data)
- Les blocs sont alloués via la fonction d'allocation des pages mémoires `allocate_kernel_page()` 
- Aucune persistance : toutes les données sont perdues au redémarrage

---

## Implémentation du RAM FS

Chaque inode est représenté en mémoire par la structure suivante :

```c
typedef struct RAM_inode {
  inode_id id;
  int ref;
  TYPE_FILE type;
  unsigned size;
  struct RAM_inode* father;
  struct RAM_inode* brother;
  struct RAM_inode* first_son;
  char name[MAX_SIZE_NAME];
  unsigned long data_block;
} inode;
```

//explication de chaque élément de la structure