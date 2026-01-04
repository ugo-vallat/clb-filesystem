#include "mm.h"
#include "user_sys.h"
#include "user.h"
#include "printf.h"
#include "vfs.h"


#define ROOT            "/"
#define TVFS_FILE1_NAME "file1.txt"
#define TVFS_FILE2_NAME "file2.txt"
#define TVFS_FILE3_NAME "file3.txt"
#define TVFS_SRC_NAME   "src"
#define TVFS_FILE1      ROOT TVFS_FILE1_NAME
#define TVFS_SRC        ROOT TVFS_SRC_NAME "/"
#define TVFS_FILE2      TVFS_SRC TVFS_FILE2_NAME
#define TVFS_FILE3      TVFS_SRC TVFS_FILE3_NAME

#define TVFS_STRING         "abc123"
#define TVFS_STRING_SIZE    (sizeof(TVFS_STRING)-1)

char *get_vfs_error_name(vfs_error err) {
    switch (err) {
    case VFS_OK:
        return "VFS_OK";
        break;
    case VFS_INVALID_PATH:
        return "VFS_INVALID_PATH";
        break;
    case VFS_INVALID_FD:
        return "VFS_INVALID_FD";
        break;
    case VFS_INVALID_PATH_DIR:
        return "VFS_INVALID_PATH_DIR";
        break;
    case VFS_INVALID_PATH_FILE:
        return "VFS_INVALID_PATH_FILE";
        break;
    case VFS_MAX_FD_REACHED:
        return "VFS_MAX_FD_REACHED";
        break;
    case VFS_MEMORY_FULL:
        return "VFS_MEMORY_FULL";
        break;
    case VFS_INVALID_NAME:
        return "VFS_INVALID_NAME";
        break;
    case VFS_NULL_POINTER:
        return "VFS_NULL_POINTER";
        break;
    case VFS_UNKWON_ERROR:
        return "VFS_UNKWON_ERROR";
        break;
    default:
        return "UNDEFINED VFS ERROR";
    }
}

void test_create_file(char* path, char* file) {
    vfs_error ret;
    call_sys_write("Test create file <");
    call_sys_write(path);
    call_sys_write(">:<");
    call_sys_write(file);
    call_sys_write("> : ");
    ret = call_create_file(path, file);
    if(VFS_OK == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO (");
        call_sys_write(get_vfs_error_name(ret));
        call_sys_write(")\n");
    }
}

void test_delete_file(char* path) {
    vfs_error ret;
    call_sys_write("Test delete file <");
    call_sys_write(path);
    call_sys_write("> : ");
    ret = call_delete_file(path);
    if(VFS_OK == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO (");
        call_sys_write(get_vfs_error_name(ret));
        call_sys_write(")\n");
    }
}

fd_t test_open_file(char* file) {
    vfs_error ret;
    fd_t fd;
    call_sys_write("Test open file <");
    call_sys_write(file);
    call_sys_write("> : ");
    ret = call_open_file(file, &fd);
    if(VFS_OK == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO (");
        call_sys_write(get_vfs_error_name(ret));
        call_sys_write(")\n");
    }

    return fd;
}

void test_write_file(fd_t *fd, char *string, unsigned long len) {
    vfs_error ret;
    call_sys_write("Test write <");
    call_sys_write(string);
    call_sys_write("> in file : ");
    ret = call_write_file(fd, string, len);
    if(len == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO\n");
    }
}

void test_append_file(fd_t *fd, char *string, unsigned long len) {
    vfs_error ret;
    call_sys_write("Test write <");
    call_sys_write(string);
    call_sys_write("> in file : ");
    ret = call_append_file(fd, string, len);
    if(len == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO\n");
    }
}

void test_read_file(fd_t *fd, char *buff, unsigned long len) {
    vfs_error ret;
    call_sys_write("Test read file : ");
    ret = call_read_file(fd, buff, len);
    if(len == ret) {
        buff[len] = '\0';
        call_sys_write("OK (");
        call_sys_write(buff);
        call_sys_write(")\n");
    } else {
        call_sys_write("KO\n");
    }
}

void test_close_file(fd_t *fd) {
    vfs_error ret;
    call_sys_write("Test close file : ");
    ret = call_close_file(fd);
    if(VFS_OK == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO (");
        call_sys_write(get_vfs_error_name(ret));
        call_sys_write(")\n");
    }
}

void test_create_dir(char* path, char* dir) {
    vfs_error ret;
    call_sys_write("Test create dir <");
    call_sys_write(path);
    call_sys_write(">:<");
    call_sys_write(dir);
    call_sys_write("> : ");
    ret = call_create_dir(path, dir);
    if(VFS_OK == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO (");
        call_sys_write(get_vfs_error_name(ret));
        call_sys_write(")\n");
    }
}

void test_delete_dir(char* path) {
    vfs_error ret;
    call_sys_write("Test delete dir <");
    call_sys_write(path);
    call_sys_write("> : ");
    ret = call_delete_dir(path);
    if(VFS_OK == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO (");
        call_sys_write(get_vfs_error_name(ret));
        call_sys_write(")\n");
    }
}

fd_t test_open_dir(char* dir) {
    vfs_error ret;
    fd_t fd;
    call_sys_write("Test open dir <");
    call_sys_write(dir);
    call_sys_write("> : ");
    ret = call_open_dir(dir, &fd);
    if(VFS_OK == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO (");
        call_sys_write(get_vfs_error_name(ret));
        call_sys_write(")\n");
    }
    return fd;
}

void test_get_next_dir_son(fd_t *fd, char* son) {
    vfs_error ret;
    call_sys_write("Test get next son : ");
    ret = call_get_dir_son(fd, son);
    if(VFS_OK == ret) {
        call_sys_write("OK (");
        call_sys_write(son);
        call_sys_write(")\n");
    } else {
        call_sys_write("KO (");
        call_sys_write(get_vfs_error_name(ret));
        call_sys_write(")\n");
    }
}

void test_close_dir(fd_t *fd) {
    vfs_error ret;
    call_sys_write("Test close dir : ");
    ret = call_close_dir(fd);
    if(VFS_OK == ret) {
        call_sys_write("OK\n");
    } else {
        call_sys_write("KO (");
        call_sys_write(get_vfs_error_name(ret));
        call_sys_write(")\n");
    }
}


void user_process()
{
    fd_t fd; 
    char buff[TVFS_STRING_SIZE*4+1];

	
    call_sys_write("User process, starting the call to the VFS :\n\r");



    /*_________________FILE_________________*/

    test_create_file(ROOT, TVFS_FILE1_NAME);
    fd = test_open_file(TVFS_FILE1);
    test_write_file(&fd, TVFS_STRING, TVFS_STRING_SIZE);
    test_read_file(&fd, buff, TVFS_STRING_SIZE);
    test_close_file(&fd);

    fd = test_open_file(TVFS_FILE1);
    test_append_file(&fd, TVFS_STRING, TVFS_STRING_SIZE);
    test_read_file(&fd, buff, TVFS_STRING_SIZE*2);
    test_close_file(&fd);

    /*_________________DIR_________________*/


    test_create_dir(ROOT, TVFS_SRC_NAME);
    test_create_file(TVFS_SRC, TVFS_FILE2_NAME);
    test_create_file(TVFS_SRC, TVFS_FILE3_NAME);
    fd = test_open_dir(TVFS_SRC);
    test_get_next_dir_son(&fd, buff);
    test_get_next_dir_son(&fd, buff);
    test_get_next_dir_son(&fd, buff);
    test_close_dir(&fd);

    /*_________________DELETE_________________*/

    test_delete_file(TVFS_FILE2);
    fd = test_open_dir(TVFS_SRC);
    test_get_next_dir_son(&fd, buff);
    test_get_next_dir_son(&fd, buff);
    test_close_dir(&fd);
    
    test_delete_file(TVFS_SRC);
    fd = test_open_dir(ROOT);
    test_get_next_dir_son(&fd, buff);
    test_get_next_dir_son(&fd, buff);
    test_close_dir(&fd);
    call_sys_write("\r\n THE END");

    while(1) {
        call_sys_write(".");
        for(int i = 0; i < 5000000; i++) {
        }
    } 

}

