#ifndef _VFS_H_
#define _VFS_H_

/* 
    -----------------------------
    Virtual File System interface
    -----------------------------
*/

#define BLOCK_SIZE      (512)
#define FILE_NAME_SIZE  (24)
#define DIR_NAME_SIZE   (FILE_NAME_SIZE)
#define INODE_ID_SIZE   (8)
#define FILE_REF_SIZE   (INODE_ID_SIZE + FILE_NAME_SIZE)
#define NB_FILES_IN_DIR (BLOCK_SIZE/FILE_REF_SIZE)

typedef enum {
    VFS_OK,
    VFS_INVALID_PATH,
    VFS_INVALID_FD,
    VFS_INVALID_PATH_DIR,
    VFS_INVALID_PATH_FILE,
    VFS_MAX_FD_REACHED,
    VFS_MEMORY_FULL,
    VFS_UNKWON_ERROR,

} vfs_error;

typedef struct filedescriptor_s fd_t;
typedef char *path_t;

/*
    --------------
    File managment
    --------------
*/

/**
 * @brief Create a file object
 * 
 * @param[in] path Absolute path to the destination directory
 * @param[in] file Name of the new file
 * @return vfs_error 
 */
vfs_error create_file(const path_t path, const char *file);

/**
 * @brief Delete a file object
 * 
 * @param[in] path Absolute path to the file
 * @return vfs_error 
 */
vfs_error delete_file(const path_t path);

/**
 * @brief Open a file object
 * 
 * @param[in] path Absolute path to the file
 * @param[out] fd Returned file descriptor
 * @return vfs_error 
 */
vfs_error open_file(const path_t path, fd_t *fd);

/**
 * @brief Close a file object
 * 
 * @param[in] fd File descriptor
 * @return vfs_error 
 */
vfs_error close_file(const fd_t *fd);

/**
 * @brief Read a file object
 * 
 * @param[in] fd File descriptor
 * @param[out] str Read string of maximum *len* + 1 characters
 * @param[in] len Number of characters to read 
 *
 * @return number char readen or -1 if error 
 */
long read_file(const fd_t *fd, char *str, unsigned len);

/**
 * @brief write a file object (overwrite)
 * 
 * @param[in] fd File descriptor
 * @param[in] str String to write
 * @param[in] len Size of the string
 * @return number char written or -1 if error 
 */
long write_file(const fd_t *fd, const char *str, unsigned len);

/**
 * @brief write at the end of a file object (not overwrite)
 * 
 * @param[in] fd File descriptor
 * @param[in] str String to write
 * @param[in] len Size of the string
 * @return number char written or -1 if error  
 */
long append_file(fd_t *fd, const char *str, unsigned len);


/*
    -------------------
    Directory managment
    -------------------
*/

/**
 * @brief Create a dir object
 * 
 * @param[in] path Path to parent directory
 * @param[in] dir Name of new directory
 * @return vfs_error 
 */
vfs_error create_dir(path_t path, char *dir);

/**
 * @brief delete a dir object
 * 
 * @param[in] dir Path to the directory to delete
 * @return vfs_error 
 */
vfs_error delete_dir(path_t dir);

/**
 * @brief read a dire object, to list current files in the dir
 * 
 * @param[in] dir Path to the directory to read
 * @param[out] files List of files/directories name's in the directory ()
 * @param[out] nb_entries Number of entries in the file
 * @return vfs_error 
 */
vfs_error read_dir(path_t dir, char **files[FILE_NAME_SIZE], unsigned *nb_entries);







#endif // _VFS_H_
