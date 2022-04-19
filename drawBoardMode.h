
#include <time.h>

// board update flag
#define B_UPDATED 1   
#define B_NOT_UPDATED 0

// cursor blink flag
#define CURSOR_ON 1
#define CURSOR_OFF 0

// ds for board state
struct drawBoardState{
    int cursor_on;  //cursor showing mode
    int cnt;        //counter
	unsigned char board_map[10][7]; //information about board (1 draw, 0 not draw)
    int pos_x;   //x coordinate of cursor
    int pos_y;   //y coordinate of cursor
};

void drawBoard_handle_switch_code(int switch_code, struct drawBoardState *draw_board_state, struct sbuf_output *o_sbuf);
void draw_board_mode_func(int switch_code, int input_flag, struct drawBoardState *draw_board_state, struct sbuf_output* o_sbuf);
unsigned char make_dotline_to_bit(unsigned char line[7]);