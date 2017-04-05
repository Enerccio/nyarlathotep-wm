#ifndef WORKSPACE_H_
#define WORKSPACE_H_

#include <wlc/wlc.h>

typedef struct workspace {
	wlc_handle output;
} workspace_t;

workspace_t* create_workspace(wlc_handle handle);
void terminate_workspace(workspace_t* workspace);

#endif /* WORKSPACE_H_ */
