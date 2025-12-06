#ifndef	_USER_SYS_H
#define	_USER_SYS_H


void call_sys_write(char * buf);
int call_sys_fork();
void call_sys_exit();

void call_create_file();
void call_delete_file();
void call_open_file();
void call_close_file();
void call_read_file();
void call_write_file();
void call_append_file();

void call_create_dir();
void call_delete_dir();
void call_read_dir();


extern void user_delay ( unsigned long);
extern unsigned long get_sp ( void );
extern unsigned long get_pc ( void );

#endif  /*_USER_SYS_H */
