#include "nyarlathotep.h"
#include "platform/platform.h"
#include <nwm/nwm-api.h>

static int general_read(int fd, uint32_t mask, void* data) {
	// TODO
	return 0;
}

void init_communication() {
	int general_comm_channel_fd = open_communication_channel(NWM_GENERAL_COMMUNICATION_DOMAIN);

	wlc_event_loop_add_fd(general_comm_channel_fd, WLC_EVENT_READABLE, general_read, NULL);
}
