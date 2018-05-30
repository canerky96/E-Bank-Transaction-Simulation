rm -f main.o
gcc main.c -o main.o -lpthread > /dev/null 2>&1
./main.o | tee log.txt
cat log.txt | grep "C_ID" | wc -l
cat log.txt | grep "A_ID" | wc -l
