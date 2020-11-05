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
  int ps1 = GetPeakHeapSize();
  free(v);
  StartPeakHeapSizeMeasurement();
  int ps2 = GetPeakHeapSize();
  std::cout << "ps1 " << ps1 << " - ps2 " << ps2 << std::endl;
  return 0;
}
