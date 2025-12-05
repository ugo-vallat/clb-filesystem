#include "user_sys.h"
#include "user.h"
#include "printf.h"

void loop(char* str)
{
	char buf[2] = {""};
	while (1){
		for (int i = 0; i < 5; i++){
			buf[0] = str[i];
			call_sys_write(buf);
			user_delay(1000000);
		}
	}
}

void user_process()
{
	call_sys_write("User process, starting the call to the VFS :\n\r");

	call_create_file();
	call_delete_file();
	call_open_file();
	call_close_file();
	call_read_file();
	call_write_file();
	call_append_file();
	
	call_create_dir();
	call_delete_dir();
	call_read_dir();
}

