
#define ENG_MODE 0
#define NUM_MODE 1

struct textEditorState {
	int cnt;
    unsigned char text[TEXTLCD_SIZE];    
    int idx;
    int mode;
};
int get_switch_num(int code);
void text_editor_mode_func(int sw, int inflag, struct textEditorState *text_editor_state, struct sbuf_output* buf_out);
