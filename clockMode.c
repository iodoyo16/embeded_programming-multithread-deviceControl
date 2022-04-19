//////////////////////////////////////////////////////////////
//    Clock Mode
//    Set to output buffer with time and modify the time 

//    * param
//    switch_code : input switch code
//    input_flag : check flag for input existence
//    clock_state : save clock state
//    o_sbuf: output shared buffer
#include "main.h"
#include "clockMode.h"

void clock_handle_switch_code(int switch_code, struct clockState *clock_state, struct sbuf_output* o_sbuf,struct tm* time_ds){
    int hour_state=(time_ds->tm_hour+clock_state->clock_hour)+(time_ds->tm_min+clock_state->clock_min)/60;
    int min_state=time_ds->tm_min+clock_state->clock_min;

    switch(switch_code){
        case KEY_VOL:   
            /*Initailize devices' output and clock mode*/
            memset(o_sbuf->text_lcd, 0, TEXTLCD_SIZE);
            clock_state->clock_mode = FIX_MODE;
            memset(o_sbuf->dot_matrix,0x00,DOT_MAT_SIZE);
            break;
        case SW1:  
            /*changes clock mode*/
            if(clock_state->clock_mode==FIX_MODE){
                clock_state->clock_mode=MODIFY_MODE;
                clock_state->volatile_h = (hour_state)%24;
                clock_state->volatile_m = (min_state)%60 ;
            }else if(clock_state->clock_mode==MODIFY_MODE){
                clock_state->clock_mode=FIX_MODE;
                clock_state->clock_hour = clock_state->volatile_h - time_ds->tm_hour;
                clock_state->clock_min = clock_state->volatile_m - time_ds->tm_min;
            }
            break;
        case SW2: 
            /* reset to board time */
            if(clock_state->clock_mode!=MODIFY_MODE)
                break;
            clock_state->volatile_h = time_ds->tm_hour;
            clock_state->volatile_m = time_ds->tm_min ;
            break;
        case SW3: 
            // increase hour
            if(clock_state->clock_mode!=MODIFY_MODE){
                break;
            }
            clock_state->volatile_h = (clock_state->volatile_h + 1)%24;
            break;
        case SW4:
        // increase minute, and check hour update
            if(clock_state->clock_mode!=MODIFY_MODE){
                break;
            }
            clock_state->volatile_m = (clock_state->volatile_m + 1);
            clock_state->volatile_h = (clock_state->volatile_h + (clock_state->volatile_m/60))%24;
            (clock_state->volatile_m)%=60;                    
            break;
    }
}
void clock_mode_func(int switch_code, int input_flag, struct clockState *clock_state, struct sbuf_output* o_sbuf){
    struct tm *time_ds;
    time_t raw_t = time(NULL);
    time_ds = localtime(&raw_t);

    // check input exist and handle switch code
    if(input_flag !=0){
        clock_handle_switch_code(switch_code,clock_state,o_sbuf,time_ds);
    }
    // Set outpub shared buffer with formatted data
    if(clock_state->clock_mode==FIX_MODE){
        // set first led
        o_sbuf->led = 1<<7;
        // set current_time(tm_hour, tm_min)+saved_time_diffrence(clock_hour, clock_min) to fnd output in FIX_MODE mode*/
        int hour_state=(time_ds->tm_hour+clock_state->clock_hour)+(time_ds->tm_min+clock_state->clock_min)/60;
        int min_state=time_ds->tm_min+clock_state->clock_min;
        sprintf(o_sbuf->fnd,"%02d%02d",(hour_state)%24,(min_state)%60);
        /* set led output in FIX_MODE mode*/
        
    }
    else if(MODIFY_MODE){
        sprintf(o_sbuf->fnd,"%02d%02d",clock_state->volatile_h,clock_state->volatile_m);
        // blinking 3rd and 4th led alternately : modifying mode
        if(time_ds->tm_sec%2 == 1){
            o_sbuf->led = 1<<4;
        }
        else {
            o_sbuf->led = 1<<5;    
        }
    }
    return;
}