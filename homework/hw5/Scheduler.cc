#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class proc {
public:
  int pid, arrival_time, burst_time, priority, remaining_time;
  proc(int p, int a, int b, int pr) {
    pid = p;
    arrival_time = a;
    burst_time = b;
    priority = pr;
    remaining_time = b;
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
  for (int i = 0; i < nprocs; ++i) {
    proc &proc = procs_copy[i];
    int waiting_time = total_time - proc.arrival_time;
    int completion_time = total_time + proc.burst_time;
    int turnaround_time = completion_time - proc.arrival_time;
    total_time += proc.burst_time;
    total_wait += waiting_time;
    total_turnaround += turnaround_time;
  }
  printf("Average Turnaround Time: %.3f\n",
         total_turnaround / static_cast<float>(nprocs));
  printf("Average Wait Time: %.3f\n", total_wait / static_cast<float>(nprocs));
  printf("Throughput: %.3f\n", nprocs / static_cast<float>(total_time));
}

void sjfp(vector<proc> &procs) {
  printf("--- SJFP ---\n");
  size_t ncompleted = 0;
  int total_time = 0, total_turnaround = 0, total_wait = 0,
      nprocs = procs.size();
  vector<proc> procs_copy = procs;
  vector<bool> completed(procs_copy.size(), false);
  while (ncompleted < procs_copy.size()) {
    int ip = -1;
    int burst = INT_MAX;
    for (size_t i = 0; i < procs_copy.size(); i++) {
      if (procs_copy[i].arrival_time <= total_time && !completed[i]) {
        if (procs_copy[i].remaining_time < burst) {
          burst = procs_copy[i].remaining_time;
          ip = i;
        }
        if (procs_copy[i].remaining_time == burst)
          ip = i;
      }
    }
    total_time++;
    if (ip > -1) {
      procs_copy[ip].remaining_time--;
      if (procs_copy[ip].remaining_time == 0) {
        ncompleted++;
        completed[ip] = true;
        int completion_time = total_time;
        int turnaround_time = completion_time - procs_copy[ip].arrival_time;
        int waiting_time = turnaround_time - procs_copy[ip].burst_time;
        total_wait += waiting_time;
        total_turnaround += turnaround_time;
      }
    }
  }
  printf("Average Turnaround Time: %.3f\n",
         total_turnaround / static_cast<float>(nprocs));
  printf("Average Wait Time: %.3f\n", total_wait / static_cast<float>(nprocs));
  printf("Throughput: %.3f\n", nprocs / static_cast<float>(total_time));
}

void priority(vector<proc> &procs) {
  printf("--- Priority ---\n");
  size_t ncompleted = 0;
  int total_time = 0, total_turnaround = 0, total_wait = 0,
      nprocs = procs.size();
  vector<proc> procs_copy = procs;
  vector<bool> completed(procs.size(), false);
  while (ncompleted < procs.size()) {
    int ip = -1;
    int pr = INT_MAX;
    for (size_t i = 0; i < procs_copy.size(); i++) {
      if (procs_copy[i].arrival_time <= total_time && !completed[i]) {
        if (procs_copy[i].priority < pr) {
          pr = procs_copy[i].priority;
          ip = i;
        } else if (procs_copy[i].priority == pr) {
          if (procs_copy[i].arrival_time < procs_copy[ip].arrival_time)
            ip = i;
        }
      }
    }
    total_time++;
    if (ip > -1) {
      procs_copy[ip].remaining_time--;
      if (procs_copy[ip].remaining_time == 0) {
        ncompleted++;
        completed[ip] = true;
        int completion_time = total_time;
        int turnaround_time = completion_time - procs_copy[ip].arrival_time;
        int waiting_time = turnaround_time - procs_copy[ip].burst_time;
        total_wait += waiting_time;
        total_turnaround += turnaround_time;
      }
    }
  }
  printf("Average Turnaround Time: %.3f\n",
         total_turnaround / static_cast<float>(nprocs));
  printf("Average Wait Time: %.3f\n", total_wait / static_cast<float>(nprocs));
  printf("Throughput: %.3f\n", nprocs / static_cast<float>(total_time));
}

int main(int argc, char **argv) {
  vector<proc> procs = read_from_csv(argv[1]);
  fcfs(procs);
  sjfp(procs);
  priority(procs);
  return 0;
}
