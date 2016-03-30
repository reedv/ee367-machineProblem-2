/*
 * createcode.c
 *
 *  Created on: Mar 29, 2016
 *      Author: reedvilanueva
 */


/*
 * Input:  a frequency file (from frequency367.c)
 *
 * Output:
 *	1. idk what the fuck he's asking for, as he uses only a specific example to hint at what he wants
 *	2. file containing an encoded huffman tree (see end of section #3 in handout; don't use brackets or commas)
 *
 *
 * To run the program:
 *
 * ./createcode <input data file>  <output file>
 */

#include <string.h>

#define MAX_PQ 511  // 256 = n poss. ascii chars, n-1 internal nodes, 2n-1 total nodes
#define EMPTY -1

typedef struct KV {
	char symbol[8];  // store binary string of ascii value
	float freq;
} KV;

typedef struct PQ {
	int size;
	KV pq[MAX_PQ];
} PQ;


KV PQ_parent(PQ pq, int i) {
	return pq[i/2];
}

KV PQ_left(PQ pq, int i) {
	return pq[i*2];
}

KV PQ_right(PQ pq, int i) {
	return pq[i*2 + 1];
}

// add a new element to the priority queue
void PQ_insert(PQ pq, KV sf) {
	pq.size++;

	KV empty;
	strlcpy(empty.symbol, "", sizeof(empty.symbol));
	empty.freq = -1;
	pq[pq.size] = empty;

}

// pop lowest freq. element from priority queue
void PQ_popMin(PQ pq) {

}


int main(int argc, char *argv[]) {

	FILE *input_fp,
		 *output_fp;

	PQ pqueue;
	pqueue.size = 0;

	// read in each symbol and freq.
	// place each pair in a min-priority queue sorted by freq. (implemented as unsorted array)

	// build huffman tree using the min-priority queue

	// traverse tree in preorder to create encoded huffman tree as binary string (see end of section #3)

	// write encoded huffman tree to output file
}
