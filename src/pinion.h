/*
 * Grouped header for all subsystems.
 *
 * All subsystems must import this header.
 */
#ifndef PINION_H_
#define PINION_H_

#include <stdio.h>
#include <stdlib.h>

#include <wlc/wlc.h>

// workspace.c
void init_workspaces();

bool output_created(wlc_handle output);
void output_terminated(wlc_handle output);
// workspace.c ends

#endif /* PINION_H_ */
