all: procA procB procC procD

procA: procA.c
	gcc -o procA procA.c queues.c

procB: procB.c
	gcc -o procB procB.c queues.c

procC: procC.c
	gcc -o procC procC.c queues.c

procD: procD.c
	gcc -o procD procD.c queues.c