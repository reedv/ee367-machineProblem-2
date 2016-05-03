.DEFAULT_GOAL:=all

all: frequency367 createcode367 encode367 decode367

frequency367: frequency367.c
	gcc -o frequency367 frequency367.c
	
createcode367: createcode.c
	gcc -o createcode367 createcode.c

encode367: encode.c
	gcc -o encode367 encode.c
	
decode367: decode.c
	gcc -o decode367 decode.c