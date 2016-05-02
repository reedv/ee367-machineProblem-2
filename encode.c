/*
 * encode.c
 *
 *  Created on: Apr 30, 2016
 *      Author: reedvilanueva
 */


/*
 * Input:	Encoded Huffman tree file (from createcode.c)
 * 			Original data file (that tree is based on) to be encoded
 *
 * Output: a compressed version of the data file based on the encoded Huffman tree
 *
 *
 * To run the program:
 *
 * ./encode <input codebook file> <input data file>  <output file>
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// This constant can be avoided by explicitly calculating height of Huffman Tree
#define MAX_WORD_LENGTH 256


/* A tree node has data,
 * pointer to left child
 * and a pointer to right child */
struct Node
{
    int data;
    struct Node *left;
    struct Node *right;
};

/* Codebook entries are to be stored in an array[256] (each elem. reps. an ascii value)
 * each entry has a valid bit (whether this ascii value has a codeword or not),
 * codeword (based on Huffman tree),
 * and the codeword length */
struct CodeEntry
{
	int valid;
	char codeword[MAX_WORD_LENGTH];
	int length;
};

/*******************************
 *
 * Utility functions
 *
 *******************************/
// A utility function to init. a Node
struct Node* newNode (int data)
{
    struct Node* temp = (struct Node *) malloc( sizeof(struct Node) );

    temp->data = data;
    temp->left = temp->right = NULL;

    return temp;
}

// Utility function to init. a CodeEntry
struct CodeEntry newCode(int valid, char word[], int length)
{
	struct CodeEntry temp;
	temp.valid = valid;
	strcpy(temp.codeword, word);
	temp.length = length;

	return temp;
}

/* Utility function to check if this node is leaf */
int isLeaf(struct Node* root)
{
    return !(root->left) && !(root->right) ;
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
 * recursively builds a binary representation of an int (as an int)
 */
int int2binInt(int dec) {
	if (dec == 0) return 0;
	if (dec == 1) return 1;
	return (dec % 2) + 10*int2binInt(dec/2);
}

/*
 * stores 8-bit binary representation of input int i in char array buffer
 */
void int2binStr(char * binstring_buffer, int dec) {
	int pos;
	for (pos = 7; pos >= 0; pos--) {
		if (dec % 2)
			binstring_buffer[pos] = '1';
		else binstring_buffer[pos] = '0';
		dec /= 2;
	}
}

/* Function to convert binary (int) to decimal (int)*/
int binInt2decInt(int bin)
{
    int decimal=0,
    	i=0,
		rem;

    while(bin != 0)
    {
        rem = bin % 10;
        bin /= 10;
        decimal += rem * pow(2,i);
        i++;
    }

    return decimal;
}


/*******************************
 *
 *  Reconstructing Huffman tree
 *
 *******************************/
struct Node * reconstructPreUtil(int pre[], int * preIndex, int size)
{
	// Base case
	if (*preIndex >= size)
	{
		printf("** encode.c/reconstructPreUtil: base reached\n");
		return NULL;
	}

	struct Node * root = newNode(pre[*preIndex]);

	// if data in preorder stream is 1 (the data is for internal node)
	// (will always be the case for the first elem. in stream)
	if (root->data == 1)
	{
		// continue building in preorder fashion
		printf("** encode.c/reconstructPreUtil: preindex=%d, root->data = %d\n", *preIndex, root->data);
		*preIndex = *preIndex + 1;
		printf("** encode.c/reconstructPreUtil: going left\n");
		root->left = reconstructPreUtil(pre, preIndex, size);
		printf("** encode.c/reconstructPreUtil: going right\n");
		root ->right = reconstructPreUtil(pre, preIndex, size);

		// return this internal Node
		return root;
	}
	// if data is 0 (indicating a leaf)
	else if(root->data == 0)
	{
		// jump to next elem (the data for the leaf)
		*preIndex = *preIndex + 1;
		struct Node * root = newNode(pre[*preIndex]);
		printf("** encode.c/reconstructPreUtil: preindex=%d, root->data = %d\n", *preIndex, root->data);
		root->left = root->right = NULL;
		*preIndex = *preIndex + 1;

		// return the leaf
		return root;
	}
}

struct Node * reconstructPre(int pre[], int size)
{
	int preIndex = 0;
	return reconstructPreUtil(pre, &preIndex, size/*, root*/);
}

// A utility function to print inorder traversal of a Binary Tree
void printPreorder (struct Node* node)
{

    if (node == NULL)
    	return;

    printf("%d ", node->data);
    if (isLeaf(node))
		printf("** encode.c/printPreorder: leaf reached\n");

    printPreorder(node->left);
    printPreorder(node->right);
}



/*******************************
 *
 * Filling codebook
 *
 *******************************/

void fillCodebook(struct Node * root, struct CodeEntry codebook[], int code_buff[], int top)
{
	printf("** encode.c/fillcodebook: root->data=%d\n", root->data);
	// Assign 0 to left edge and recur
	if (root->left)
	{
		code_buff[top] = 0;
		printf("** encode.c/fillcodebook: going left, code_buff[top=%d]=%d\n", top, code_buff[top]);
		fillCodebook(root->left, codebook, code_buff, top+1);  // NOTE: ++a != a++
	}

	// Assign 1 to right edge and recur
	if (root->right)
	{
		code_buff[top] = 1;
		printf("** encode.c/fillcodebook: going right, code_buff[top=%d]=%d\n", top, code_buff[top]);
		fillCodebook(root->right, codebook, code_buff, top+1);
	}

	// If this is a leaf node, then it contains one of the input characters,
	// print the character and its code from code_buff[]
	if (isLeaf(root))
	{
		printf("%d: ", root->data);
		printArr(code_buff, top);

		// set codebook[data] entry
		codebook[root->data].valid = 1;

		// convert code_buff ints to chars to set as codeword
		char code_buff_chars[top+1];
		int i;
		for (i=0 ; i < top ; i++)
		{
		    code_buff_chars[i] = code_buff[i] + '0';
		    printf("** encode.c/fillcodebook/leaf: code_buff_chars[%d]=%c\n", i, code_buff_chars[i]);
		}
		code_buff_chars[top] = '\0';
		printf("** encode.c/fillcodebook/leaf: code_buff_chars=%s\n", code_buff_chars);
		strcpy(codebook[root->data].codeword, code_buff_chars);

		codebook[root->data].length = strlen(code_buff_chars);
	}

}



// Driver program to test above functions
int main(int argc, char *argv[])
{
	//    10
	//   /   \
	//  5     40
	// /  \      \
	//1    7      50
    //int pre[] = {10, 5, 1, 7, 40, 50};

	// reconstruct huffman tree from preorder encoding
	int pre[] = {1, 1, 0, 32, 1, 0, 101, 1, 0, 84, 0, 104, 1, 0, 115, 1, 0, 105, 0, 116};
    //int size = sizeof( pre ) / sizeof( pre[0] );
    //printf("** encode.c/main: size=%d\n", size);

    // TODO:
    // read in 2nd field of encoded Huffman tree file to build preorder array, pre, of tree
    // the size of pre can be found using the 1st field of the encoding file: (its value) - 14

    /* Build preorder stream from input encoded Huffman tree*/
    //Check if there are correct number of arguments
	if (argc != 3) {
	   printf("Usage:  createcode <input list file> <output encoded huffman file>\n");
	   return 1;
	}

	FILE *input_fp,
		 *output_fp;

	// get size of preorder stream
	input_fp = fopen(argv[1], "r");
	char in_buff[MAX_WORD_LENGTH];
	fscanf(input_fp, "%s", in_buff);
	int size = binInt2decInt(atoi(in_buff));
	printf("** encode.c/main: in_fp, found size = %d = %d\n", size, int2binInt(size));
	size = size - 14;

	// get rid of newline char so we can read in char-by-char later.
	// From http://www.cplusplus.com/reference/cstdio/fscanf/:
	// Whitespace character: the function will read and ignore any whitespace characters
	// encountered before the next non-whitespace character (whitespace characters include spaces,
	// newline and tab characters -- see isspace). A single whitespace in the format string validates
	// any quantity of whitespace characters extracted from the stream (including none).
	fscanf(input_fp, "%c", in_buff);
	printf("** encode.c/main: reading newline from input file: %s\n", in_buff);

	// fill preorder stream from input file
	int pre_stream[size];
	int k;
	for(k=0; k < size; k++) {
		fscanf(input_fp, "%c", in_buff);
		in_buff[1] = '\0';
		pre_stream[k] = atoi(in_buff);
		printf("** encode.c/main: filling stream: k=%d: symbols[k]=%d\n", k, pre_stream[k]);
	}
	fclose(input_fp);
	exit(1);  // FIXME: Remove: this is just for debugging

    /* reconstruct Huffman tree from preorder stream */
    struct Node * root = reconstructPre(pre, size);

    printf("** encode.c/main: Preorder traversal of reconstructed Huffman tree\n");
    printPreorder(root);

    /* build codebook array from preorder traversal of reconstructed tree */
    int top = 0;
    int code_buff[MAX_WORD_LENGTH];
    struct CodeEntry codebook[MAX_WORD_LENGTH];
    // init. codebook
    int i;
	for(i=0; i < MAX_WORD_LENGTH; i++)
	{
		codebook[i].valid = 0;
		strcpy(codebook[i].codeword, "");
		codebook[i].length = 0;
	}
    fillCodebook(root, codebook, code_buff, top);

    printf("** encode.c/main: Filled codebook\n");
    int j;
    for(j=0; j < MAX_WORD_LENGTH; j++)
    {
    	if(codebook[j].valid == 1)
    	{
    	printf("codebook[%d]\n", j);
    	printf("\tvalid: %d\n\tword: %s\n\tlength:%d\n",
    			codebook[j].valid, codebook[j].codeword, codebook[j].length);
    	printf("\n");
    	}
    }


    return 0;
}








