#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

//#define BSIZE 512
//#define NDIRECT 8

#define FS_NPAGES 128
#define FS_SIZE (FS_NPAGES * BSIZE)


#define MAX_SIZE_NAME 16

typedef enum TYPE_FILE {
    FILE,
    FOLDER,
    EMPTY
} TYPE_FILE;

typedef enum PERMISSION {
  WRITABLE = 1,
  READABLE = 2
} PERMISSION;

typedef struct superblock {
    unsigned size;          //number file system blocks
    unsigned nbr_block;     //number of data blocks
    unsigned nbr_inodes;    //number of inodes
    unsigned first_inode;   //first block for inodes
    unsigned bitmap;     //first available block for bitmap
} superblock_t;


// in-memory copy of an inode
typedef struct RAM_inode {
  int id;
  int ref;            // Reference count
  TYPE_FILE type;     // type of element inode
  unsigned size;     
  struct RAM_inode* father;
  struct RAM_inode* brother;
  struct RAM_inode* first_son;
  char name[MAX_SIZE_NAME];
  unsigned data_block;    //pointer of the Memory block
}inode;

inode RAM_inodes_table[FS_NPAGES];
//char bitmap;

void init_fs();
int alloc_inode(char name[MAX_SIZE_NAME], inode* father, TYPE_FILE type);
void free_inode(int i);
int write_inode(int inode, const char* data);
int read(int inode, int pos, int size);
int reset(int inode);


#endif