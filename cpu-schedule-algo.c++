#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <deque>

using namespace std;

struct Job {
    int id;
    int arrival_time;
    vector<int> cpu_io_sequence;  // Alternating CPU and I/O bursts
    int total_cpu_time;           // Sum of all CPU bursts
    int current_burst_index;
    int remaining_cpu_time;
    int waiting_time;
    int turnaround_time;
    
    Job(int _id, int _arrival_time, const vector<int>& bursts)
        : id(_id), arrival_time(_arrival_time), cpu_io_sequence(bursts),
          current_burst_index(0), waiting_time(0), turnaround_time(0) {
        total_cpu_time = 0;
        for (int i = 0; i < bursts.size(); i += 2) {  // Sum all CPU bursts (ignoring I/O bursts)
            total_cpu_time += bursts[i];
        }
        remaining_cpu_time = total_cpu_time;
    }
};

//custom comparators for priority queue prioritized based on arrival time if they are same prioritize based on remaining cpu time
class sort_arrival{
    public:
       bool operator()(Job a, Job b){
           if(a.arrival_time>b.arrival_time ||(a.arrival_time==b.arrival_time && a.remaining_cpu_time>b.remaining_cpu_time)){
               return true;
           }
           return false;
      }
};

//custom comparators for priority queue prioritized based on remaining cpu time if they are same prioritize based on arrival time
class sort_remaining_time{
    public:
       bool operator()(Job a, Job b){
           if(a.remaining_cpu_time>b.remaining_cpu_time || (a.remaining_cpu_time==b.remaining_cpu_time && a.arrival_time>b.arrival_time)){
               return true;
           }
           return false;
      }
};

// Function to parse the job profiles from a file
vector<Job> parse_jobs(const string& filename) {
    ifstream infile(filename);
    vector<Job> jobs;
    
    int job_id, arrival_time;
    while (infile >> job_id >> arrival_time) {
        vector<int> bursts;
        int burst;
        while (infile >> burst && burst != -1) {
            bursts.push_back(burst);
        }
        Job job(job_id, arrival_time, bursts);
        jobs.push_back(job);
    }
    
    return jobs;
}

// First Come First Serve (FCFS) Scheduler
pair<double, double> fcfs_scheduler(vector<Job> jobs) {

    int time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int n = jobs.size();

    //tat -> turnaround time, ct->completion time, wt->waiting time
    vector<int>tat(n),ct(n),wt(n);
    for(int i=0;i<n;i++) tat[i]-=jobs[i].arrival_time;

    priority_queue<Job, vector<Job>, sort_arrival> ready_queue;
    for (auto job : jobs) {
       ready_queue.push(job);
    }
    ofstream outfile("FCFS.txt");
    outfile<<"Arrival Time | JOB ID"<<endl;

    while(!ready_queue.empty()){
        Job curr_job = ready_queue.top();
        outfile<<time<<"  | "<<curr_job.id<<endl;
        ready_queue.pop();
        int burst_size= curr_job.cpu_io_sequence.size();
        if(time<curr_job.arrival_time) time = curr_job.arrival_time;
        time+= curr_job.cpu_io_sequence[curr_job.current_burst_index];
        if(curr_job.current_burst_index+2>=burst_size){
            ct[curr_job.id-1]=time;
        }else{
            curr_job.current_burst_index+=2;
            curr_job.arrival_time= time+curr_job.cpu_io_sequence[curr_job.current_burst_index-1];
            ready_queue.push(curr_job);
        }
    }

    for(int i=0;i<n;i++){
        tat[i]+=ct[i];
       total_turnaround_time+=tat[i];
        wt[i]=tat[i]-jobs[i].total_cpu_time;
        total_waiting_time+=wt[i];
    }

    double avg_waiting_time = (double)total_waiting_time / n;
    double avg_turnaround_time = (double)total_turnaround_time / n;
    return {avg_waiting_time, avg_turnaround_time};
}

// Shortest Remaining Time Next (SRTN) Scheduler
pair<double, double> srtn_scheduler(vector<Job> jobs) {
    int time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int n = jobs.size();

    //tat -> turnaround time, ct->completion time, wt->waiting time
    vector<int>tat(n),ct(n),wt(n);
    for(int i=0;i<n;i++) tat[i]-=jobs[i].arrival_time;

    // wait_queue store the jobs that have not arrived yet or perfoming io operation
    priority_queue<Job, vector<Job>, sort_arrival> wait_queue;
    priority_queue<Job, vector<Job>,sort_remaining_time> ready_queue;
     for (auto job : jobs) {
       wait_queue.push(job);
    }

    ofstream outfile("SRTN.txt");
    outfile<<"Arrival Time | JOB ID"<<endl;

   
    while(!wait_queue.empty() || !ready_queue.empty()){

        while(!wait_queue.empty() && wait_queue.top().arrival_time<=time){
            ready_queue.push(wait_queue.top());
            wait_queue.pop();
        }

        if(ready_queue.empty()){
            ready_queue.push(wait_queue.top());
            time=wait_queue.top().arrival_time;
            wait_queue.pop();
        }

        Job curr_job = ready_queue.top();
        outfile<<time<<" | "<<curr_job.id<<endl;
        ready_queue.pop();

        int burst_size= curr_job.cpu_io_sequence.size();
        int add_time=min(curr_job.cpu_io_sequence[curr_job.current_burst_index],1);
        time+= add_time;
        curr_job.cpu_io_sequence[curr_job.current_burst_index]-=add_time;
        int next_idx=curr_job.current_burst_index;
        int io_time=0;
        if(curr_job.cpu_io_sequence[curr_job.current_burst_index]==0){
            next_idx+=2;
            if(next_idx-1<burst_size)
            io_time+=curr_job.cpu_io_sequence[next_idx-1];
        }
        if(next_idx>=burst_size){
            ct[curr_job.id-1]=time;
        }else{
            curr_job.current_burst_index=next_idx;
            curr_job.arrival_time= time+io_time;
            wait_queue.push(curr_job);
        }
    }

    for(int i=0;i<n;i++){
        tat[i]+=ct[i];
       total_turnaround_time+=tat[i];
        wt[i]=tat[i]-jobs[i].total_cpu_time;
        total_waiting_time+=wt[i];
    }

    double avg_waiting_time = (double)total_waiting_time / n;
    double avg_turnaround_time = (double)total_turnaround_time / n;
    return {avg_waiting_time, avg_turnaround_time};
}

// Round Robin (RR) Scheduler
pair<double, double> rr_scheduler(vector<Job> jobs, int time_slice) {
    int time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int n = jobs.size();

    ofstream outfile("RR.txt");
    outfile<<"Arrival Time | JOB ID"<<endl;

    //tat -> turnaround time, ct->completion time, wt->waiting time
    vector<int>tat(n),ct(n),wt(n);
    for(int i=0;i<n;i++) tat[i]-=jobs[i].arrival_time;

     // wait_queue store the jobs that have not arrived yet or perfoming io operation
    priority_queue<Job, vector<Job>,sort_arrival > wait_queue;
    priority_queue<Job, vector<Job>,sort_arrival> ready_queue;
    for (auto job : jobs) {
       wait_queue.push(job);
    }

    while(!wait_queue.empty() || !ready_queue.empty()){
        while(!wait_queue.empty() && wait_queue.top().arrival_time<=time){
            ready_queue.push(wait_queue.top());
            wait_queue.pop();
        }
        if(ready_queue.empty()){
            ready_queue.push(wait_queue.top());
            time=wait_queue.top().arrival_time;
            wait_queue.pop();
        }

        Job curr_job = ready_queue.top();
        outfile<<time<<" | "<<curr_job.id<<endl;
       
        ready_queue.pop();
        int burst_size= curr_job.cpu_io_sequence.size();
        int add_time=min(curr_job.cpu_io_sequence[curr_job.current_burst_index],time_slice);
        time+= add_time;
        curr_job.cpu_io_sequence[curr_job.current_burst_index]-=add_time;
        int next_idx=curr_job.current_burst_index;
        int io_time=0;
        if(curr_job.cpu_io_sequence[curr_job.current_burst_index]==0){
            next_idx+=2;
            if(next_idx-1<burst_size)
            io_time+=curr_job.cpu_io_sequence[next_idx-1];
        }
        if(next_idx>=burst_size){
            ct[curr_job.id-1]=time;
        }else{
            curr_job.current_burst_index=next_idx;
            curr_job.arrival_time= time+io_time;
            wait_queue.push(curr_job);
        }
    }

    for(int i=0;i<n;i++){
        tat[i]+=ct[i];
       total_turnaround_time+=tat[i];
        wt[i]=tat[i]-jobs[i].total_cpu_time;
        total_waiting_time+=wt[i];
    }

    double avg_waiting_time = (double)total_waiting_time / n;
    double avg_turnaround_time = (double)total_turnaround_time / n;
    return {avg_waiting_time, avg_turnaround_time};
}

// Function to compare the results of the schedulers
void compare_schedulers(const vector<Job>& jobs) {
    auto fcfs_result = fcfs_scheduler(jobs);
    auto srtn_result = srtn_scheduler(jobs);
    auto rr_result = rr_scheduler(jobs, 25);

    ofstream outfile("compare.txt");

    outfile << "FCFS Average Waiting Time: " << fcfs_result.first << ", Turnaround Time: " << fcfs_result.second << endl;
    outfile << "SRTN Average Waiting Time: " << srtn_result.first << ", Turnaround Time: " << srtn_result.second << endl;
    outfile << "RR Average Waiting Time: " << rr_result.first << ", Turnaround Time: " << rr_result.second << endl;
}

int main() {
    
    string filename = "job_profiles.txt";  
    vector<Job> jobs = parse_jobs(filename);
    compare_schedulers(jobs);

    return 0;
}
