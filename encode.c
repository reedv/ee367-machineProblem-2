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

// This constant can be avoided by explicitly calculating height of Huffman Tree
#define MAX_TREE_HT 100
#define INTERNAL_SENTINEL '$'


/* A tree node has data, pointer to left child
   and a pointer to right child */
struct node
{
    int data;
    struct node *left;
    struct node *right;
};

// A utility function to init. a node
struct node* newNode (int data)
{
    struct node* temp = (struct node *) malloc( sizeof(struct node) );

    temp->data = data;
    temp->left = temp->right = NULL;

    return temp;
}


// A recursive function to construct Full from pre[]. preIndex is used
// to keep track of index in pre[].
// FIXME: This copied code is just a starting pt. Needs to be checked and changed to build tree properly
struct node* constructTreeUtil (int pre[], int* preIndex,
                                int low, int high, int size)
{
    // Base case
    if (*preIndex >= size || low > high)
        return NULL;

    // The first node in preorder traversal is root. So take the node at
    // preIndex from pre[] and make it root, and increment preIndex
    struct node* root = newNode ( pre[*preIndex] );
    *preIndex = *preIndex + 1;

    // If the current subarry has only one element, no need to recur
    if (low == high)
        return root;

    // Search for the first element greater than root
    int i;
    for ( i = low; i <= high; ++i )
        if ( pre[i] > root->data )
            break;

    // Use the index of element found in preorder to divide preorder array in two parts;
    // Left subtree and right subtree
    root->left = constructTreeUtil ( pre, preIndex, *preIndex, i - 1, size );
    root->right = constructTreeUtil ( pre, preIndex, i, high, size );

    return root;

}

// The main function to construct tree from given preorder traversal.
// This function mainly uses constructTreeUtil()
struct node *constructTree (int pre[], int size)
{
    int preIndex = 0,
    	low = 0,
		high = size-1;
    return constructTreeUtil (pre, &preIndex, low, high, size);
}


/*void*/struct node * reconstructPreUtil(int pre[], int * preIndex, int size)
{
	// Base case
	if (*preIndex >= size)
	{
		printf("** encode.c/reconstructPreUtil: base reached\n");
		return NULL;
	}

	struct node * root = newNode(pre[*preIndex]);

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

		// return this internal node
		return root;
	}
	// if data is 0 (indicating a leaf)
	else if(root->data == 0)
	{
		// jump to next elem (the data for the leaf)
		*preIndex = *preIndex + 1;
		struct node * root = newNode(pre[*preIndex]);
		printf("** encode.c/reconstructPreUtil: preindex=%d, root->data = %d\n", *preIndex, root->data);
		root->left = root->right = NULL;
		*preIndex = *preIndex + 1;

		// return the leaf
		return root;
	}
}

struct node * reconstructPre(int pre[], int size)
{
	int preIndex = 0;
	return reconstructPreUtil(pre, &preIndex, size/*, root*/);
}

// A utility function to print inorder traversal of a Binary Tree
void printPreorder (struct node* node)
{

    if (node == NULL)
    {
    	printf("** encode.c/printPreorder: reached a leaf\n");
    	return;
    }

    printf("%d ", node->data);
    printPreorder(node->left);
    printPreorder(node->right);
}



// Driver program to test above functions
int main()
{
	//    10
	//   /   \
	//  5     40
	// /  \      \
	//1    7      50
    //int pre[] = {10, 5, 1, 7, 40, 50};
	int pre[] = {1, 1, 0, 32, 1, 0, 101, 1, 0, 84, 0, 104, 1, 0, 115, 1, 0, 105, 0, 116};
    int size = sizeof( pre ) / sizeof( pre[0] );
    printf("** encode.c/main: size=%d\n", size);

    // TODO:
    // read in 2dn field of encoded Huffman tree file to build preorder array, pre, of tree
    // the size of pre can be found using the 1st field of the encoding file: (its value) - 14


    //struct node * root = constructTree(pre, size);
    struct node * preroot = reconstructPre(pre, size);

    //printf("Preorder traversal of the constructed tree: \n");
    //printPreorder(root);

    //printf("\n");
    printf("Huffman code of huffman tree\n");
    printPreorder(preroot);

    return 0;
}








