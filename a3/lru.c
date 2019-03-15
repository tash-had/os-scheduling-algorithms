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
	struct frame *vict = tail_frame;
	return vict->pte->frame >> PAGE_SHIFT;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	int frame_num = p->frame >> PAGE_SHIFT;
	struct frame *mru_frame = &coremap[frame_num];

	if (mru_frame == head_frame) {
		// there's either only 1 frame, or the frame that was referenced is
		// already at the head of our list. no work to be done here.
		return;
	}
	mru_frame->prev_pgt_frame->next_pgt_frame = mru_frame->next_pgt_frame;
	if (mru_frame == tail_frame) {
		tail_frame = tail_frame->prev_pgt_frame;
	} else {
		mru_frame->next_pgt_frame->prev_pgt_frame = mru_frame->prev_pgt_frame;
	}
	mru_frame->prev_pgt_frame = NULL;
	mru_frame->next_pgt_frame = head_frame;
	mru_frame->next_pgt_frame->prev_pgt_frame = mru_frame;
	head_frame = mru_frame;

	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	// set next and prev to NULL for every frame
	for (int i = 0; i < memsize; i++) {
		if (i > 0) {
			(&coremap[i])->prev_pgt_frame = &coremap[i-1];
		} else {
			(&coremap[i])->prev_pgt_frame = NULL;

		}
		if (i < memsize - 1) {
			(&coremap[i])->next_pgt_frame = &coremap[i+1];
		} else {
			(&coremap[i])->next_pgt_frame = NULL;
		}
	}
	head_frame = &coremap[0];
	tail_frame = &coremap[memsize - 1];
}
