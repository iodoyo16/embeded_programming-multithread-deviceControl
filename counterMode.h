/* ds for counter */
struct counterState {
    int base; //base
	int number; //number
};

/* base macro */
#define DECMODE 10
#define OCTMODE 8
#define BINMODE 2
#define QUAMODE 4


void counter_handle_switch_code(int switch_code,struct counterState *counter_state,struct sbuf_output* o_sbuf);
void counter_mode_func(int sw, int inflag, struct counterState *counter_state, struct sbuf_output* o_sbuf);
