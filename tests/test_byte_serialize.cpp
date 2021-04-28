//
// Created by frongere on 27/04/2021.
//

#include <utility>
//#include <array>
#include <vector>
#include <cassert>
//#include <>
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


class ByteArray {

 public:
  ByteArray() : m_bytes(nullptr), m_buffer_size(0), m_offset(0) {}

  ~ByteArray() {
    free(m_bytes);
  }

  void reserve(const size_t &n) {
    m_buffer_size = n;
    m_bytes = static_cast<std::byte *>(malloc(n));
  }

  std::byte* get() {
    return m_bytes;
  }

  template <typename T>
  ByteArray& operator<<(T&val) {
    std::size_t elt_s = sizeof(T);
    memcpy(m_bytes + m_offset, &val, elt_s);
    m_elements.emplace_back(std::pair{m_offset, elt_s});
    m_offset += elt_s;
    return *this;
  }

  template <typename T>
  T& as(std::size_t i) const {
    auto elt = m_elements[i];
    std::size_t offset = elt.first;
    assert(elt.second == sizeof(T));
    return *reinterpret_cast<T *>(m_bytes + offset);
  }

 private:
  std::byte *m_bytes;
  std::size_t m_buffer_size;
  std::size_t m_offset;
  std::vector<std::pair<std::size_t, std::size_t>> m_elements;
};


/// Copying byte by byte the value into the buffer
template<typename T>
void push_back(std::byte *buffer, size_t &pos, const T &value) {
  memcpy(buffer + pos, &value, sizeof(T));
  pos += sizeof(T);
}


int main() {

  int i = 1;
  double d = 3.14;

  // precompute buffer size
  const std::size_t size = sizeof(int) + sizeof(double);

  std::byte buffer[size];

  // Fill the buffer with an int and a double
  std::size_t pos = 0;
  push_back(buffer, pos, i);
  push_back(buffer, pos, d);


  std::size_t offset = 0;

  // Reading i
  int ir = *reinterpret_cast<int *>(buffer + offset);
  offset += sizeof(int);
//  std::cout << ir << std::endl;

  // Reading d
  double dr = *reinterpret_cast<double *>(buffer + offset);
  offset += sizeof(double);
//  std::cout << dr << std::endl;





  ByteArray buff;
  buff.reserve(size);

  buff << i << d;

  std::cout << buff.as<int>(0) << std::endl;
  std::cout << buff.as<double>(1) << std::endl;



  return 0;
}