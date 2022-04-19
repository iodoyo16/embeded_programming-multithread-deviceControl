#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define FIX_MODE 0
#define MODIFY_MODE 1 


struct clockState {
	int clock_mode;	// current clock mode (FIX_MODE, MODIFY_MODE)
	int clock_hour; // diff of hour initial and modified
	int volatile_h;	// volatile modifying hour
	int volatile_m;	// volatile modifying minutes
    int clock_min;	// diff of minutes initial and modified
	
};

void clock_mode_func(int switch_code, int input_flag, struct clockState *clock_state,struct sbuf_output* o_sbuf);
void clock_handle_switch_code(int switch_code, struct clockState *clock_state, struct sbuf_output* o_sbuf,struct tm* time_ds);