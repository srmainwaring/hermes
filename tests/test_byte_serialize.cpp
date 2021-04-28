//
// Created by frongere on 27/04/2021.
//

//#include <memory>
//#include <array>
#include <vector>
#include <cstddef>
#include <iostream>
#include <cstring>

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





/// Copying byte by byte the value into the buffer
template<typename T>
void push_back(std::byte *buffer, std::size_t &pos, const T &value) {
  memcpy(buffer + pos, &value, sizeof(T));

//  std::size_t k = pos;
//  for (; k < pos + sizeof(T); ++k) {
//    buffer[k] = ((std::byte *) (&value))[k];
//  }
//  pos = k;
  pos += sizeof(T);
}


int main() {

  int i = 1;
  double d = 3.14;

  // precompute buffer size
  const std::size_t size = sizeof(int) + sizeof(double);

  std::byte buffer[size];

//  std::cout << buffer << std::endl;

  // Fill the buffer with an int and a double
  std::size_t pos = 0;
  push_back(buffer, pos, i);

  std::cout << pos << std::endl;

  push_back(buffer, pos, d);

  std::cout << pos << std::endl;
  std::cout << std::endl;

  // Reading i
  int ir = 0;
//  int *pir = &ir;

  std::cout << ir << std::endl;

  std::size_t offset = 0;

  ir = *reinterpret_cast<int*>(buffer + offset);
  offset += sizeof(int);

//  std::cout << &buffer[0] << std::endl;
//  std::cout << *pir << std::endl;
//  std::cout << std::endl;

  // Reading d
  double dr = 0.;
//  double *pdr = &dr;

  std::cout << dr << std::endl;
  dr = *reinterpret_cast<double *>(buffer + offset);
  std::cout << &buffer[sizeof(int)] << std::endl;
  std::cout << dr << std::endl;


  return 0;
}