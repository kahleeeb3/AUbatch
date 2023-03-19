
/* 
 * COMP7500/7506
 * Project 3: process.c 
 *
 * This program will be launched in the run_cmd() function of scheduler.c.
 * The main functionality of this code is to sleep for a specified quantity
 * of time specified by the user input.
 * EX: "./process 5" will execute a process that sleeps for 5 seconds.
 *
 * Compilation Instruction: 
 * gcc process.c -o process
 * 
 * Reference: 
 * http://www.cs.ecu.edu/karl/4630/spr01/example1.html
 * Modified by Caleb Powell
 * Department of Computer Science and Software Engineering
 * Auburn University
 * Mar 19, 2023.
 */
#include <stdio.h> 

int main(int argc, char *argv[] )
{
  int sleepTime = atoi(argv[1]);
  // printf("this process will sleep for %d secs.\n",sleepTime);
  sleep(sleepTime);
  // printf("Thread2: Finished Executing Job.\n");

  return 0;
}
