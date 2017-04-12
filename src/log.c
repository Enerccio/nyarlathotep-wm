#include <stdio.h>
#include "nyarlathotep.h"

void logger_callback(enum wlc_log_type type, const char *str) {
	// TODO: log to file and to journald
	printf(str);
	printf("\n");
}
