#include <algorithm>
#include <cstddef>
using namespace std;
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class proc {
public:
  int pid, arrival_time, burst_time, priority, remaining_time;
  proc(int pid, int arrival_time, int burst_time, int priority) {
    this->pid = pid;
    this->arrival_time = arrival_time;
    this->burst_time = burst_time;
    this->priority = priority;
    this->remaining_time = burst_time;
  }
};

vector<proc> read_from_csv(char *filename) {
  vector<proc> procs;
  fstream file(filename);
  string line;
  while (getline(file, line)) {
    stringstream sstream(line);
    string token;
    vector<int> values;
    while (getline(sstream, token, ',')) {
      values.push_back(stoi(token));
    }
    int pid = values[0];
    int arrival_time = values[1];
    int burst_time = values[2];
    int priority = values[3];

    proc p(pid, arrival_time, burst_time, priority);
    procs.emplace_back(p);
  }
  return procs;
}

void fcfs(vector<proc> &procs) {
  printf("--- FCFS ---\n");
  vector<proc> procs_copy = procs;
  int total_time = 0, total_turnaround = 0, total_wait = 0,
      nprocs = procs_copy.size();
  sort(procs_copy.begin(), procs_copy.end(), [](const proc &a, const proc &b) {
    if (a.arrival_time == b.arrival_time)
      return a.pid < b.pid;
    return a.arrival_time < b.arrival_time;
  });
  for (size_t i = 0; i < nprocs; ++i) {
    proc &proc = procs_copy[i];
    int waiting_time = total_time - proc.arrival_time;
    int completion_time = total_time + proc.burst_time;
    int turnaround_time = completion_time - proc.arrival_time;
    total_time += proc.burst_time;
    total_wait += waiting_time;
    total_turnaround += turnaround_time;
  }
  printf("Average Turnaround Time: %.3f\n", total_turnaround / (float)nprocs);
  printf("Average Wait Time: %.3f\n", total_wait / (float)nprocs);
  printf("Throughput: %.3f\n", nprocs / (float)total_time);
}

void sjfp(vector<proc> &procs) {
  printf("--- SJFP ---\n");
  int total_time = 0, total_turnaround = 0, total_wait = 0, ncompleted = 0,
      nprocs = procs.size();
  vector<bool> completed(procs.size(), false);
  while (ncompleted != procs.size()) {
    int ip = -1;
    int burst = 1000;
    for (int i = 0; i < procs.size(); i++) {
      if (procs[i].arrival_time <= total_time && !completed[i]) {
        if (procs[i].remaining_time < burst) {
          burst = procs[i].remaining_time;
          ip = i;
        }
        if (procs[i].remaining_time == burst) {
          ip = i;
        }
      }
    }
    total_time++;
    if (ip != -1) {
      procs[ip].remaining_time--;
      if (procs[ip].remaining_time == 0) {
        ncompleted++;
        completed[ip] = true;
        int completion_time = total_time;
        int turnaround_time = completion_time - procs[ip].arrival_time;
        int waiting_time = turnaround_time - procs[ip].burst_time;
        total_wait += waiting_time;
        total_turnaround += turnaround_time;
      }
    }
  }
  printf("Average Turnaround Time: %.3f\n", total_turnaround / (float)nprocs);
  printf("Average Wait Time: %.3f\n", total_wait / (float)nprocs);
  printf("Throughput: %.3f\n", nprocs / (float)total_time);
}

void priority(vector<proc> &procs) {
  printf("--- Priority ---\n");
  sort(procs.begin(), procs.end(), [](proc &a, proc &b) {
    if (a.priority == b.priority)
      return a.pid < b.pid;
    return a.priority < b.priority;
  });
  int total_time = 0, total_turnaround = 0, total_wait = 0,
      num_procs = procs.size();
  for (size_t i = 0; i < num_procs; ++i) {
    proc &proc = procs[i];
    int waiting_time = total_time - proc.arrival_time;
    int completion_time = total_time + proc.burst_time;
    int turnaround_time = completion_time - proc.arrival_time;
    total_time += proc.burst_time;
    total_wait += waiting_time;
    total_turnaround += turnaround_time;
  }
  printf("Average Turnaround Time: %.3f\n",
         total_turnaround / (float)num_procs);
  printf("Average Wait Time: %.3f\n", total_wait / (float)num_procs);
  printf("Throughput: %.3f\n", num_procs / (float)total_time);
}

int main(int argc, char **argv) {
  vector<proc> procs = read_from_csv(argv[1]);
  fcfs(procs);
  sjfp(procs);
  priority(procs);
  return 0;
}
