#include <vfs.h>
#include <filesystem.h>
#include <printf.h>
#include <string.h>

#define WARN(...) printf("[VFS] "__VA_ARGS__)

inode* parse_path(const path_t path);
int get_path_first_token(const path_t path);
inode* get_son_inode(inode* father, char * son_name);
void delete_inode(inode *i);
fd_t get_next_fd(void);

typedef struct file_descriptor {
    inode* inode;       // inode of the file
    unsigned int next;  // next character to read 
} file_descriptor_t;

static char token[MAX_SIZE_NAME+1];
file_descriptor_t open_files[MAX_OPEN_FILES];

int get_path_first_token(const path_t path) {
    
    int i;
    for(i = 0; i <= MAX_SIZE_NAME; i++) {
        switch (path[i]) {
            case '\0':
            case '/':
                token[i] = '\0';
                return i;
            default:
                token[i] = path[i];
                break;
        }
    }

    WARN("name too long\n");
    token[0] = '\0';
    return -1;
}

inode* get_son_inode(inode* father, char* son_name) {
    inode* current_son;
    current_son = father->first_son;
    while(0 != current_son) {
        if (strcmp(son_name, current_son->name) == 0) {
            return current_son;
        } else {
            current_son = current_son->brother;
        }
    }
    WARN("file does not exist\n");
    return 0;
}

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

fd_t get_next_fd(void) {
    for(fd_t fd = 0; fd < MAX_OPEN_FILES; fd++) {
        if(0 == open_files[fd].inode) {
            return fd;
        }
    }
    WARN("No fd available\n");
    return -1;
}

vfs_error create_file(const path_t path, const char *file) {
    inode* dir;
    inode_id new_inode;

    if(0 == path || 0 == file) {
        WARN("Null pointer\n");
        return VFS_NULL_POINTER;
    }

    if('\0' == file[0] || strlen(file) > MAX_SIZE_NAME) {
        WARN("Invalid file name\n");
        return VFS_INVALID_NAME;
    }
    
    dir = parse_path(path);
    if(0 == dir) {
        return VFS_INVALID_FD;
    }
    if(FOLDER != dir->type) {
        WARN("Parent folder is not a folder\n");
        return VFS_INVALID_FD;
    }

    new_inode = alloc_inode(file, dir, FILE);
    if(new_inode < 0) {
        return VFS_MEMORY_FULL;
    }

    return VFS_OK;
}

void delete_inode(inode *i) {
    if(EMPTY == i->type) {
        WARN("Delete empty inode\n");
        return;
    }
    if(FILE == i->type) {
        if(free_inode(i->id) != 0) {
            WARN("Failed to free inode %d\n", i->id);
        }
        return;
    } else {
        inode *to_delete;
        /* delete children recursively */
        while (0 != i->first_son) {
            to_delete = i->first_son;
            i->first_son = to_delete->brother;
            delete_inode(to_delete);
        }
        /* delete folder */
        if(free_inode(i->id) != 0) {
            WARN("Failed to free inode %d\n", i->id);
        }
        return;
    }
}

vfs_error delete_file(const path_t path) {
    inode* to_delete;
    
    if(0 == path) {
        WARN("Null pointer\n");
        return VFS_NULL_POINTER;
    }

    to_delete = parse_path(path);
    if(0 == to_delete) {
        return VFS_INVALID_FD;
    }

    delete_inode(to_delete);
    return VFS_OK;
}

vfs_error open_file(const path_t path, fd_t *fd) {
    inode* file;

    if(0 == fd) {
        WARN("Null pointer\n");
        return VFS_NULL_POINTER;
    }

    *fd = get_next_fd();
    if(*fd < 0) {
        *fd = -1;
        return VFS_MAX_FD_REACHED;
    }
    
    file = parse_path(path);
    if(0 == file) {
        return VFS_INVALID_FD;
    }
    if(FILE != file->type) {
        WARN("Not a path to a file\n");
        return VFS_INVALID_FD;
    }
    open_files[*fd].inode = file;
    open_files[*fd].next = 0;
    file->ref++;
    return VFS_OK;
}


vfs_error close_file(fd_t *fd) {
    if(0 == fd) {
        WARN("Null pointer\n");
        return VFS_NULL_POINTER;
    }

    if(0 > *fd) {
        WARN("Invalid fd\n");
        return VFS_INVALID_FD;
    }

    if(0 == open_files[*fd].inode) {
        WARN("File not open\n");
        *fd = -1;
        return VFS_INVALID_FD;
    }
    open_files[*fd].inode->ref--;
    open_files[*fd].inode = 0;
    open_files[*fd].next = 0;
    *fd = -1;
    return VFS_OK;
}

long read_file(const fd_t *fd, char *str, unsigned len) {
    if(0 == fd || 0 == str) {
        WARN("Null pointer\n");
        return VFS_NULL_POINTER;
    }

    if(0 > *fd) {
        WARN("Invalid fd\n");
        return VFS_INVALID_FD;
    }

    if(0 == open_files[*fd].inode) {
        WARN("File not open\n");
        return VFS_INVALID_FD;
    }

    if(FILE != open_files[*fd].inode->type) {
        WARN("File not readable\n");
        return VFS_INVALID_FD;
    }

    if(open_files[*fd].next >= open_files[*fd].inode->size || 0 == len) {
        str[0] = '\0';
        return 0;
    }

    return (long) read_inode(open_files[*fd].inode->id, open_files[*fd].next, len, str);
}

long write_file(const fd_t *fd, const char *str, unsigned len) {
    WARN("write_file not implemented yet\n");
    return -1;
}

long append_file(fd_t *fd, const char *str, unsigned len) {
    WARN("append_file not implemented yet\n");
    return -1;
}


/*
    -------------------
    Directory managment
    -------------------
*/

vfs_error create_dir(path_t path, char *dir) {
    WARN("create_dir not implemented yet\n");
    return VFS_UNKWON_ERROR;
}

vfs_error delete_dir(path_t dir) {
    WARN("delete_dir not implemented yet\n");
    return VFS_UNKWON_ERROR;
}

vfs_error read_dir(path_t dir, char **files[FILE_NAME_SIZE], unsigned *nb_entries) {
    WARN("read_dir not implemented yet\n");
    return VFS_UNKWON_ERROR;
}

