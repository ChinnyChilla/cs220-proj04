CC = gcc
CFLAGS = -ggdb -Wall -std=c18 -O0

test : grades solution.txt
	setarch linux64 -R ./grades <solution.txt
	
testASLR: grades solution.txt
	./grades <solution.txt
	
gdb : grades solution.txt
	gdb -x gdb_cmds.txt --args grades	
	
showhex : solution.txt
	od -Ax -tx1z solution.txt
	
hack.o : hack.s
	gcc -c -o hack.o hack.s
	
solution.txt : mix mix_in.txt hack.o
	./mix <mix_in.txt >solution.txt
	
grades.s : grades
	objdump -dSl grades >grades.s	
	
clean:  
	-rm grades grades.s mix solution.txt hack.o hack.objdump.txt
