#ifndef THEME_H_
#define THEME_H_

#include "nyarlathotep.h"
#include "platform/platform.h"
#include "libds/aojls.h"
#include "utils.h"

typedef struct theme {
	struct rgb_color window_list_color;
	struct rgb_color window_selection_color;
	struct {
		int height, offset;
	} window_list_sizes;
} theme_t;

#endif /* THEME_H_ */
