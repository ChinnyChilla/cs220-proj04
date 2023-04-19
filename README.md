# Project 4 - Make your own grade

## Background

So far, we have had both a [lecture](https://www.cs.binghamton.edu/~tbartens/CS220_Spring_2023/Lectures/L15_Buffer_Overflow_Attack.pdf) and we have done a lab that covered the basics of a buffer overflow attack.  In Project 4, we will complete the process, generating a complete buffer overflow attack, including inserting your own "nefarious" code to modify the results of a specific program.

The techniques you will use for this attack will *not* work outside our specialized project environment.  I have designed the code in this project to be particularly vulnerable to a buffer overflow attack.  Besides, as good citizens, we would never try to use these methods for evil purposes.

## Files Provided

The proj4 repository comes with the following files:

- **grades.c** : This is a C program that randomly generates grades for projects, homework, and tests for all students in the class, then reads a single line from standard input (using the gets C library function) and looks at the first character to determine what section you are in (A or B).  The program then prints out the grades for each student in that section, along with a weighted grade average.

  The random generation is designed to give all students a grade in either the low 60's or high 50's. If you modify this file, save a copy of the original because your solution.txt file will be run using the original unmodified code. Modify the file in order to get some ideas on how to hack some X86 assembler code to modify your grade.

- **mix.c** : This is the C program we used in lab11 to mix ASCII and binary data and write that data to standard output. We've added one more enhancement... the ability to read binary data from a file and include it in the output.

- **mix_in.txt** : The textual input to the `mix` executable. As delivered, this will put a single character, an "A" into the output file.

- **gdb_cmds.txt** : A file to hold gdb commands that should be run after grades is loaded when you run "make gdb".

- **hack.s** : A file to hold your nefarious X86 assembler code that will change your grade to 100, patch up anything broken by the buffer overflow attack, and rejoin the normal program control flow.

  You will need to figure out what X86 assembler code to put into this file. The Makefile includes targets to run the X86 assembler on this program and produce a binary object code file, **hack.o**, from the file.

  You need to figure out how to get the binary object code into memory, and how to branch to the program using a buffer overflow attack.

- **Makefile** : A make command Makefile which has several targets and associated rules, as follows:

  - **test** : This runs the grades executable with a ASLR turned off, and redirects the **solution.txt** file to standard input. It has dependencies on both the grade executable and the solution.txt file, and will rebuild either if required.
  
  - **testASLR** : This runs with grades executable without turning off ASLR, but still redirects the **solution.txt** file to standard input. This is provided to demonstrate that your solution probably will not work with ASLR turned on.
  
  - **gdb** : This runs gdb on the grades executable, and runs the commands from the file **gdb_cmds.txt** once the grades executable is loaded. This has dependencies on the grade executable and the solution.txt file, and will rebuild either if required.
  
  - **showhex** : This runs the od command on the solution.txt file to show both hex and ASCII values in that file.
  
  - **hack.o** : This runs the x86 assembler under the covers of gcc to assemble hack.s and produce the binary hack.o object code file. Note that hack.o is an ELF file.
  
  - **solution.txt** : This runs the mix executable, redirecting file **mix_in.txt** to standard input, and redirecting standard output to **solution.txt**. Since solution.txt is a dependency for other commands, this will run automatically if mix_in.txt changes.
  
  - **grades.s** : This runs objdump on the grades executable, and saves the result in file grades.s.
  
  - **grades** : This builds the grades executable from grades.c.
  
  - **mix** : This builds the mix executable from mix.c.
  
  - **clean** : This removes all the intermediate files created by the Makefile.

## Specifications

The grades program does the following:

  1. Sets up a special 1024 byte area to hold the line read from standard input called `retBuffer`. Notice the call to the C library function called `mprotect`. You may want to investigate what mprotect does, and think about why I have included an invocation of that function in this code.

  2. If there is a command line argument, uses that command line argument as a random seed. If not, use the current time as a random seed. This allows me to specify a random seed and get the same results from the C library random number generator each time, *reproducible* random values. (I use this to check to make sure you have not modified other peoples grades.)

  3. Generates random grades for all students in the A and B sections using the `getGrades` function.

  4. Generates weighted averages for each student in the A and B sections using the `getAvgs` function.

  5. Prompts for an input, and reads input from standard input using the getUserLine function, which will copy the text read into `retBuffer`, and returns a pointer to retBuffer, which is saved in variable `secID`.

  6. Formats and prints the grades for the A section if the first character of `secID` is 'A', and for the B section if the first character of `secID` is 'B'. Otherwise, prints an error message.

  7. Runs the C `exit` routine with a zero return code to indicate the grades program worked. Note - I could have used a return instruction instead, but sometimes the exit routine is not as picky about the environment (e.g. stack frames).

You need to provide an input file in **solution.txt** that includes the section letter.  Your grade for this project will be the result of applying your input file to my copy of the grades binary file with a random seed of my choice. You will receive the grade printed out by the program under your name.

## Implementation
  
Your job is to come up with an input file which not only specifies the section, but which also alters the execution of the grades executable in such a way as to modify your individual grades, as well as your average grade, without altering anything else about the results of the program.

In order to do that, you will need to generate your own object code, figure out how to get that object code into the input file in such a way that it ends up in memory that allows your code to execute, and then figure out how to modify the return address to branch to your new object code.  Your object code will need to repair/manage the stack frame that may have been corrupted by your buffer overflow attack, modify your grades and average grade, and then return to normal program execution.

## Hints

  1. The Makefile test target is specifically designed to avoid randomized Loading that is performed by default in Unix. If you do not use the setarch prefix used by the Makefile, the program will be loaded at some random location, and make your buffer overflow attack virtually impossible.  If you **do** use the setarch prefix, then when you run the grades program, it will be loaded at the exact same place it is loaded when you run gdb (which overrides random loading as well.) When I grade your program, I will use the setarch prefix as in the Makefile.
  
  2. You might want to create your own x86 code to perform various functions. You may write your code in C, and then run gcc to produce X86 for you, or, you might just want to cut/paste and modify code you have seen before (for instance the code in grades.s). It's easiest to put this x86 code in a file called "hack.s", and then used the gcc compiler to translate that to binary code using `make hack.o`.  You can then copy the hex object code to your input file. You will need to find the offset of the object code in the hack.o file by running "objdump -f hack.o" and looking up the offset and length of the .text section.
  
  3. To generate a solution.txt file that has a combination of both text and binary data, you may want to use a program, such as the "mix" provided for you. The mix program does what is described in the buffer overflow lecture, but has an added feature to read from an input file and write the results to the output. You can use this to read the object code from your hack.o file and write it into solution.txt.
  
  4. Don't forget that the gets function reads until it either finds a newline (0x10) or and end-of-file character.  That means that if you insert code that contains a 0x10 value, gets will stop reading when it runs into that character! If it doesn't seem like gets is reading everything you have specified, check to make sure you haven't inadvertently put a new line in the middle of the text in your solution.txt file.
  
  5. It is often nice to be able to see both the binary and the ASCII values of your solution.txt file. You can use the `showhex` target in the Makefile to show both the binary and ASCII values of your solution.txt file.  (This uses "od", the UNIX "octal dump" command, although it clearly does more than display just octal values.)
  
  6. It is possible to get gdb to redirect standard input for your program to read from a file by using the syntax "run <solution.txt" when you specify your run command.  This makes it possible to run gdb and test your results. This is used in `make gdb` because the `run <solution.txt` command is included in the file gdb_cmds.txt.
  
  7. I have gone to great lengths to provide an area that you can execute instructions from in the grades.c code.  Read through the code, and check to see what the library functions that I use in that code do to figure out how you can hack the attack and make it work. However, there is limited space in this area. It is a challenge, but it is possible to make x86 code that fits in the space provided that does everything it needs to do.
  
  8. There are three ways update your average grade. One is just to manually calculate your average and use your evil code to write that value into the grades. The second is to include the code to calculate your average in the evil code, but that may take a lot of instructions. The third is to re-invoke the getAvgs function from your evil code to update the entire section's averages. This is the most difficult, but most elegant way to update the average.
  
## Submitting Project 4

When you are finished creating your buffer overflow attack, add "solution.txt" to your repository, and commit and push your repository (which should contain at least the solution.txt file) using git. Then, paste the hash code associated with the commit into the Project 4 submission area in Brightspace. You may submit as many times as you wish up to the deadline, but only the last submission will be graded.

## Grading Project 4

The random grades generated by the grades executable are designed to generate an OK, but not very good grade (between the 55 and 65), so if you submit an input file that simply contains a valid section ID, you are guaranteed of getting some random grade that might not be too bad.  However, the point of this project is to provide an input file which alters your grade, and guarantees that you get a very good grade.

Note that the random number generator used to generate the grades is normally seeded with the time date stamp, so grades generates a different grade for you every time it runs. However, when I run your code, I will supply a random seed argument so that I can compare the output with your solution.txt file against the output produced with an input file that contains only your section letter.

We will run the original grades program (so changing grades.c in your repository won't help), using your **solution.txt** file as input, and comparing the results to a similar run (with the same random seed) using an input that contains only your section letter. Your grade starts out as the value printed by the grades program with your solution.txt file as input, under your name. We will then make some checks on your results.  If these checks fail, we will know that you have been hacking, and will subtract points from your project grade.  The checks we will be making are as follows:

| Check	| Score Impact |
|----------------------------------------------------------------|---------------------|
| If any grade other than your own has been modified| -20 points |
| If any of the grades in your row are less than zero, or greater than the maximum grade. |-20 points |
| If your average grade is calculated incorrectly, based on the algorithm in the getAvgs function in grades.c |-20 points |
| If Output lines (e.g. student grades) are missing compared to normal run | -20 points |
| If extra messages (e.g. segmentation violation) appear | -20 points |
| Submission error (e.g. file named incorrectly)  | -10 points |
| First to report a valid bug in the code supplied in the proj4 repository | +5 points |

Note that no late submissions or extensions will be accepted for this project. If you have not submitted by midnight of the deadline, then you will get a zero for the project.