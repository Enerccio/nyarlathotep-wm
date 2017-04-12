#include "nyarlathotep.h"

typedef struct animator_data {
	animation_request callback;
	void* data;
	struct wlc_event_source* event_source;
	uint32_t delay;
} animator_data_t;

int on_animation(void* passdata) {
	animator_data_t* animator_data = (animator_data_t*)passdata;
	bool stop = animator_data->callback(animator_data->data);
	if (stop) {
		wlc_event_source_remove(animator_data->event_source);
		free(animator_data);
	} else {
		if (!wlc_event_source_timer_update(animator_data->event_source,
				animator_data->delay)) {
			free(animator_data);
		}
	}
	return 0;
}

void register_animation(animation_request request, void* data, int delay) {
	animator_data_t* passarg = malloc(sizeof(animator_data_t));
	struct wlc_event_source* event_source = wlc_event_loop_add_timer(on_animation, passarg);
	passarg->callback = request;
	passarg->event_source = event_source;
	passarg->data = data;
	passarg->delay = delay;
	if (!wlc_event_source_timer_update(event_source, delay)) {
		free(passarg);
	}
}
