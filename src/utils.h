#ifndef UTILS_H_
#define UTILS_H_

#include <wlc/wlc.h>
#include "macros.h"

const char* logging_type_str(enum logging_type type);

bool is_parent_view(wlc_handle p, wlc_handle view);
char* read_file_to_string(const char* file, size_t* maxl);
char* copy_string(const char* src);

#endif /* UTILS_H_ */
