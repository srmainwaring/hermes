//
// Created by frongere on 06/04/2021.
//

#include <iostream>
#include <hermes/version.h>

using namespace hermes::git;

int main() {

  std::cout << ProjectName() << " Version: " << GetNormalizedVersionString() << std::endl;
  std::cout << "Has Uncommitted changes: " << AnyUncommittedChanges() << std::endl;
  std::cout << "Commit: " << CommitSHA1() << std::endl;
  std::cout << "Date: " << CommitDate() << std::endl;
  std::cout << "Author: " << AuthorName() << std::endl;

  return 0;
}