.section ".text.user"

// Macro pour les appels de syscall simple.
.macro make_syscall name, num
    .globl  \name
\name:
    mov     w8, #\num
    svc     #0
    ret
    .endm

// syscal numbers
.set SYS_WRITE_NUMBER, 			0
.set SYS_FORK_NUMBER, 			1
.set SYS_EXIT_NUMBER, 			2

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


.globl user_delay
user_delay:
	subs x0, x0, #1
	bne user_delay
	ret

.globl call_sys_write
call_sys_write:
	mov w8, #SYS_WRITE_NUMBER
	svc #0
	ret

.globl call_sys_exit
call_sys_exit:
	mov w8, #SYS_EXIT_NUMBER
	svc #0
	ret

.globl call_sys_fork
call_sys_fork:
	mov w8, #SYS_FORK_NUMBER
	svc #0
	ret
