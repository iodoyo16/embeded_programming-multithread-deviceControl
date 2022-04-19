#include "main.h"
#include "clockMode.h"
#include "counterMode.h"
#include "devices.h"
#include "textEditorMode.h"
#include "drawBoardMode.h"

/* semaphore and shared memory */
struct sembuf p1 = {0, -1, SEM_UNDO }, p2 = {1, -1, SEM_UNDO}, p3 = {2, -1, SEM_UNDO};
struct sembuf v1 = {0, 1, SEM_UNDO }, v2 = {1, 1, SEM_UNDO }, v3 = {2,1,SEM_UNDO};
static int shared_mem_id1, shared_mem_id2, sem_id;

/* device on descriptor */
extern int fpga_key, fpga_sw; 
void init_states(
		struct clockState* clock_state,
		struct counterState* counter_state,
		struct textEditorState* text_editor_state,
		struct drawBoardState* draw_board_state
	){
	counter_state->number = 0;
	counter_state->base = 10;

	clock_state->clock_hour =0;
	clock_state->clock_min =0;
	clock_state->clock_mode = 0;
    clock_state->volatile_h =0;
	clock_state->volatile_m =0;

	text_editor_state->cnt = 0;
	text_editor_state->mode = ENG_MODE;
	memset(text_editor_state->text , 0, TEXTLCD_SIZE);
	text_editor_state->idx = 0;

	int i,j;
    for (i = 0; i < 10; i++)
        for(j =0; j<7 ;j++) (draw_board_state->board_map)[i][j] = 0x00;
    draw_board_state->pos_x = 0;
    draw_board_state->pos_y = 0;
    draw_board_state->cnt = 0;
    draw_board_state->cursor_on = CURSOR_ON;
};
void init_sbuf(int *i_sbuf, struct sbuf_output* o_sbuf){
	/* initialze shared memory buffer*/
	int i;
	*i_sbuf = 0;
	for(i=0; i<TEXTLCD_SIZE; i++) 
		o_sbuf->text_lcd[i] = 0;
}	

int init_sema(){
	semctl_union x;
	x.val = 0;
	int id = -1;

	/* create semaphore and get semaphore id*/
	id = semget (SEM_KEY, 3, IPC_CREAT);
	if (id== -1){
		perror("error semget");
		exit(1);	
	} 
	/* initialize semaphore */
	if (semctl (id, 0, SETVAL, x) == -1){	
		perror("ERROR: semctl 0\n");
		exit(1);
	}
	if (semctl (id, 1, SETVAL, x) == -1){			
		perror("ERROR: semctl 1\n");
		exit(1);	
	}
	if (semctl (id, 2, SETVAL, x) == -1){				
		perror("ERROR: semctl 2\n");
		exit(1);
	}

	return (id);
}

void rm_shm_sema(){
	/* remove shared memory */
	int success;
	success=shmctl(shared_mem_id1, IPC_RMID, 0);
	if (success == -1)
		exit(1);
	success=shmctl(shared_mem_id2, IPC_RMID, 0);
	if ( success== -1) 
		exit(1);
	success=semctl(sem_id, 0, IPC_RMID, 0);
	/* remove semaphore */
	if ( success== -1) 
		exit(1);
}

void get_shared_mem(int **i_shm, struct sbuf_output** o_shm){ // init
	/*shared mem allocate*/
	shared_mem_id1 = shmget(SHARED_KEY1, sizeof(int), 0600 | IFLAGS);
	shared_mem_id2 = shmget(SHARED_KEY2, sizeof(struct sbuf_output), 0600 | IFLAGS);
	if(shared_mem_id1 == -1||shared_mem_id2== -1){	
		perror("ERROR: shmget error\n");
		exit(1);
	}
	/* attach shared mem*/
	*i_shm = (int*)shmat(shared_mem_id1, 0, 0);
	*o_shm = (struct sbuf_output*)shmat(shared_mem_id2, 0, 0);
	if(*i_shm==ERR_INT||*o_shm==ERR_OUTBUF){
		perror("ERROR: shmat error\n");
		exit(1);	
	}
}


void input_process(int semid, int *i_sbuf){
	// fpga swtich var
	int i, size_sw;
	unsigned char input_sw_buf[SW_BUF_SIZE];
	int flag = 0; //flag for checking key or switch pressed

	// fpga key var
	struct input_event i_event[INPUT_BUF_SIZE];
    int rd, value, event_ds_size = sizeof(struct input_event);
	
	size_sw = sizeof(input_sw_buf);
	while(1){
		usleep(250000);
		// reading fpga key
		rd = read(fpga_key, i_event, event_ds_size * INPUT_BUF_SIZE);
		if(rd<event_ds_size){
			// reading switch part
			read(fpga_sw, &input_sw_buf, size_sw);
			int btn=0;
			for( i=0; i<SW_BUF_SIZE;i++){
				btn <<= 1;
				if(input_sw_buf[i]==1){
					btn += 1;
					flag= 1;
				} 
			}
			*i_sbuf = btn;
		}
		else{
			// key press handling
			value = i_event[0].value;
			if(value == PRESS){
				if(i_event[0].code==VOL_UP||i_event[0].code==VOL_DOWN||i_event[0].code==BACK){
						*i_sbuf = (i_event[0].code) * 10;
						flag = 1;
				}
			}
		}
		// handling input and wait until other process signals
		if(flag == 1){
			semop(semid, &p1, 1);
			flag = 0;
		}
	}
}
void output_process(int semid, struct sbuf_output* o_sbuf){
	while(1){
		// write_devname write sbuf data to device
		write_text_lcd(o_sbuf->text_lcd);
		write_dot_matrix(o_sbuf->dot_matrix);
		write_fnd(o_sbuf->fnd);
		write_led(o_sbuf->led);
		//signal to main process
		semop(semid, &v2, 1);
		usleep(10000);
	}
}

void main_process(int semid, int *i_sbuf, struct sbuf_output *o_sbuf){

	//initialize state
	int sw = SW1;	//variable to save input type
	int input_flag = 0;		
	*i_sbuf = 0;
	int mode = 1;

	struct clockState clock_state;
	struct counterState counter_state;
	struct textEditorState text_editor_state;
	struct drawBoardState draw_board_state;

	//init state
	init_states(&clock_state,&counter_state,&text_editor_state,&draw_board_state);

	while(1){
		input_flag = 1; // flag for checking input existance
		
		/* check input */
		if(*i_sbuf==VOL_DOWN*10){
			mode = (mode>1)? mode-1 : 4;
			sw = KEY_VOL;
		}
		else if(*i_sbuf==VOL_UP*10){
			mode = (mode<4)? mode+1 : 1;
			sw = KEY_VOL;
		}
		else if(*i_sbuf==BACK*10){
			return;
		}
		else if(*i_sbuf==0){
			input_flag=0;
		}
		else{
			sw = *i_sbuf;
		}
		/* Set output buffer by mode */
		if(mode==1){
			clock_mode_func(sw, input_flag, &(clock_state), o_sbuf);
		}
		else if(mode==2){
			counter_mode_func(sw, input_flag, &(counter_state), o_sbuf);
		}else if(mode==3){
			text_editor_mode_func(sw,input_flag, &(text_editor_state), o_sbuf);
		}else if(mode==4){
			draw_board_mode_func(sw,input_flag,&(draw_board_state), o_sbuf);
		}

		*i_sbuf = 0; //initializing input buffer

		// signal sema 2 
		semop(semid, &p2,1);
		// wait output process done if there is input
		if(input_flag == 1){
			semop(semid, &v1, 1);
		}
	}	
}


int main ()
{

	pid_t input_pid=0, output_pid=0;
	struct sbuf_output *o_sbuf;
	int *i_sbuf, *buf_mode;

	sem_id = init_sema(); //creat and init semaphore
	get_shared_mem (&i_sbuf, &o_sbuf);	//create and attach shared mem 
	init_sbuf(i_sbuf, o_sbuf);
	fpga_device_open();

	/* create child processes */
	input_pid=fork();
	if(input_pid>0){
		// parent(main) process
		output_pid=fork();

		if(output_pid>0){
			// parent(main) process
			main_process(sem_id, i_sbuf, o_sbuf);
			//end main process
			//kill input, output process
			kill(output_pid,SIGKILL);
			wait(NULL);
			kill(input_pid, SIGKILL); 	
			wait(NULL);
			rm_shm_sema();
			fpga_device_close();	//close device_file
		}
		else if(output_pid==0){		
			output_process(sem_id, o_sbuf); 
			rm_shm_sema();
		}
		else{
			perror("ERROR: fork() error");
		}
	}
	else if(input_pid==0){	
		input_process(sem_id, i_sbuf); 
		rm_shm_sema();
	}
	else{
		perror("ERROR: fork() error");
	}
	return 0;
}



