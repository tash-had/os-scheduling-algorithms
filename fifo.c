#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int frame_to_evict;

/* Page to evict is chosen using the fifo algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int fifo_evict() {
	int old = frame_to_evict;
	frame_to_evict++;

	if (frame_to_evict == memsize){
		// reset frame_to_evict
		frame_to_evict = 0;
	}
	return old;
}

/* This function is called on each access to a page to update any information
 * needed by the fifo algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void fifo_ref(pgtbl_entry_t *p) {
}

/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void fifo_init() {
	frame_to_evict = 0;
}
