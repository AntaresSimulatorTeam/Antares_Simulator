#pragma once

#include <string>
#include <vector>
#include <ostream>

class InfeasibleProblemReport {
 private:
  std::vector<std::pair<std::string, double>> mConstraints;
 public:
  void append(const std::string& constraintName,
              double value);
  void printLargest(int n);
};



