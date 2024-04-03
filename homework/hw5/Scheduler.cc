#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class CSVRow {
public:
  std::string_view operator[](std::size_t index) const {
    return std::string_view(&m_line[m_data[index] + 1],
                            m_data[index + 1] - (m_data[index] + 1));
  }
  std::size_t size() const { return m_data.size() - 1; }
  void readNextRow(std::istream &str) {
    std::getline(str, m_line);

    m_data.clear();
    m_data.emplace_back(-1);
    std::string::size_type pos = 0;
    while ((pos = m_line.find(',', pos)) != std::string::npos) {
      m_data.emplace_back(pos);
      ++pos;
    }
    // check for trailing commas
    pos = m_line.size();
    m_data.emplace_back(pos);
  }

private:
  std::string m_line;
  std::vector<int> m_data;
};

std::istream &operator>>(std::istream &str, CSVRow &data) {
  data.readNextRow(str);
  return str;
}

class CSVIterator {
public:
  typedef std::input_iterator_tag iterator_category;
  typedef CSVRow value_type;
  typedef std::size_t difference_type;
  typedef CSVRow *pointer;
  typedef CSVRow &reference;

  CSVIterator(std::istream &str) : m_str(str.good() ? &str : nullptr) {
    ++(*this);
  }
  CSVIterator() : m_str(nullptr) {}

  // pre-increment
  CSVIterator &operator++() {
    if (m_str) {
      if (!((*m_str) >> m_row)) {
        m_str = nullptr;
      }
    }
    return *this;
  }
  // post-increment
  CSVIterator operator++(int) {
    CSVIterator tmp(*this);
    ++(*this);
    return tmp;
  }
  CSVRow const &operator*() const { return m_row; }
  CSVRow const *operator->() const { return &m_row; }

  bool operator==(CSVIterator const &rhs) {
    return ((this == &rhs) || ((this->m_str == nullptr)));
  }
  bool operator!=(CSVIterator const &rhs) { return !((*this) == rhs); }

private:
  std::istream *m_str;
  CSVRow m_row;
};

int main(int argc, char **argv) {
  std::ifstream file(argv[1]);
  // NOTE: format for value in line looks like this: (*loop)[i]
  // i = 0: pid
  // i = 1: arrival time
  // i = 2: burst duration
  // i = 3: priority

  // fcfs (non-preemptive)
  std::cout << "--- FCFS ---";
  int total_time = 0, total_turnaround = 0, total_wait = 0;
  for (CSVIterator loop(file); loop != CSVIterator(); ++loop) {
    // get arrival time, which when parsed, shows up in position 1
    int arrivalTime = atoi((*loop)[1]);
    // add proc time burst to total time
    total_time += atoi((*loop)[2]);
  }
  // sjfp
  std::cout << "--- SJFP ---";
  for (CSVIterator loop(file); loop != CSVIterator(); ++loop) {
  }
  // priority (preemptive)
  std::cout << "--- Priority ---";
  for (CSVIterator loop(file); loop != CSVIterator(); ++loop) {
  }
  return 0;
}
