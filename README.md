```
gcc -c -o obj/hellomake.o src/hellomake.c -Wall -I./include
gcc -c -o obj/hellofunc.o src/hellofunc.c -Wall -I./include
gcc -o hellomake obj/hellomake.o obj/hellofunc.o -Wall -I./include
```