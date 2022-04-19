////////////////////////////////////////////
//    Draw Board Mode
//    draw on dot matrix and control direction
//    *param
//    draw_board_state : save the draw information


#include "main.h"
#include "drawBoardMode.h"

void draw_board_mode_func(int switch_code, int input_flag, struct drawBoardState *draw_board_state, struct sbuf_output *o_sbuf) {
    
    
    int i, j;
    int isUpdate = B_NOT_UPDATED;
    unsigned char line[7];

    /* time for cursor blinking */
    struct tm *time_ds;
    time_t raw_t = time(NULL); //get raw time
    time_ds = localtime(&raw_t);

    if (input_flag) {
        isUpdate = B_UPDATED;
        drawBoard_handle_switch_code(switch_code,draw_board_state,o_sbuf);
    }
    //set operation count
    (draw_board_state->cnt)%=10000;
    sprintf(o_sbuf->fnd,"%04d",draw_board_state->cnt);

    // set shared buff with dotmatrix datas
    if (isUpdate == B_UPDATED) {
        for (i = 0; i < 10; i++) {
            o_sbuf->dot_matrix[i] = make_dotline_to_bit((draw_board_state->board_map)[i]);
        }
    }
    // cursor on
    if(draw_board_state->cursor_on == CURSOR_OFF)
        return;
        // add or minus cursor dot to line
    int line_bit;
    if(time_ds->tm_sec%2==0){
        line_bit = make_dotline_to_bit(draw_board_state->board_map[draw_board_state->pos_y]);
        line_bit = line_bit&0x7f - (1<<(6-draw_board_state->pos_x));
        (o_sbuf->dot_matrix)[draw_board_state->pos_y] =  line_bit;
    }else{
        line_bit = make_dotline_to_bit(draw_board_state->board_map[draw_board_state->pos_y]);
        line_bit = line_bit | (1<<6-draw_board_state->pos_x);
        (o_sbuf->dot_matrix)[draw_board_state->pos_y] =  line_bit;
    }
}
void drawBoard_handle_switch_code(int switch_code, struct drawBoardState *draw_board_state, struct sbuf_output *o_sbuf){
    int cur_x = draw_board_state->pos_x;
    int cur_y = draw_board_state->pos_y;
    if(switch_code==KEY_VOL||switch_code==SW1){
        //initialize board
        int i,j;
        o_sbuf->led = 0;
        o_sbuf->text_lcd[0] = 0;
        for (i = 0; i < 10; i++){
            for(j =0; j<7 ;j++) {
                (draw_board_state->board_map)[i][j] = 0x00;
            }
        }
        draw_board_state->pos_x = draw_board_state->pos_y = draw_board_state->cnt = 0;
        draw_board_state->cursor_on = CURSOR_ON;
    }
    else if(switch_code==SW2){
        // move up
        draw_board_state->cnt++;
        (cur_y)--;
        // up or rotate
        draw_board_state->pos_y = (cur_y < 0) ? 9 :cur_y;
    }
    else if(switch_code==SW3){
        // cursor blinking toggle
        draw_board_state->cnt++;
        draw_board_state->cursor_on = (draw_board_state->cursor_on==CURSOR_OFF)? CURSOR_ON : CURSOR_OFF;
    }
    else if(switch_code==SW4){
        // move left
        draw_board_state->cnt++;
        cur_x--;
        draw_board_state->pos_x = (cur_x < 0) ? 6 : cur_x;
    }
    else if(switch_code==SW5){
        // draw or remove dot
        int cur_state=(draw_board_state->board_map)[cur_y][cur_x];
        draw_board_state->cnt++;
        (draw_board_state->board_map)[cur_y][cur_x]=(cur_state==0)? 1:0;
    }
    else if(switch_code==SW6){
        // move right
        draw_board_state->cnt++;
        cur_x++;
        draw_board_state->pos_x = (cur_x > 6) ? 0 : cur_x;        
    }
    else if(switch_code==SW7){
        // clear board
        int i,j;
        draw_board_state->cnt++;
        for (i = 0; i < 10; i++){
            for(j=0; j< 7; j++){
                (draw_board_state->board_map)[i][j] = 0;
            }
        }
    }
    else if(switch_code==SW8){
        // move down or rotate
        draw_board_state->cnt++;
        (cur_y)++;
        draw_board_state->pos_y = (cur_y > 9) ? 0 : cur_y;
    }
    else if(switch_code==SW9){
        // revserse color
        int i,j;
        draw_board_state->cnt++;
        for (i = 0; i < 10; i++){
            for(j=0; j< 7; j++){
                (draw_board_state->board_map)[i][j] = ((draw_board_state->board_map)[i][j] == 1) ? 0 : 1;
            }
        }
    }
}

unsigned char make_dotline_to_bit(unsigned char line[7]) {
    // masking dot line to bit
    int num = 0;
    int i;
    for (i = 0; i < 7; i++) {
        num = num << 1;
        num +=line[i];
    }
    return num;
}