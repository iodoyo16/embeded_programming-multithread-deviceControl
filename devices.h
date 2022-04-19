#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>

// fpga key , prog, vol, back
#define FPGA_KEY "/dev/input/event0"

//fpga dot matrix
#define FPGA_DOT_DEVICE "/dev/fpga_dot"
// fpga led (mmap)
#define FPGA_BASE_ADDRESS 0x08000000 //fpga_base address
#define LED_ADDR 0x16  //led address




//text lcd
#define BUF_SIZE 32
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"

// fpga fnd
#define FND_DEVICE "/dev/fpga_fnd"


// switch push
#define FPGA_SW "/dev/fpga_push_switch"



//file descriptor on device
int fpga_key;
int fpga_sw;


// device control function 
void write_led(unsigned char data);
void write_fnd(char *str_data);
void write_dot_matrix(unsigned char* data);
void write_text_lcd(unsigned char *data);

void fpga_device_open();
void fpga_device_close();




