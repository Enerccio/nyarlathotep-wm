#include "pinion.h"

int main(void) {

	init_workspaces();

	wlc_set_output_created_cb(output_created);
	wlc_set_output_destroyed_cb(output_terminated);

	if (!wlc_init())
		return EXIT_FAILURE;

	wlc_run();

	return EXIT_SUCCESS;
}
