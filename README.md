# CPU Scheduling Algorithm

This repository contains an implementation of various CPU Scheduling algorithms. CPU scheduling is a critical aspect of operating systems, allowing efficient process management by determining the sequence in which processes are executed.

## Features

- **Implemented Algorithms:**
  - First-Come, First-Served (FCFS) (Non Pre-emptive)
  - Shortest Job Next (SJN) (Pre-emptive)
  - Round Robin (RR) (Pre-emptive)
- Easy-to-read code with detailed comments.
- Sample inputs and outputs to test the algorithms.

## Input Format
 - job_profiles.txt is the input file. Each line hold the record for each preocess.
 - First column is the process-id, second column is arrival time of the process.
 - Then variable number of columns for each process denote alternative cpu and io burst time.
 - "-1" indicates the end of the input for each process.
 - general input format : process-id   Arrival-Time   cpu-time   io-time  cpu-time   io-time  .....  cpu--time  -1.

## How to run ?
  - Make Sure You have a g++ compiler.
  - In terminal first run "g++ cpu-schedule-algo.c++".
  - Then "./a.exe".

## Output Format 
  - On running this program generate 4 files, compare.txt , FCFS.txt, SRTN.txt , RR.txt
  - compare.txt shows the average waiting time and turn-around time for each Prcess.
  - FCFS.txt , SRTN.txt, RR.txt shows the time and and in that time which process is going to executed by CPU. Output Format : Time | Process-id .
