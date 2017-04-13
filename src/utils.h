#ifndef UTILS_H_
#define UTILS_H_

#include <wlc/wlc.h>
#include "macros.h"

const char* logging_type_str(enum logging_type type);

bool is_parent_view(wlc_handle p, wlc_handle view);

#endif /* UTILS_H_ */
