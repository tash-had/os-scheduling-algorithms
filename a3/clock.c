#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int page_to_evict;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int clock_evict() {
	int found = 0;

	while(!found){
		pgtbl_entry_t *potential_evict = coremap[page_to_evict].pte;

		if (potential_evict->frame & PG_REF){
			// reset the reference bit
			potential_evict->frame &= ~PG_REF;
			page_to_evict++;

			if (page_to_evict >= memsize){
				// 'clock' has gone full circle
				page_to_evict = 0;
			}
		} else {
			found = 1;
		}	
	}

	return page_to_evict;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	p->frame |= PG_REF;
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	page_to_evict= 0;
}
