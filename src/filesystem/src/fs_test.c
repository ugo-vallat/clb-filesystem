#include "vfs.h"
#include "printf.h"
#include "user_sys.h"

/*
void fs_test(void)
{
    vfs_error err;
    fd_t fd;          // file descriptor VFS
    char buffer[64];
    long n;

    // 0) Initialiser le FS (si ce n'est pas déjà fait avant)
    init_fs();

    printf("=== FS TEST START ===\n");

    // 1) Créer un répertoire /docs
    err = create_dir("/", (char*)"docs");
    if (err != VFS_OK) {
        printf("create_dir(\"/\", \"docs\") FAILED: %d\n", err);
        return;
    }

    // 2) Créer un fichier /docs/test.txt
    err = create_file("/docs", "test.txt");
    if (err != VFS_OK) {
        printf("create_file(\"/docs\", \"test.txt\") FAILED: %d\n", err);
        return;
    }

    // 3) Ouvrir /docs/test.txt
    err = open_file("/docs/test.txt", &fd);
    if (err != VFS_OK) {
        printf("open_file(\"/docs/test.txt\") FAILED: %d\n", err);
        return;
    }

    // 4) Écrire dans le fichier
    const char *msg = "Hello FS en RAM !\n";

    n = write_file(&fd, msg, 20); 
    if (n < 0) {
        printf("write_file FAILED: %ld\n", n);
        close_file(&fd);
        return;
    }
    printf("Wrote %ld bytes into /docs/test.txt\n", n);

    // 5) Fermer le fichier
    err = close_file(&fd);
    if (err != VFS_OK) {
        printf("close_file FAILED: %d\n", err);
        return;
    }

    // 6) Rouvrir pour lire
    err = open_file("/docs/test.txt", &fd);
    if (err != VFS_OK) {
        printf("re-open_file(\"/docs/test.txt\") FAILED: %d\n", err);
        return;
    }

    // 7) Lire le contenu
    n = read_file(&fd, buffer, sizeof(buffer) - 1);
    if (n < 0) {
        printf("read_file FAILED: %ld\n", n);
        close_file(&fd);
        return;
    }
    buffer[n] = '\0';

    printf("Read back (%ld bytes): \"%s\"\n", n, buffer);

    // 8) Fermer
    err = call_close_file(&fd);
    if (err != VFS_OK) {
        printf("close_file (2) FAILED: %d\n", err);
        return;
    }

    printf("=== FS TEST DONE ===\n");
}
*/
