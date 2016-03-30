/*
 * huffman_withHeaps.c
 *
 *  Created on: Mar 29, 2016
 *      Author: reedvilanueva
 */


#include <stdio.h>
#include <stdlib.h>

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

// Prints huffman codes from the root of Huffman Tree.
// It uses arr[] to store codes
void printCodes(struct MinHeapNode* root, int arr[], int top)
{
	//Moves down tree in preorder

    // Assign 0 to left edge and recur
    if (root->left)
    {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    // Assign 1 to right edge and recur
    if (root->right)
    {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    // If this is a leaf node, then it contains one of the input
    // characters, print the character and its code from arr[]
    if (MinHeapNode_isLeaf(root))
    {
        printf("%c: ", root->data);   printArr(arr, top);
    }
}

// The main function that builds a Huffman Tree and print codes by traversing
// the built Huffman Tree
void HuffmanCodes(char data[], float freq[], int size)
{
   //  Construct Huffman Tree
   struct MinHeapNode* root = buildHuffmanTree(data, freq, size);

   // Print Huffman codes using the Huffman tree built above
   int arr[MAX_TREE_HT], top = 0;
   printCodes(root, arr, top);
}

// Driver program to test above functions
int main()
{
    char arr[] = {'a', 'b', 'c', 'd', 'e', 'f'};
    float freq[] = {0.05, 0.09, 0.12, 0.13, 0.16, 0.45};
    int size = sizeof(arr)/sizeof(arr[0]);
    HuffmanCodes(arr, freq, size);
    return 0;
}
