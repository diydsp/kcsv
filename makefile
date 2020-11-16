
all:
	gcc -g kompress.c kcsv.c -lm -o kompress
	gcc -g unkompress.c kcsv.c -o unkompress



