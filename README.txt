Name: Reed Villanueva
Date: May 2, 2016

Instructions:

	Program Usages:
		To create frequency table for a data file:
			./frequency367 <input data file> <output frequency file>

		To create encoded Huffman tree for frequency table:
			./createcode367 <input frequency file> <output encoded tree file>
		
		To create compressed data file:
			./encode367 <input encoded tree file> <input data file> <output compressed data file>

		To decompress the compressed data file:
			./decode367 <input encoded tree file> <input compressed data file> <output decompressed data file>