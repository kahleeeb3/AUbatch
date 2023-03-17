# AUbatch- A Pthread‐based Batch Scheduling System
## User Interface
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
```