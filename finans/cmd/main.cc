#include <iostream>

#include "finans/core/finans.h"

void main() {
  auto finans = Finans::CreateNew();
  if (finans == nullptr) {
    std::cout << "Finans is not installed\n";
  }
  std::cout << "Hello world\n";
}
