/////////////////////////////////////////////////
//Text editor Mode:
//        type character and number on text lcd 
//        
//        *param
//         text_editor_state : save text editor state
#include "main.h"
#include "textEditorMode.h"

// keyboard value mapping
unsigned char charInput[9][3] = {{'.','Q','Z'},{'A','B','C'},{'D','E','F'},{'G','H','I'},{'J','K','L'},{'M','N','O'},{'P','R','S'},{'T','U','V'},{'W','X','Y'}};


// to indicate mode on dotMatrix 
unsigned char dotMat_alpha[10] = {0x1c, 0x36, 0x63, 0x63, 0x63, 0x7f, 0x7f, 0x63, 0x63, 0x63};
unsigned char dotMat_num[10] = {0x0c, 0x1c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3f, 0x3f};

int get_switch_num(int code){
    int i;
    int masking = 1;
    for(i=9; i>0; i--){
        if((masking & code)!=0)
            return i;
        code = code >> 1;
    }
    return -1;
}
void text_editor_mode_func(int switch_code, int input_flag, struct textEditorState *text_editor_state, struct sbuf_output* o_sbuf){


    int switch_num, i;

    if(input_flag == 1){
        if(switch_code==KEY_VOL){
            //initialize state    
            o_sbuf->led = 0;
            memset(text_editor_state->text , 0, TEXTLCD_SIZE);
            text_editor_state->cnt = 0;
            text_editor_state->idx = 0;
            text_editor_state->mode = ENG_MODE;
        }
        else if(switch_code==(SW2|SW3)){
            // text lcd clear
            for(i=0; i<TEXTLCD_SIZE ; i++){
                (text_editor_state->text)[i] = 0 ;
            }
            text_editor_state->idx = 0;
            text_editor_state->cnt = 0;
        }
        else if(switch_code==(SW5|SW6)){
            // chagne inut char number<=>english
            if(text_editor_state->mode==1)
                text_editor_state->mode = 0;
            else{
                text_editor_state->mode = 1;
            }
        }
        else if(switch_code==(SW8|SW9)){
            //  adding space
            int cur_idx=text_editor_state->idx;
            int cur_cnt=text_editor_state->cnt;
            text_editor_state->text[cur_idx] = ' ';
            text_editor_state->idx += 1; 
            text_editor_state->cnt = (cur_cnt+1)%10000;
        }
        else{
            int cur_idx=text_editor_state->idx;
            int cur_mode = text_editor_state->mode;
            int cur_cnt = text_editor_state->cnt;
            switch(switch_code){
                case SW1:
                case SW2:
                case SW3:
                case SW4:
                case SW5:
                case SW6:
                case SW7:
                case SW8:
                case SW9:
                    switch_num = get_switch_num(switch_code);
                    if(switch_num==-1)
                        break;
                    text_editor_state->cnt = (cur_cnt+1)%10000;
                    if(cur_mode == NUM_MODE){ 
                        text_editor_state->text[cur_idx] = '0'+switch_num;
                        text_editor_state->idx++;
                    }else{
                        char prev_input_char = '0';
                        if(cur_idx >0){
                            prev_input_char = text_editor_state->text[cur_idx-1]; //get prev character
                        }
                        if(cur_idx==0 ||  (prev_input_char != charInput[switch_num-1][0] && prev_input_char != charInput[switch_num-1][1] )){ 
                            // new character push
                            text_editor_state->text[cur_idx] = charInput[switch_num-1][0];
                            text_editor_state->idx += 1;
                        }
                        else if(prev_input_char == charInput[switch_num-1][0]){
                            // push twice
                                text_editor_state->text[cur_idx-1] = charInput[switch_num-1][1];
                        }
                        else if(prev_input_char == charInput[switch_num-1][1]){
                            // push three times
                            text_editor_state->text[cur_idx-1] = charInput[switch_num-1][2];
                        }
                    } 
                    break;
                default:
                    break;
            }
        }
    }
    // push character if index is more than 32
    if(text_editor_state->idx >= TEXTLCD_SIZE){
        int i;
        for(i=0; i<TEXTLCD_SIZE-1; i++){
            text_editor_state->text[i] = text_editor_state->text[i+1];
        }
        text_editor_state->text[TEXTLCD_SIZE-1] = 0;
        text_editor_state->idx = TEXTLCD_SIZE-1;
    }

    // set output buffer to show 'A' or '1'
    if(text_editor_state->mode==NUM_MODE){
        strncpy(o_sbuf->dot_matrix,dotMat_num,10);
    }
    else if(text_editor_state->mode==ENG_MODE){
        strncpy(o_sbuf->dot_matrix, dotMat_alpha,10);
    }

    // set fnd and textEditor sbuf
    sprintf(o_sbuf->fnd,"%04d",text_editor_state->cnt);
    sprintf(o_sbuf->text_lcd,"%s",text_editor_state->text);
    
    return;
}

