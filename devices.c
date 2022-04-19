#include "main.h"
#include "devices.h"

extern int fpga_key, fpga_sw;

int fd_text_lcd;
int fd_dev_dotMat;
int fd_fnd;
int fd_led;


// write at fnd
void write_fnd(char *str_data)
{
	unsigned char data[4];
	data[0]=str_data[0]-0x30;
	data[1]=str_data[1]-0x30;
	data[2]=str_data[2]-0x30;
	data[3]=str_data[3]-0x30;
    write(fd_fnd,&data,4);	
	memset(data,0,sizeof(data));
}
// write at text lcd
void write_text_lcd(unsigned char *str_data)
{
	int str_len = BUF_SIZE;
	int i;
	unsigned char output_str[BUF_SIZE];
	for(i=0; i<BUF_SIZE; i++){
		if(str_data[i] == '\0' || str_data[i] == 0 ){
			str_len = i;
			break;
		}
		output_str[i] = str_data[i];
	}
	int empty_size=BUF_SIZE-str_len;
	if(str_len>=0){
		memset(output_str+str_len, ' ', empty_size);
	}
	write( fd_text_lcd,output_str,BUF_SIZE);		
	return;
}
// write at dotmatrix
void write_dot_matrix(unsigned char* data)
{
	write(fd_dev_dotMat,data,10);
	return;
}



// write at led (mmap)
void write_led(unsigned char data)
{
	int i;
	unsigned char *led_offset =0;
	unsigned long *base_addr = 0;
	//get fpga address 
	base_addr = (unsigned long *)mmap(NULL, 4096, PROT_WRITE|PROT_READ, MAP_SHARED, fd_led, FPGA_BASE_ADDRESS);
	if (base_addr == MAP_FAILED)
	{
		perror("ERROR :mmap error\n");
		close(fd_led);
		exit(1);
	}
	//calc led address
	led_offset=(unsigned char*)((void*)base_addr+LED_ADDR);
	//put data to led address to turn on the led
	*led_offset=data;  
	munmap(led_offset, 4096);
	return;
}
void fpga_device_open(){
	// open fpga_device
	// proc,back,vol
	//text lcd
	fd_text_lcd = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
	if ( fd_text_lcd<0) {
		perror("ERROR: text lcd open error \n");
		exit(1);
	}
	//fnd
	fd_fnd = open(FND_DEVICE, O_RDWR);
	if(fd_fnd<0){
		perror("ERROR: write_fnd open error \n");
		exit(1);
	}
	//matrix
	fd_dev_dotMat = open(FPGA_DOT_DEVICE, O_WRONLY);
	if (fd_dev_dotMat<0) {
		perror("ERROR: dotmatrix open error \n");
		exit(1);
	}
	// sw1~9
	fpga_sw = open(FPGA_SW, O_RDWR );
	if (fpga_sw<0) {
		perror("ERROR:there is no /dev/fpga_push_switch \n");
		exit(1);
	}
	// led
	fd_led = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd_led < 0) {
		perror("ERROR:/dev/mem open error");
		exit(1);
	}
	
	
	fpga_key = open(FPGA_KEY, O_RDONLY | O_NONBLOCK);
    if (fpga_key <0) {
        perror("ERROR:there is no /dev/input/event0 \n");
		exit(1);
    }
}

void fpga_device_close(){

	unsigned char clr_fnd[FND_SIZE];
	unsigned char clr_text_lcd[TEXTLCD_SIZE];
	unsigned char clr_dot_matrix[DOT_MAT_SIZE];
	//clear device
	int i;
	for(i=0; i<DOT_MAT_SIZE; i++)	{
		clr_dot_matrix[i] = 0x00;
	}
	write_dot_matrix(clr_dot_matrix);

	sprintf(clr_fnd, "0000");
	sprintf(clr_text_lcd, "");
	write_fnd(clr_fnd);
	write_text_lcd(clr_text_lcd);
	write_led(0);
	
	// device close
	close(fd_led);
	close(fd_text_lcd);
	close(fpga_key);
	close(fd_fnd);
	close(fd_dev_dotMat);
	close(fpga_sw);
}