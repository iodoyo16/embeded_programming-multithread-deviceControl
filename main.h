#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>

#include <termios.h>
#include <errno.h>

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/msg.h>	

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/input.h>



// switch macro
#define SW1 1<<8
#define SW2 1<<7
#define SW3 1<<6
#define SW4 1<<5
#define SW5 1<<4
#define SW6 1<<3
#define SW7 1<<2
#define SW8 1<<1
#define SW9 1
#define KEY_VOL 1<<9



// key macro
#define INPUT_BUF_SIZE 64
#define PRESS 1
#define VOL_DOWN 114
#define VOL_UP 115
#define BACK 158

// shared memory macro
#define SHARED_KEY1 (key_t) 0x10	
#define SHARED_KEY2 (key_t) 0x11

// sema macro
#define SEM_KEY (key_t) 0x20 
#define IFLAGS (IPC_CREAT)
#define ERR_OUTBUF ((struct sbuf_output *)-1)
#define ERR_INT ((int *)-1)

/* device */
#define SW_BUF_SIZE 9
#define FND_SIZE 5
#define DOT_MAT_SIZE 10
#define TEXTLCD_SIZE 33




/* struct for output buffer */
struct sbuf_output{
	unsigned char dot_matrix[DOT_MAT_SIZE];
	unsigned char text_lcd[TEXTLCD_SIZE];
	int led;
	unsigned char fnd[FND_SIZE];
	
};

// semactl argument structure
typedef union {
	unsigned short * array;
	int val;
	struct semid_ds *buf;
	
} semctl_union;