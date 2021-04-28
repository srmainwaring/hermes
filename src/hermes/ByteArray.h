//
// Created by frongere on 28/04/2021.
//

#ifndef HERMES_BYTEARRAY_H
#define HERMES_BYTEARRAY_H

#include <utility>
#include <vector>
#include <cassert>
#include <cstddef>
#include <iostream>

class ByteArray {

 public:
  ByteArray() : m_bytes(nullptr), m_buffer_size(0), c_offset(0) {}

  ~ByteArray() {
    m_offsets.clear();
    free(m_bytes);
  }

  template<typename T>
  void insert(T &val) {
    m_offsets.push_back(m_buffer_size);
    m_buffer_size += sizeof(T);
  }

  void allocate() {
    m_bytes = static_cast<std::byte *>(malloc(m_buffer_size));
  }

  size_t size() const {
    return m_buffer_size;
  }

  void reset() {
    c_offset = 0;
  }

  std::byte *data() {
    return m_bytes;
  }

  template<typename T>
  ByteArray &operator<<(T &val) {
    std::size_t elt_s = sizeof(T);
    memcpy(m_bytes + c_offset, &val, elt_s);
    c_offset += elt_s;
    return *this;
  }


  template<typename T>
  T &as(std::size_t i) const {
    size_t offset = m_offsets[i];
    return *reinterpret_cast<T *>(m_bytes + offset);
  }

 private:
  std::byte *m_bytes;
  std::size_t m_buffer_size;
  std::vector<std::size_t> m_offsets; // offset, type size

  std::size_t c_offset;

//  std::size_t m_offset;
};


#endif //HERMES_BYTEARRAY_H
