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

main(int argc, char *argv[])
{
	FILE *fp1, *fp2;
	TableEntry Table[256];
	int i;
	int n;
	float totalcount;
	int numsymbols;

	totalcount = 0.0;
	numsymbols = 0;
	for (i=0; i<256; i++) Table[i].count = 0;  // Initialize table

	// Check if there are two arguments
	if (argc != 3) {
	   printf("Usage:  frequency367 <input data file> <output list file>\n");
	   return;
	}

	// Read in codebook file
	fp1 = fopen(argv[1],"r");
	fp2 = fopen(argv[2],"w");

	//
	while ((n=fgetc(fp1)) != EOF) {
	   if (n>=0 && n<256) {
		  if (Table[n].count == 0) numsymbols++;
		  Table[n].count++;
		  totalcount = totalcount + 1.0;
		  }
	   else printf("Error:  out of bounds character\n");
	}

	fprintf(fp2,"%d\n",numsymbols);
	for (i=0; i<256; i++) {
	   if (Table[i].count > 0) {
		  fprintf(fp2,"%d  %f\n",i,((float) Table[i].count)/ totalcount);
		  }
	}
	fclose(fp1);
	fclose(fp2);
}
