#pragma once

#include <string>
#include <vector>

// Report class, without OR-Tools dependency in its API
// Only dependency is in the implementation

class InfeasibleProblemReport {
 private:
  std::vector<std::pair<std::string, double>> mConstraints;
 public:
  void append(const std::string& constraintName,
              double value);
  void printLargest(int n);
};



