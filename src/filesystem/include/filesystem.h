#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

//#define BSIZE 512
//#define NDIRECT 8

#define FS_NPAGES 128
#define FS_SIZE (FS_NPAGES * BSIZE)
#define FILE_MAX_SIZE PAGE_SIZE

#define MAX_SIZE_NAME   16
#define ROOT_INODE      0
#define INVALID_INODE   (-1)

typedef int inode_id;

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
  inode_id id;
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

/**
 * @brief Initialization of RAM inodes array with the first element root
 * 
 */
void init_fs();

/**
 * @brief Create a Inode to create a file or folder.
 * return a inode id, if there is an error return -1
 * 
 * @param name 
 * @param father 
 * @param type 
 * @return inode_id
 */
inode_id alloc_inode(const char name[MAX_SIZE_NAME], inode* father, TYPE_FILE type);

/**
 * @brief Destroy a file or folder, return 1 on success return 0 on failure
 * failed if the folder is not empty 
 *
 * @param inode
 * @return int
 */
int free_inode(inode_id inode);


/**
 * @brief Write on the datablock of the inode. Append the element at the end of the datablock.
 * return the number of bytes written in the file
 * 
 * @param inode 
 * @param data 
 * @param size
 * @return int 
 */
int write_inode(inode_id inode, const char* data, int size);

/**
 * @brief Read a datablock of the inode.
 * Return the number of bytes read in the file
 * 
 * if the function returns -1, the inode is out of range of the inodes array
 * if the function returns -2, the inode is not a file to read 
 * 
 * The value reads is save in the buff_dest
 * 
 * @param inode 
 * @param pos 
 * @param size 
 * @param buff_dest 
 * @return int 
 */
int read_inode(inode_id inode, int pos, int size, char* buff_dest);

/**
 * @brief Clear all data bytes of the datablock.
 * Return 0, if there is a error return -1
 * 
 * @param inode 
 * @return int 
 */
int reset_inode(inode_id inode);


#endif
