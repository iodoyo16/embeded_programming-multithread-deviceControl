///////////////////////////////////////////////////
//    Counter Mode
//    * count number with different base, set formated data to  output buffer(o_sbuf)
//     counter_state : save the coutner state

#include "main.h"
#include "counterMode.h"

void counter_handle_switch_code(int switch_code,struct counterState *counter_state,struct sbuf_output* o_sbuf){
    if(switch_code==KEY_VOL){
        memset(o_sbuf->dot_matrix,0x00,DOT_MAT_SIZE);
        memset(o_sbuf->text_lcd, 0, TEXTLCD_SIZE);
        // initialize counter
        counter_state->number = 0;
        counter_state->base = 10;
    }
    else if(switch_code==SW1){
        if(counter_state->base==10){
            counter_state->base=8;
        }
        else{
            counter_state->base=counter_state->base/2;
        }
        if(counter_state->base==1){
            counter_state->base=10;
        }
    }
    else if(switch_code==SW2){
        counter_state->number +=  (counter_state->base)* (counter_state->base);
    }
    else if(switch_code==SW3){
        counter_state->number += (counter_state->base);        
    }
    else if(switch_code==SW4){
        counter_state->number += 1;        
    }
}

void counter_mode_func(int switch_code, int input_flag, struct counterState *counter_state, struct sbuf_output* o_sbuf){

    int tmp;
    if(input_flag == 1){
        //switch updated;
        counter_handle_switch_code(switch_code,counter_state,o_sbuf);
    }
    // set led and formatting value and set sbuf
    if(counter_state->base==DECMODE){
        //set seond led on
        o_sbuf->led = 1 << 6;
        counter_state->number = (counter_state->number)%10000;
        sprintf(o_sbuf->fnd, "%04d",(counter_state->number));
        
    }
    else if(counter_state->base==OCTMODE){
        //set 3rd led on
        o_sbuf->led = 1 <<5;
        counter_state->number = (counter_state->number)%4096;
        sprintf(o_sbuf->fnd,"%04o",(counter_state->number));
    }
    else if(counter_state->base==QUAMODE){
        int i=0;
        //set 4th led on
        o_sbuf->led = 1 << 4;
        
        counter_state->number = (counter_state->number)%256;
        /* compute and save quadratic number*/
        tmp = (counter_state->number);
        sprintf(o_sbuf->fnd, "0000");
        while(tmp>0){
            (o_sbuf->fnd)[3-i] = '0'+tmp%4;
            tmp = tmp/4;
            i++;
        }
    }
    else if(counter_state->base==BINMODE){
        int i=0;
        //set first led on
        o_sbuf->led = 1 << 7;
        counter_state->number = (counter_state->number)%16;
        tmp = (counter_state->number);
        sprintf(o_sbuf->fnd, "0000");
        while(tmp>0){
            (o_sbuf->fnd)[3-i] = '0'+tmp%2;
            tmp = tmp/2;
            i++;
        } 
    }

    return;
}