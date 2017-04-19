#ifndef NWM_API_H_
#define NWM_API_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#define MAX_CHAR_LEN_DECIMAL_INTEGER(type) (10*sizeof(type)*CHAR_BIT/33 + 2)
#define MAX_CHAR_SIZE_DECIMAL_INTEGER(type) (10*sizeof(type)*CHAR_BIT/33 + 3)

/* Communication channel id used for general commands */
#define NWM_GENERAL_COMMUNICATION_DOMAIN (1)

#ifndef NWM_TMP_FILE_PREFIX
#ifdef linux
#define NWM_TMP_FILE_PREFIX "nwm-comm-chan"
#else
#error not supported platform
#endif
#endif

#ifndef NWM_TMP_PATH
#ifdef linux
#define NWM_TMP_PATH "/tmp/"
#else
#error not supported platform
#endif
#endif

static inline char* nwm_generate_pipe_name(uint32_t channel_id) {
	char* path = (char*)malloc(sizeof(char) *
			(strlen(NWM_TMP_PATH) + strlen(NWM_TMP_FILE_PREFIX) + MAX_CHAR_SIZE_DECIMAL_INTEGER(uint32_t) + 1));
	if (path == NULL)
		return NULL;
	sprintf(path, "%s%s%d", NWM_TMP_PATH, NWM_TMP_FILE_PREFIX, channel_id);
	return path;
}

#ifdef linux
static inline int nwm_get_communication_channel_fd(uint32_t channel_id) {
	int fd = fileno(fopen(nwm_generate_pipe_name(channel_id), "wb"));
	return fd;
}
#else
#error not supported platform
#endif

#endif
