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

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int lru_evict() {
	struct frame *vict = tail_frame; // victim is always the tail
	return vict->pte->frame >> PAGE_SHIFT;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	int frame_num = p->frame >> PAGE_SHIFT;
	struct frame *mru_frame = &coremap[frame_num]; // most recently used frame

	if (mru_frame == head_frame) {
		// there's either only 1 frame, or the frame that was referenced is
		// already at the head of our list. no work to be done here.
		return;
	}
	// remove the mru_frame
	mru_frame->prev_pgt_frame->next_pgt_frame = mru_frame->next_pgt_frame;
	if (mru_frame == tail_frame) {
		// reset the tail if mru_frame is the current tail
		tail_frame = tail_frame->prev_pgt_frame;
	} else {
		// if mru_frame is not the current tail, update the prev pointer of the node after mru_frame
		mru_frame->next_pgt_frame->prev_pgt_frame = mru_frame->prev_pgt_frame;
	}
	// update pointers to make mru_frame the new head
	mru_frame->prev_pgt_frame = NULL;
	mru_frame->next_pgt_frame = head_frame;
	// set the prev pointer of the current head to mru_frame
	mru_frame->next_pgt_frame->prev_pgt_frame = mru_frame;
	head_frame = mru_frame;
}

/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	// link all the frames as a doubly linked list
	for (int i = 0; i < memsize; i++) {
		if (i > 0) {
			(&coremap[i])->prev_pgt_frame = &coremap[i-1];
		} else {
			// first frame should have a null prev pointer
			(&coremap[i])->prev_pgt_frame = NULL;

		}
		if (i < memsize - 1) {
			(&coremap[i])->next_pgt_frame = &coremap[i+1];
		} else {
			// last frame should have a null next pointer
			(&coremap[i])->next_pgt_frame = NULL;
		}
	}
	head_frame = &coremap[0];
	tail_frame = &coremap[memsize - 1];
}
