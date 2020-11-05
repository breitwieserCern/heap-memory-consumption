#include <iostream>
#include <vector>
#include <cstdlib>
#include "mem-trace.h"

int main() {
   // std::vector<int> v;
   // v.resize(1000);j
   auto* v = (char*) malloc(1000); 
   std::cout << v[0] << " - " << GetPeakMemUsage() << std::endl;
   return 0;
}
