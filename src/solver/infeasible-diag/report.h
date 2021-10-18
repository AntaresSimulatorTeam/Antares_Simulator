#include <string>
#include <map>

class InfeasibleProblemReport {
 private:
  std::map<std::string, double> infeasibleConstraints;
 public:
  void append(const std::string& constraintName,
              double value) {
    // TODO check prior existence of constraintName
    infeasibleConstraints[constraintName] = value;
  }
};
