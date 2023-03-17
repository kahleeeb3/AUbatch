# AUbatch- A Pthread‐based Batch Scheduling System
Batch scheduling system using the C programming language and the Pthread library. AUbatch is comprised of two distinctive and collaborating threads: the scheduling thread, and the dispatching thread.
> The **scheduling thread** is in charge of:
1. accepting submitted jobs from users and  
2. enforcing a chosen scheduling policy.  
> The **dispatching thread** is in charge of:  
1. making use of the execv()function to run jobs sorted in the job queue
2. measuring the execution time and response time (a.k.a., turn‐around time) of each finished job

More
specifically, the scheduling module issues (produces) newly submitted jobs into the job queue, enforcing a scheduling policy; the dispatching module extracts (consumes) a job from the job queue to run the job.

## Thread Synchronization:
The threads library provides three synchronization mechanisms:

- **mutexes** - Mutual exclusion lock: Block access to variables by other threads. This enforces exclusive access by a thread to a variable or set of variables.
- **joins** - Make a thread wait till others are complete (terminated).
- **condition variables** - data type pthread_cond_t

Apart from critical sections, the synchronization of our two threads **must be implemented by condition variables.** In addition to condition variables, **mutex must be adopted** to solve the critical section problem in AUbatch since the two threads share a job queue.

## PThread Functions:
A list of important PThread functions to be employed in this project is given below:

**Thread Creation:**
- pthread_create(thread, attr, start_routine, arg);

**The mutex functions:**
- pthread_mutex_lock(mutex);
- pthread_mutex_unlock (mutex);

**The condition‐variable functions:**
- pthread_cond_init(condition, attr);
- pthread_cond_wait(condition, mutex);
- pthread_cond_signal(condition);

To learn more about the pthread library: [POSIX thread (pthread) libraries](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html)

<!-- ## Using AUbatch:
### 1. Help Information
```
$ ./aubatch 
Welcome to Caleb Powell's batch job scheduler Version 1.0
Type 'help' to find more about AUbatch commands.

>help

run <job> <time> <pri>: submit a job named <job>,
                        execution time is <time>,
                        priority is <pri>.
list: display the job status
fcfs: change the scheduling policy to FCFS.
sjf: change the scheduling policy to SJF.
priority: change the scheduling policy to priority.
test <benchmark> <policy> <num_of_jobs> <priority_levels>
     <min_CPU_time> <max_CPU_time>
quit: exit AUbatch

>
```

### 2. Submit a Job
```
$ ./aubatch 
Welcome to Caleb Powell's batch job scheduler Version 1.0
Type 'help' to find more about AUbatch commands.

>run sample_job 10

Job sample_job was submitted.
Total number of jobs in the queue: 4
Expected waiting time: 113 seconds
Scheduling Policy: FCFS.

>
```

### 3. Display the Job List
```
$ ./aubatch 
Welcome to Caleb Powell's batch job scheduler Version 1.0
Type 'help' to find more about AUbatch commands.

>list

Total number of jobs in the queue: 4
Scheduling Policy: FCFS.
Name CPU_Time Pri Arrival_time Progress
job1 59       3   07:12:13     Run
job2 127      1   07:12:35
job3 8        2   07:13:26

> 
```
### 4. Switch Scheduling Policy
```
$ ./aubatch 
Welcome to Caleb Powell's batch job scheduler Version 1.0
Type 'help' to find more about AUbatch commands.

>sjf

Scheduling policy is switched to SJF. All the 3 waiting jobs
have been rescheduled.

>fcfs

Scheduling policy is switched to FCFS. All the 2 waiting jobs
have been rescheduled.

> 
```
### 5. Quit AUbatch
```
$ ./aubatch 
Welcome to Caleb Powell's batch job scheduler Version 1.0
Type 'help' to find more about AUbatch commands.

>quit

Total number of job submitted: 5
Average turnaround time: 32.12 seconds
Average CPU time: 15.43 seconds
Average waiting time: 16.69 seconds
Throughput: 0.031 No./second

$ 
``` -->