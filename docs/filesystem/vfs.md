# Partie 2 : Virtual File System (VFS)  

## Objectif  

Dans cette partie, nous allons implémenter un virtual filesystem (VFS) minimaliste capable d'interragir avec le filesystem et fournissant une interface aux applications.

## Définition  

Il est important de distinguer le rôle du filesystem et du VFS. Le VFS est une couche d'abstraction au dessus des filesystem :

```txt

process_1       process_2       process_3
    ┃               ┃               ┃
    ┗━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━┛
                   VFS
    ┏━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━┓
    ┃               ┃               ┃
filesystem_1    filesystem_2    filesystem_3
    ┃               ┃               ┃
 Support_1       Support_2       Support_3

```

Le **filesystem** est le mécanisme qui organise et stocke les données sur un support physique (disque, SSD, RAM, en réseau, ...). Chaque support peut être géré par un ou plusieurs filesystems différents. Un filesystem définit une abstraction des données (fichiers, dossiers, métadonnées) ainsi que les règles et fonctions permettant leur manipulation (lecture, écriture, permissions, organisation).

Le **VFS** (Virtual Filesystem) est une couche d’abstraction qui uniformise l’accès aux différents filesystems et fournit des fonctionnalités communes. Il expose une interface unique de manipulation des fichiers et traduit les appels des applications en appels spécifiques au filesystem sous-jacent sur lequel est stocké le fichier ou le dossier.  

## Fonctionnalités d'un VFS  

Les fonctionnalités de base de manipulation des fichiers sont implémentées par le filesystem lui-même. Le VFS fournit de structures génériques (inode, superblock, dentry, etc.) que chaque filesystem doit remplir et fournir au VFS pour que celui-ci puisse communiquer avec le filesystem.

Le VFS fournit cependant des fonctionnalités supplémentaires qui ne sont pas implémentable à l'échelle du filesystem :
- **API unifiée POSIX** : fournit une interface standard (`open`, `read`, `write`, etc.) indépendante du filesystem sous-jacent, garantissant la portabilité des applications  
- **Arborescence globale** : permet de réunir plusieurs filesystems hétérogènes dans une seule hiérarchie de répertoires cohérente  
- **Résolution de chemins** : prend en charge la traversée des chemins, les points de montage et les liens symboliques, même à travers plusieurs filesystems  
- **Cache générique** : met en cache dentries et inodes afin d’accélérer l’accès aux fichiers sans dépendre du format du filesystem  
- **FS distants/virtuels** : rend transparents l’accès aux systèmes de fichiers réseau ou générés dynamiquement comme s’ils étaient locaux  
- **Sécurité globale / namespaces** : applique des mécanismes de confinement et d’isolation (permissions, mount namespaces, chroot) à l’échelle de l’arborescence globale  

En résumé, le filesystem gère **ses données** alors que le VFS gère la **cohérence**, l’**uniformité** et l’**intégration** globale.  

Dans notre cas, le vfs va implémenter les fonctionnalités minimale à savoir une API unifiée côté utilisateur et la résolution des chemins. 

## Implémentation du VFS  

### API  

Dans un premier temps, notre VFS doit définir une API fournissant les fonctions de mainupulation des fichiers / dossiers. Celle-ci se trouve dans `include/vfs.h` et contient : 

Des constantes communes :
```c

#define MAX_OPEN_FILES  128

```

Des définitions de types :

```c

typedef enum {
    VFS_OK,
    VFS_INVALID_PATH,
    VFS_INVALID_FD,
    VFS_INVALID_PATH_DIR,
    VFS_INVALID_PATH_FILE,
    VFS_MAX_FD_REACHED,
    VFS_MEMORY_FULL,
    VFS_INVALID_NAME,
    VFS_NULL_POINTER,
    VFS_UNKWON_ERROR,

} vfs_error;

typedef int fd_t;
typedef char *path_t;

```

Des déclarations de fonctions :

```c

/* File managment */
vfs_error create_file(const path_t path, const char *file);
vfs_error delete_file(const path_t path);
vfs_error open_file(const path_t path, fd_t *fd);
vfs_error close_file(fd_t *fd);
long read_file(const fd_t *fd, char *str, unsigned len);
long write_file(const fd_t *fd, const char *str, unsigned len);
long append_file(fd_t *fd, const char *str, unsigned len);

/* Directory managment */
vfs_error create_dir(path_t path, char *dir_name);
vfs_error delete_dir(path_t path);
vfs_error open_dir(const path_t path, fd_t *fd);
vfs_error close_dir(fd_t *fd);
vfs_error get_next_dir_son(fd_t *fd, char *son_name);

```

### Suivi des inodes ouverts  

Le VFS utilise un tableau de descripteurs de fichier pour suivre l'ensemble des fichiers ouverts.

```c
file_descriptor_t open_inodes[MAX_OPEN_FILES];
```

Un descripteur de fichier (fd) est une structure privée qui contient une référence à la strucure de donnée inode utilisée par le filesystem, une tête de lecture de pour les fichiers et une pour les dossiers. Notre VFS ne gérant qu'un seul filesystem et ayant des fonctionnalités limitées, cette structure est simple mais elle pourait contenir des caches de lecture / écriture, des pointeurs vers les fonctions de manipulation du fichier implémentées par le filesystem gérant le ficher, des informations de sécurité et bien d'autres.

```c
typedef struct file_descriptor {
    inode* inode;       // inode of the file
    unsigned int next;  // next character to read 
    inode* next_son;    // next son of the current directory
} file_descriptor_t;
```

### Gestion de chemin  

La fonction `inode* parse_path(const path_t path)` est en charge de la lecture des chemins et renvoie le pointeur de l'inode vers lequel mène le chemin : 

```c
inode* parse_path(const path_t path) {
    int token_size;
    path_t local_path;
    inode* current_inode;
    // unsigned current_pos = 0;

    if(path[0] != '/') {
        WARN("invalid path\n");
        return 0;
    }

    current_inode = &(RAM_inodes_table[ROOT_INODE]);
    local_path = &(path[1]);
    while(1) {
        token_size = get_path_first_token(local_path);
        if(token_size < 0) {
            return 0;
        }

        if(token_size > 0) {
            current_inode = get_son_inode(current_inode, token);
        }

        if(0 == current_inode) {
            WARN("file not found : %s\n", token);
            return 0;
        }

        if(FILE == current_inode->type) {
            if('\0' != local_path[token_size]) {
                WARN("Path contain a file\n");
                return 0;
            } else {
                return current_inode;
            }
        } else if (EMPTY == current_inode->type) {
            WARN("Empty node\n");
            return 0;
        }

        if('\0' == local_path[token_size]) {
            return current_inode;
        } else {
            local_path = &(local_path[token_size+1]);
        }
    }
}
```

Cette implémentation ne permet d'utiliser que des chemins absoulus, un chemin doit donc commencer par un '/' :

```c
if(path[0] != '/') {
    WARN("invalid path\n");
    return 0;
}
```

Pour parcourir un chemin, celui-ci est découpé en token séparés par des '/'. Une boucle infinie itère sur ces tokens :  

```c
local_path = &(path[1]);
while(1) {
    token_size = get_path_first_token(local_path);
    ...
    local_path = &(local_path[token_size+1]);
}
```

A chaque étape, l'inode associé au token est récupéré et lorsque l'inode correspond à un fichier, on le renvoie.  

### Création / suppression  

Pour créer un fichier / dossier, l'inode du parent est récupéré via la lecture du chemin d'accès puis le filesystem est appelé pour créer un nouvel inode.  

Pour la suppression, le même processus est réalisé mais le vfs implémente en plus la suppression récursive des dossiers. Si un dossier non vide est supprimé, le vfs parcours récursivement le dossier pour supprimer tout son contenu puis supprime le dossier lui-même.  

### Lecture / écriture des fichiers  

Pour lire ou écrire dans un fichier, celui-ci doit d'abord être ouvert via `open_file` afin que l'inode soit récupéré et qu'un descripteur de fichier associé au fichier soit créé. 

Les fonctions de lecture et d'écriture ne prennent donc pas en paramètre le chemin vers le fichier mais un pointeur vers un entier correspondant à l'indice du descripteur dans la table des descripteurs de fichiers ouverts.  

Pour la lecture, `read_file` utilie la tête de lecture dans un fichier `next` qui correspond à l'offset du prochain caractère à lire. Chaque lecture de N caractères incrémente l'offset de N.  

```c
long read_file(const fd_t *fd, char *str, unsigned len) {
    ...

    return (long) read_inode(open_inodes[*fd].inode->id, open_inodes[*fd].next, len, str);
}
```

Pour l'écriture, deux options sont possible : 
- l'ajout à la fin du fichier qui utilise la fonction d'écriture du filesystem directement
- l'écriture au début du fichier qui supprime d'abord le contenu du fichier puis écrit dedans

```c
long write_file(const fd_t *fd, const char *str, unsigned len) {
    ...

    if (0 != reset_inode(open_inodes[*fd].inode->id)) {
        WARN("Failed to reset file\n");
        return -1;
    }

    return write_inode(open_inodes[*fd].inode->id, str, len);
}

long append_file(fd_t *fd, const char *str, unsigned len) {
    ...

    return write_inode(open_inodes[*fd].inode->id, str, len);
}
```

### Lecture des dossiers  

Pour lire la liste des inodes contenus dans un dossier, le vfs utilise le même mécanisme que pour les fichiers. Un dossier doit d'abord être ouvert pour initialiser un descripteur de fichier associé.

Ensuite, l'utilisateur peut récupérer le nom des sous-dossiers / fichier via une fonction d'itération. La tête de lecture des dossiers `next_son` contenue dans le descripteur de fichier est utilisée pour repérer le prochain inode à lire dans le dossier.  

```c
vfs_error get_next_dir_son(fd_t *fd, char *son_name) {
    ...

    if(0 != open_inodes[*fd].next_son) {
        strcpy(son_name, open_inodes[*fd].next_son->name);              // copy son name in output string
        open_inodes[*fd].next_son = open_inodes[*fd].next_son->brother; // move offset to next son
    } else {
        son_name[0] = '\0';
    }

    return VFS_OK;
}
```

## Gestion des appels système  

### Arbre d'appel du vfs  

Un processus utilisateur ne peut pas appeler le vfs directement, il doit passer par des appels systèmes. Voici la chaîne d'appel réalisée dans le cas de l'ouverture d'un fichier : 


1. Appel système de l'utilisateur (user.c) : `call_open_file("/src/file", &fd);`  
2. Génération interruption synchrone (user_sys.S) : `svc #0`  
3. Appel du handler pour interruption synchrone par l'irq controller (entry.S) : `el0_sync`  
4. Appel handler des interruptions de type svc (entry.S) : `b.eq el0_svc`  
5. Appel de la fonction système associée au numéro du syscall (entry.S) : `blr x16    // call sys_open_file routine`  
6. Appel du vfs (sys.c) : `open_file(path, fd);`  
7. Récupération du inode (vfs.c) : `parse_path(path)`  


`user_sys.h` fournit donc des fonctions côté utilisateur (EL0) pour réaliser les appels systèmes vers le VFS. L'irq controller est configuré pour gérer les interruptions synchrones et ces interruptions sont reliées jusqu'au vfs qui réalise ensuite des appels au filesystem si nécessaire.  

### Syscall côté utilisateur  

L'ensemble des syscall utilisables par un processus sont définis dans `user_sys.h/.S` : 

```c
vfs_error call_create_file(const path_t path, const char *file);
vfs_error call_delete_file(const path_t path);
vfs_error call_open_file(const path_t path, fd_t *fd);
vfs_error call_close_file(fd_t *fd);
long call_read_file(const fd_t *fd, char *str, unsigned len);
long call_write_file(const fd_t *fd, const char *str, unsigned len);
long call_append_file(fd_t *fd, const char *str, unsigned len);

vfs_error call_create_dir(path_t path, char *dir_name);
vfs_error call_delete_dir(path_t path);
vfs_error call_open_dir(const path_t path, fd_t *fd);
vfs_error call_close_dir(fd_t *fd);
vfs_error call_get_dir_son(fd_t *fd, char *son_name);
```

La macro assembleur suivante permet de factoriser le code de définition des syscall. `name` correspond au nom de la fonction syscall côté utilisateur et `num` à l'indice dans la table des appel systèmes `sys_call_table` définie dans `sys.c` :

```asm
.macro make_syscall name, num
    .global  \name
\name:
    mov     w8, #\num
    svc     #0
    ret
    .endm
```

La macro `make_syscall` est ensuite appelée pour chaque appel système disponnible :  

```asm
.set SYS_CREATE_FILE_NUMBER, 	3
.set SYS_DELETE_FILE_NUMBER, 	4
.set SYS_OPEN_FILE_NUMBER, 		5
.set SYS_CLOSE_FILE_NUMBER, 	6
.set SYS_READ_FILE_NUMBER, 		7
.set SYS_WRITE_FILE_NUMBER, 	8
.set SYS_APPEND_FILE_NUMBER, 	9

.set SYS_CREATE_DIR_NUMBER, 	10
.set SYS_DELETE_DIR_NUMBER, 	11
.set SYS_OPEN_DIR_NUMBER,		12
.set SYS_CLOSE_DIR_NUMBER,		13
.set SYS_GET_DIR_SON_NUMBER, 	14


make_syscall call_create_file, 	SYS_CREATE_FILE_NUMBER
make_syscall call_delete_file, 	SYS_DELETE_FILE_NUMBER
make_syscall call_open_file, 	SYS_OPEN_FILE_NUMBER
make_syscall call_close_file, 	SYS_CLOSE_FILE_NUMBER
make_syscall call_read_file, 	SYS_READ_FILE_NUMBER
make_syscall call_write_file, 	SYS_WRITE_FILE_NUMBER
make_syscall call_append_file, 	SYS_APPEND_FILE_NUMBER

make_syscall call_create_dir, 	SYS_CREATE_DIR_NUMBER
make_syscall call_delete_dir, 	SYS_DELETE_DIR_NUMBER
make_syscall call_open_dir, 	SYS_OPEN_DIR_NUMBER
make_syscall call_close_dir, 	SYS_CLOSE_DIR_NUMBER
make_syscall call_get_dir_son, 	SYS_GET_DIR_SON_NUMBER
```

### Syscall côté système  

Lorsqu'une interruption est générée, c'est l'irq controller qui la réceptionne. Dans notre cas, l'interruption est de type synchrone et est générée au niveau EL0. Dans la vector table de l'irq controller, on définit un handler à appeler dans le cas des interruptions synchrone en EL0 sur 64bits :  

```asm
.align	11
.global vectors
vectors:
	...

	ventry	el0_sync					// Synchronous 64-bit EL0
	ventry	el0_irq						// IRQ 64-bit EL0
	ventry	fiq_invalid_el0_64			// FIQ 64-bit EL0
	ventry	error_invalid_el0_64		// Error 64-bit EL0

	...
```

Ce handler appel ensuite un handler spécifique au interruption générée par l'instruction `svc` :  

```asm
el0_sync:
	kernel_entry 0
	mrs	x25, esr_el1				// read the syndrome register
	lsr	x24, x25, #ESR_ELx_EC_SHIFT		// exception class
	cmp	x24, #ESR_ELx_EC_SVC64			// SVC in 64-bit state
	b.eq	el0_svc
	...
```

Ce handler utilise alors le numéro d'interruption (argument 8) pour déterminer le syscall appelé, le recherche dans la table des 
syscall et l'appel :

```asm
sc_nr	.req	x25					// number of system calls
scno	.req	x26					// syscall number
stbl	.req	x27					// syscall table pointer

el0_svc:
	adr	stbl, sys_call_table	    // load syscall table pointer
	uxtw	scno, w8				// syscall number in w8
	mov	sc_nr, #__NR_syscalls
	bl	enable_irq
	cmp     scno, sc_nr             // check upper syscall limit
	b.hs	ni_sys

	ldr	x16, [stbl, scno, lsl #3]	// address in the syscall table
	blr	x16					        // call sys_* routine
	...
```

Les syscall côté système et la table associée sont définit dans `sys.h/.c` : 

```c
vfs_error sys_create_file(const path_t path, const char *file);
vfs_error sys_delete_file(const path_t path);
vfs_error sys_close_file(fd_t *fd);
long sys_read_file(const fd_t *fd, char *str, unsigned len);
long sys_write_file(const fd_t *fd, char *str, unsigned len);
long sys_append_file(fd_t *fd, char *str, unsigned len);
vfs_error sys_create_dir(path_t path, char *dir);
vfs_error sys_delete_dir(path_t dir);
vfs_error sys_open_dir(const path_t path, fd_t *fd);
vfs_error sys_close_dir(fd_t *fd);
vfs_error sys_get_dir_son(fd_t *fd, char *son_name);

void * const sys_call_table[__NR_syscalls] = {
	sys_write, 
	sys_fork, 
	sys_exit,
	
	sys_create_file,
	sys_delete_file,
	sys_open_file,
	sys_close_file,
	sys_read_file,
	sys_write_file,
	sys_append_file,

	sys_create_dir,
	sys_delete_dir,
	sys_open_dir,
	sys_close_dir,
	sys_get_dir_son
};
```
Enfin, les syscall appellent les fonctions du vfs définies dans la première partie.  

## Comparaison avec le vfs de linux  

Outre la complexité bien suppérieur du vfs de Linux et le nombre de fonctionnalités supplémentaires, notre vfs diffère grandement sur son approche.  

Comme expliqué au début, un vfs a pour objectif d'offrir une interface unifiée pour l'utilisateur afin d'accéder à plusieurs filesystem. Cependant, une question se pose : **comment définir l'interface entre vfs et filesystem ?**


Deux solutions sont alors possibles : 
1. Chaque filesystem fournit une interface qui lui est propre et chaque vfs doit alors implémenter une nouvelle version de ses fonctions pour chaque filesystem  
2. Le VFS fournit une interface côté filesystem et chaque filesystem doit respecter cette interface pour que le VFS puisse le prendre en charge  

Dans notre cas, nous n'avons à prendre en charge qu'un seul filesystem. Nous avons donc opté pour la première solution plus simple à implémenter. C'est pourquoi `vfs.c` inclut `filesystem.h`.  Chaque fonction est spécifique à ce filesystem et il faudrait redéfinir ces fonctions pour un nouveau filesystem.  

Cependant, cette solution est très lourde et peu modulaire. C'est pourquoi, dans le cadre de linux, la deuxième option a été choisie. Pour ce faire, le vfs définit un ensemble de structures (`superblock`, `inode`, `dentry`, `file`, etc.) et de fonctions (`register_filesystem`, `unregister_filesystem`, etc.).

Chacune des structures contient une sous-structure `*_operations` listant l'ensemble des fonctions utilisables sur cet objet. Cette sous-structure contient des pointeurs de fonctions qui doivent être définis par chaque filesystem pour pointer vers ses propres fonctions. 

Pour que le vfs supportent un nouveau filesystem, linux utilise le mécanisme de module. Lors du chargement du module dans l'OS, celui-ci s'enregistre auprès du vfs. Ensuite, lorsque le vfs a besoin de manipuler de la donnée gérée par ce filesystem, il utilise les fonctions fournies dans les structures `*_operations`. Ainsi, si un nouveau filesystem est ajouté, il n'y a pas besoin de modifier le vfs, c'est le filesystem qui s'adapte au vfs.  

Le problème semble juste avoir été déplacé du vfs au filesystem. Mais en réalité, plusieurs arguments sont favorables à cette solution :  
- très peu de vfs différents existent et leur architecture sont proches  
- les vfs propose une API très stable (rarement modifiée)  
- lors d'une modification de l'API, celle-ci est faite progressivement et la rétrocompatibilté est conservée au maximum (callback, helpers, wrappers, etc.)  




