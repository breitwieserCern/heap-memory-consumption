#include <cstdlib>
#include <iostream>
#include <vector>

extern "C" int StartPeakHeapSizeMeasurement();
extern "C" int GetPeakHeapSize();

int main() {
  StartPeakHeapSizeMeasurement();
  // std::vector<int> v;
  // v.resize(1000);j
  auto* v = (char*)malloc(1000);
  std::cout << GetPeakHeapSize() << std::endl;
  return 0;
}
