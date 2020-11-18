
all:
	gcc -g test_01.c kcsv.c -lm -o test_01
	gcc -g test_02.c kcsv.c -o test_02
	#gcc -g kompress.c kcsv.c -lm -o kompress
	gcc -g unkompress.c kcsv.c -o unkompress
	#gcc -g scsv.c -o scsv



