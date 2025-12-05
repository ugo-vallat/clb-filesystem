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

#ifdef __FILESYSTEM_C__
inode RAM_inodes_table[FS_NPAGES];
#else // __FILESYSTEM_C__
extern inode RAM_inodes_table[FS_NPAGES];
#endif // __FILESYSTEM_C__
//char bitmap;


void init_fs();

/**
 * @brief Create a Inode to create a file or folder.
 * return a inode id, if there is an error return -1
 * 
 * @param name 
 * @param father 
 * @param type 
 * @return int 
 */
int alloc_inode(char name[MAX_SIZE_NAME], inode* father, TYPE_FILE type);

/**
 * @brief Destroy a file or folder, return 1 on success return 0 on failure
 * 
 * @param i 
 */
int free_inode(int i);

/**
 * @brief Write on the datablock of the inode.
 * return the number of bytes written in the file
 * 
 * @param inode 
 * @param data 
 * @param size
 * @return int 
 */
int write_inode(int inode, const char* data, int size);

/**
 * @brief Read a datablock of the inode.
 * Return the number of bytes read in the file
 * 
 * @param inode 
 * @param pos 
 * @param size 
 * @param buff_dest 
 * @return int 
 */
int read_inode(int inode, int pos, int size, char* buff_dest);

/**
 * @brief Clear all data bytes in the file
 * 
 * @param inode 
 * @return int 
 */
int reset_inode(int inode);


#endif
