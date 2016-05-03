/*
 * decode.c
 *
 *  Created on: May 2, 2016
 *      Author: reedvilanueva
 */



/*
 * Input:	Encoded Huffman tree file (from createcode.c)
 * 			Compressed data file (that encoded tree was used for) to be decoded
 *
 * Output: a compressed version of the data file based on the encoded Huffman tree
 *
 *
 * To run the program:
 *
 * ./decode367 <input codebook file> <input compressed-data file>  <output decompressed file>
 */


#include <errno.h>
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
		printf("** decode.c/reconstructPreUtil: base reached\n");
		return NULL;
	}

	struct Node * root = newNode(pre[*preIndex]);

	// if data in preorder stream is 1 (the data is for internal node)
	// (will always be the case for the first elem. in stream)
	if (root->data == 1)
	{
		// continue building in preorder fashion
		printf("** decode.c/reconstructPreUtil: preindex=%d, root->data = %d\n", *preIndex, root->data);
		*preIndex = *preIndex + 1;
		printf("** decode.c/reconstructPreUtil: going left\n");
		root->left = reconstructPreUtil(pre, preIndex, size);
		printf("** decode.c/reconstructPreUtil: going right\n");
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
		printf("** decode.c/reconstructPreUtil: preindex=%d, root->data = %d\n", *preIndex, root->data);
		root->left = root->right = NULL;
		*preIndex = *preIndex + 1;

		// return the leaf
		return root;
	}

	return NULL;  // FIXME: This should never be reachable by virtue of the form of the input data (can delete this line)
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
		printf("** decode.c/printPreorder: leaf reached\n");

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
	printf("** decode.c/fillcodebook: root->data=%d\n", root->data);
	// Assign 0 to left edge and recur
	if (root->left)
	{
		code_buff[top] = 0;
		printf("** decode.c/fillcodebook: going left, code_buff[top=%d]=%d\n", top, code_buff[top]);
		fillCodebook(root->left, codebook, code_buff, top+1);  // NOTE: ++a != a++
	}

	// Assign 1 to right edge and recur
	if (root->right)
	{
		code_buff[top] = 1;
		printf("** decode.c/fillcodebook: going right, code_buff[top=%d]=%d\n", top, code_buff[top]);
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
		    printf("** decode.c/fillcodebook/leaf: code_buff_chars[%d]=%c\n", i, code_buff_chars[i]);
		}
		code_buff_chars[top] = '\0';
		printf("** decode.c/fillcodebook/leaf: code_buff_chars=%s\n", code_buff_chars);
		strcpy(codebook[root->data].codeword, code_buff_chars);

		codebook[root->data].length = strlen(code_buff_chars);
	}

}



// Driver program to test above functions
int main(int argc, char *argv[])
{
    /* Build preorder stream from input encoded Huffman tree*/
    //Check if there are correct number of arguments
	if (argc != 4) {
	   printf("Usage: ./decode367 <input encoding file> <input data file> <output compressed data file>\n");
	   return 1;
	}

	FILE *input_encodedTree_fp,
		 *input_encodedData_fp,
		 *output_fp;

	// get size of preorder stream
	input_encodedTree_fp = fopen(argv[1], "r");
	if (input_encodedTree_fp == NULL)
	{
		printf("** decode.c/main: File failed to open: %s\n", argv[1]);
		fclose (input_encodedTree_fp);
		exit(1);
	}

	char in_buff[MAX_WORD_LENGTH];
	fscanf(input_encodedTree_fp, "%s", in_buff);
	int size = binInt2decInt(atoi(in_buff));
	printf("** decode.c/main: in_fp, found size = %d = %d: size-14 = %d\n", size, int2binInt(size), size-14);
	size = size - 14;

	// get rid of newline char so we can read in char-by-char later.
	// From http://www.cplusplus.com/reference/cstdio/fscanf/:
	// Whitespace character: the function will read and ignore any whitespace characters
	// encountered before the next non-whitespace character (whitespace characters include spaces,
	// newline and tab characters -- see isspace). A single whitespace in the format string validates
	// any quantity of whitespace characters extracted from the stream (including none).
	fscanf(input_encodedTree_fp, "%c", in_buff);
	printf("** decode.c/main: reading newline from input file: %s\n", in_buff);

	// fill preorder stream from input file
	int pre_stream[size];
	int is_data = 0;
	int chunk_head,		// index of front of current chunk of the stream
		chunk_count;  	// since think of stream of 1s, 0s, and 8-bit chunks
	for(chunk_head=chunk_count=0; chunk_head < size; chunk_head++, chunk_count++) {
		// if encounter a data value
		if(is_data)
		{
			fscanf(input_encodedTree_fp, "%8s", in_buff);
			// convert 8-bit binary string to decimal int
			pre_stream[chunk_count] = binInt2decInt(atoi(in_buff));

			is_data = 0;
			chunk_head+=7;
		}
		else {
			fscanf(input_encodedTree_fp, "%1s", in_buff);  // using %c does not append terminating char; would need in_buff[1]='\0'

			// if encounter a 1, add to preorder stream as internal node
			if(strcmp(in_buff, "1") == 0)
				pre_stream[chunk_count] = atoi(in_buff);

			// if encounter a 0, remember that next item is an 8-bit data value
			if(strcmp(in_buff, "0") == 0)
			{
				pre_stream[chunk_count] = atoi(in_buff);
				is_data = 1;
			}
		}

		printf("** encode.c/main: filling stream: k=%d: pre_stream[%d]=%d\n", chunk_head, chunk_head, pre_stream[chunk_count]);
	}
	printf("** encode.c/main: size = %d = %d: k=%d\n", size, int2binInt(size), chunk_head);
	size = chunk_count;  // This works somehow, see folderpaper note attached to assignment

	fclose(input_encodedTree_fp);

    /* reconstruct Huffman tree from preorder stream */
    struct Node * root = reconstructPre(pre_stream, size);

    printf("** decode.c/main: Preorder traversal of reconstructed Huffman tree\n");
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

    printf("** decode.c/main: Filled codebook\n");
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


    /* Compress input data file using generated codebook */
    input_encodedData_fp = fopen(argv[2], "r");
    output_fp = fopen(argv[3], "w");
    input_encodedTree_fp = fopen(argv[1], "r");
    	if (input_encodedTree_fp == NULL || output_fp == NULL)
    	{
    		printf("** decode.c/main: Files failed to open\n");
    		fclose (input_encodedData_fp);
    		fclose(output_fp);
    		exit(1);
    	}

    // go thru each character (or string length-1) to build codeword to add actual to output file (from codebook)
    char codeword[MAX_WORD_LENGTH] = "\0";
    int n;
    while ((n=fgetc(input_encodedData_fp)) != EOF) {
    	// convert int to string rep.
    	char strInt[2];  // one space for bit, one space for null char
    	printf("** decode.c/main: reading in compressed bitstream: n=%d\n", n);
    	n = n - '0';  // should always be 0 or 1
    	sprintf(strInt, "%d", n);
    	printf("** decode.c/main: reading compressed data: bit=%s\n", strInt);

    	// build codeword from encoded bitstream
    	strcat(codeword, strInt);

    	// check if currently built codeword from input file has valid codebook entry
    	int i;
    	for(i=0; i < MAX_WORD_LENGTH; i++)
    	{
    		if(codebook[i].valid == 1 && strcmp(codeword, codebook[i].codeword) == 0)
    		{
    			printf("** decode.c/main: codeword match found codeword=%s, codebook[%d]=%s\n\n",
    					codeword, i, codebook[i].codeword);

    			// write char rep. of codebook key to output
    			char key[] = {i, '\0'};
    			fputs(key, output_fp);

    			// reset constructed codeword
    			strcpy(codeword, "\0");
    		}
    	}

	}

    fclose(input_encodedData_fp);
    fclose(output_fp);

    return 0;
}
