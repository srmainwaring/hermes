//
// Created by frongere on 27/04/2021.
//

//#include <memory>
//#include <array>
#include <vector>
#include <cstddef>
#include <iostream>

#include "H5PacketTable.h"


/*
 * https://stackoverflow.com/questions/13302407/correct-way-to-serialize-binary-data-in-c
 * https://arxiv.org/pdf/1811.04556.pdf
 * https://github.com/jl2922/hps
 * https://gieseanw.wordpress.com/2017/05/03/a-true-heterogeneous-container-in-c/
 * https://www.codeproject.com/Articles/1113332/Streaming-with-Cplusplus-is-brilliant
 *
 * https://github.com/Megaxela/ByteArray/blob/master/include/ba/bytearray_processor.hpp
 *
 */

template <typename T>
void push_back(std::vector<std::byte> &buffer, const T& value) {
  for (std::size_t k = 0; k < sizeof(int); ++k) {
    buffer.push_back(((std::byte*)(&value))[k]);
  }
}


int main() {

  int i = 1;
  double d = 3.14;

  // precompute buffer size
  std::size_t size = sizeof(int) + sizeof(double);

  std::vector<std::byte> buffer;
  buffer.reserve(size);

  push_back(buffer, i);
  push_back(buffer, d);

//  for (std::size_t k = 0; k < sizeof(int); ++k) {
//    buffer.push_back(((std::byte*)(&i))[k]);
//  }
//
//  for (std::size_t k = 0; k < sizeof(double); ++k) {
//    buffer.push_back(((std::byte*)(&d))[k]);
//  }

//  std::cout << buffer.size() << std::endl;



  return 0;
}