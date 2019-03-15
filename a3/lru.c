#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

struct frame *head_frame;
struct frame *tail_frame;

int size;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	if (head_frame == NULL || tail_frame == NULL) {
		fprintf(stderr,"Encountered a NULL reference to head or tail frame while evicting from LRU cache.\n");
		exit(1);
	}
	struct frame *vict = tail_frame;
	if (tail_frame->prev_pgt_frame != NULL) {
		tail_frame->prev_pgt_frame->next_pgt_frame = tail_frame->next_pgt_frame;
	} else {
		// attempting to evict the only frame left. must update head.
		head_frame = NULL;
	}
	tail_frame = tail_frame->prev_pgt_frame;
	vict->prev_pgt_frame = NULL;
	return vict->pte->frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	struct frame lru_frame = coremap[p->frame];
	lru_frame.next_pgt_frame = head_frame;
	if (head_frame != NULL) {
		head_frame->prev_pgt_frame = &lru_frame;
	} else { // both head and tail are null. must set the tail to our lru_frame
		tail_frame = &lru_frame;
	}

	head_frame = &lru_frame;
	head_frame->prev_pgt_frame = NULL;

	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	// set next and prev to NULL for every frame
	for (int i = 0; i < memsize; i++) {
		coremap[i].prev_pgt_frame = NULL;
		coremap[i].next_pgt_frame = NULL;
	}
}
