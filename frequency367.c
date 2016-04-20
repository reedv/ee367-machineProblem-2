/*
 * Input:  a data file
 * Output:  a file that is a list of ASCII values (as integers) and their 
 *    frequency as a fraction.  The first line in the file is
 *    the number of values.  The subsequent lines are the values (integer)
 *    and frequency (float or double).  The following is an example
 *    when there are three ACII values 20, 40, and 73
 *
 *    3
 *    20   0.5
 *    40   0.2
 *    73   0.3 
 *
 * The executable should be named "frequency367"
 *
 * To run the program:
 *
 * ./frequency367 <input data file>  <output file> 
 */

#include <stdlib.h>
#include <stdio.h>

typedef struct tableentry{
   int   count;
   float freq;
   } TableEntry;

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

int main(int argc, char *argv[])
{
	FILE *input_fp,
		 *output_fp;
	TableEntry Table[256];
	int i;
	int n;
	float totalcount;
	int numsymbols;

	totalcount = 0.0;
	numsymbols = 0;
	for (i=0; i<256; i++) Table[i].count = 0;  // Initialize table

	// Check if there are correct number of arguments
	if (argc < 3 || 3 < argc) {
	   printf("Usage:  frequency367 <input data file> <output list file>\n");
	   return 1;
	}

	// Read in codebook file
	input_fp = fopen(argv[1],"r");
	output_fp = fopen(argv[2],"w");

	// fill table with symbols from input file
	while ((n=fgetc(input_fp)) != EOF) {
	   // check that char from input file is valid acsii char
	   if (n>=0 && n<256) {
		  // check if this symbol already in table
		  if (Table[n].count == 0) numsymbols++;
		  Table[n].count++;

		  totalcount = totalcount + 1.0;
	   }
	   else printf("Error:  out of bounds character\n");
	}

	fprintf(output_fp,"%d\n",numsymbols);
	// go thru all ascii symbols
	for (i=0; i<256; i++) {
	   // if ascii symbol exists in table, add it (as int representation) and its freq. to output file
	   if (Table[i].count > 0) {
		  //char binstring_buffer[8];
		  //int2binStr(binstring_buffer, i);
		  fprintf(output_fp,"%d   %f\n", i, ( (float)Table[i].count)/ totalcount );
		  }
	}
	fclose(input_fp);
	fclose(output_fp);

	return 0;
}
