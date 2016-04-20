/*
 * createcode.c
 *
 *  Created on: Mar 29, 2016
 *      Author: reedvilanueva
 */


/*
 * Input:  a frequency file (from frequency367.c)
 *
 * Output: a file with two fields
 *	1. A 14-bit binary representation of the total length of the file
 *	   (i.e. the length of 14-bit string + length of huffman encoding)
 *	2. An encoded huffman tree (see end of section #3 in handout; don't use brackets or commas)
 *
 *
 * To run the program:
 *
 * ./createcode <input data file>  <output file>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This constant can be avoided by explicitly calculating height of Huffman Tree
#define MAX_TREE_HT 100
#define INTERNAL_SENTINEL '$'


/* A Huffman tree node */
struct MinHeapNode
{
    char data;  // One of the input characters
    float freq;  // Frequency of the character
    struct MinHeapNode *left, *right; // Left and right child of this node
};

/* A Min Heap:  Collection of min heap (or Hufmman tree) nodes */
struct MinHeap
{
    unsigned size;    // Current size of min heap
    unsigned capacity;   // capacity of min heap
    struct MinHeapNode **array;  // Array of min-heap node pointers
};


/*******************************************
 * MinHeapNode logic
 *
 *****************************************************/
/* A utility function to allocate a new min heap node with
 * given character and frequency of the character
 */
struct MinHeapNode* MinHeapNode_newNode(char data, float freq)
{
	// init empty MinHeapNode
    struct MinHeapNode* temp =
          (struct MinHeapNode*) malloc(sizeof(struct MinHeapNode));

    // fill node
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;

    return temp;
}

/* Utility function to check if this node is leaf */
int MinHeapNode_isLeaf(struct MinHeapNode* root)
{
    return !(root->left) && !(root->right) ;
}


/*******************************************
 * MinHeap logic
 *
 *****************************************************/

/* A utility function to create an empty min heap of given capacity */
struct MinHeap* MinHeap_create(unsigned capacity)
{
	// init empty MinHeap
    struct MinHeap* minHeap =
         (struct MinHeap*) malloc(sizeof(struct MinHeap));

    // init fields of MinHeap
    minHeap->size = 0;  // current size is 0
    minHeap->capacity = capacity;
    minHeap->array =
     (struct MinHeapNode**)malloc(minHeap->capacity * sizeof(struct MinHeapNode*));

    return minHeap;
}

/* A utility function to swap two MinHeapNodes */
void MinHeap_swapNodes(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* temp = *a;
    *a = *b;
    *b = temp;
}

/* The standard minHeapify function.
 * Recursively heapifies the subtree rooted at index
 * */
void MinHeap_heapify(struct MinHeap* minHeap, int index)
{
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    // if left child index w/in bounds (exists)
    // and freq of left child < freq of current smallest
    if (left < minHeap->size &&
        minHeap->array[left]->freq < minHeap->array[smallest]->freq)
    		smallest = left;

    // if right child index w/in bounds (exists)
    // and freq of right child < freq of current smallest
    if (right < minHeap->size &&
        minHeap->array[right]->freq < minHeap->array[smallest]->freq)
    		smallest = right;

    // if index we are heapifying on is not smallest in subtree
    if (smallest != index) {
    	// node at index "floats down" thru left or right subtree
        MinHeap_swapNodes(&minHeap->array[smallest], &minHeap->array[index]);
        // The node indexed by smallest, however, now has the original value A[index] in it,
        // and thus the subtree rooted at smallest might violate the max-heap property.
        MinHeap_heapify(minHeap, smallest);
    }

    // else node at index is smallest, then the subtree rooted at node at index is already
    // a min-heap and the procedure terminates.
}

/* A utility function to check if size of heap is 1 or not */
int MinHeap_isSizeOne(struct MinHeap* minHeap)
{
    return (minHeap->size == 1);
}

/* A standard function to extract minimum value node from heap */
struct MinHeapNode* MinHeap_extractMin(struct MinHeap* minHeap)
{
	// pop off the top of the heap
    struct MinHeapNode* temp = minHeap->array[0];

    // put bottom-most elem at top and heapify all the ways back down
    minHeap->array[0] = minHeap->array[minHeap->size-1];
    (minHeap->size)--;
    MinHeap_heapify(minHeap, 0);

    return temp;
}

/* A utility function to insert a new node to Min Heap */
void MinHeap_insert(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode)
{
	// init node to be at very bottom of heap
    (minHeap->size)++;
    int i = minHeap->size - 1;

    // move node up heap until it is no longer smaller than its parent
    while (i && minHeapNode->freq < minHeap->array[(i - 1)/2]->freq)
    {
        minHeap->array[i] = minHeap->array[(i - 1)/2];
        i = (i - 1)/2;
    }
    minHeap->array[i] = minHeapNode;
}

/* A standard function to build min heap from initially unheapified nodes*/
void MinHeap_build(struct MinHeap* minHeap)
{
    int n = minHeap->size - 1;
    int i;
    for (i = (n-1) / 2; i >= 0; i--)
    	MinHeap_heapify(minHeap, i);
}

/*
 * Creates a min heap of capacity equal to size and inserts all characters from
 * data[] in min heap. Initially size of min heap is equal to capacity
 * */
struct MinHeap* MinHeap_createAndBuild(char data[], float freq[], int size)
{
    struct MinHeap* minHeap = MinHeap_create(size);

    // add all nodes with data and corresponding freq
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = MinHeapNode_newNode(data[i], freq[i]);
    minHeap->size = size;

    // build initial heap w/ given nodes
    MinHeap_build(minHeap);
    return minHeap;
}


/*******************************************
 * Huffman  logic
 *
 *****************************************************/

// The main function that builds Huffman tree
struct MinHeapNode* buildHuffmanTree(char data[], float freq[], int size)
{
    struct MinHeapNode *left, *right, *top;

    // Create a min heap of capacity equal to size.
    // Initially, the number of nodes is equal to size.
    struct MinHeap* minHeap = MinHeap_createAndBuild(data, freq, size);

    // Iterate while size of heap doesn't become 1
    while (!MinHeap_isSizeOne(minHeap))
    {
        // Step 2: Extract the two smallest freq items from min heap
        left = MinHeap_extractMin(minHeap);
        right = MinHeap_extractMin(minHeap);

        // Step 3:  Create a new internal node with frequency equal to the
        // sum of the two nodes frequencies. Make the two extracted nodes
        // left and right children of this new node. Add this node to the min heap
        top = MinHeapNode_newNode(INTERNAL_SENTINEL, left->freq + right->freq);
        top->left = left;
        top->right = right;
        MinHeap_insert(minHeap, top);
    }

    // Step 4: The remaining node is the root node and the tree is complete.
    return MinHeap_extractMin(minHeap);
}


/* A utility function to print an array of size n */
void printArr(int arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        printf("%d", arr[i]);
    printf("\n");
}

void printArr_char(char arr[], int n)
{
    int i;
    for (i = 0; i < n; ++i)
        printf("%c", arr[i]);
    printf("\n");
}

/*
 * A utility function that stores length-bit binary representation of input int i in char array buffer
 */
void int2binStr(char binstring_buffer[], int length, int dec) {
	int pos;
	for (pos = length-1; pos >= 0; pos--) {
		if (dec % 2)
			binstring_buffer[pos] = '1';
		else binstring_buffer[pos] = '0';
		dec /= 2;
	}
}

// Prints huffman codes from the root of Huffman Tree.
// Stores codes in codes[]
// todo: convert function to encode huffman tree, rather then the codewords
void printCodes(struct MinHeapNode* root, int codes[], int top, char code_buffer[])
{
	//Moves thru tree in preorder

    // Assign 0 to left edge and recur
    if (root->left)
    {
        codes[top] = 0;
        printCodes(root->left, codes, top + 1, code_buffer);
    }

    // Assign 1 to right edge and recur
    if (root->right)
    {
        codes[top] = 1;
        printCodes(root->right, codes, top + 1, code_buffer);
    }

    // If this is a leaf node, then it contains one of the input
    // characters, print the character and its code from codes[]
    if (MinHeapNode_isLeaf(root))
    {
    	printf("** createcodes.c/printCodes: isLeaf reached\n");
    	//top++;
    	//codes[top] = 0;
    	printf("%c: ", root->data);
    	printArr(codes, top);

        // prepend to encoded huffman string in the form (data)+(codes)
    	// concat leaf encoding to code_buffer
    	int i;
		for(i=0; i < top; i++) {
			char encoding_buffer[] = {codes[i] + '0', '\0'};
			printf("createcodes.c/printCodes: encoding_buffer[0]=%c\n", encoding_buffer[0]);
			strcat(code_buffer, encoding_buffer);
		}
		// concat leaf data to code_buffer
    	char data_buffer[] = {root->data, '\0'};
    	printf("createcodes.c/printCodes: data_buffer[0]=%c\n", data_buffer[0]);
    	strcat(code_buffer, data_buffer);
    }
}

// The main function that builds a Huffman Tree and print codes by traversing
// the built Huffman Tree
void HuffmanCodes(char data[], float freq[], char code_buffer[], int size)  // todo: needs to take a string buffer for encoded tree
{
   //  Construct Huffman Tree
   struct MinHeapNode* root = buildHuffmanTree(data, freq, size);

   // Print Huffman codes using the Huffman tree
   int arr[MAX_TREE_HT],
   	   top = 0;
   printCodes(root, arr, top, code_buffer);

   // TODO: store huffman tree encoded as single binary string

}

int main(int argc, char *argv[])
{
	// Check if there are correct number of arguments
//	if (argc != 3) {
//	   printf("Usage:  createcode <input list file> <output encoded huffman file>\n");
//	   return 1;
//	}

	FILE *input_fp,
		 *output_fp;

	//todo: init symbol and freq arrays from the input file
//	input_fp = fopen(argv[1], "r");
	char symbols[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
	float freq[] = {0.7, 0.2, 0.1, 0.3, 0.4, 0.5, 0.8, 0.6};
	if(sizeof(symbols)/sizeof(symbols[0]) != sizeof(freq)/sizeof(freq[0])) {
		printf("createcode.c/main Error: symbol and frequency array size mismatch.\n");
		exit(1);
	}
    char code_buffer[10000] = "\0";  // arbitrary size, init. with NULL char to concat to later
    int size = sizeof(symbols)/sizeof(symbols[0]);

    // store huffman tree encoding as single binary string
    HuffmanCodes(symbols, freq, code_buffer, size);
    int buffer_size = sizeof(code_buffer)/sizeof(code_buffer[0]);
    printf("** createcodes.c/main: buffer_size = %d\n", buffer_size);
    printArr_char(code_buffer, buffer_size);

    // count length of encoded tree string (plus 14 extra bits) and store length as a 14-bit binary string
    int charcount = strlen(code_buffer);
//    int m;
//    int charcount = 0;
//    for(m=0; code_buffer[m]; m++) {
//        if(code_buffer[m] != ' ' || code_buffer[m] != '\0') {
//        	printf("** createcode.c/main: code_buffer[m]=%c\n", code_buffer[m]);
//            charcount ++;
//        }
//    }
    printf("** createcode.c/main: encoding_size (w/out 14-bit extra) = %d\n", charcount);
    char codesize_buffer[14];
    int2binStr(codesize_buffer, 14, charcount+14);
    printArr_char(codesize_buffer, 14);

    // prepend the length and encoded tree to output file
//    output_fp = fopen(argv[2], "w");

//    fclose(output_fp);
    return 0;
}
