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
  ByteArray() : m_bytes(nullptr), m_buffer_size(0), m_offset(0) {}

  ~ByteArray() {
    m_elements.clear();
    free(m_bytes);
  }

  void reserve(const std::size_t &n) {
    m_buffer_size = n;
    m_bytes = static_cast<std::byte *>(malloc(n));
  }

  void reset() {
    m_offset = 0;
  }

  template <typename T>
  ByteArray& operator<<(T&val) {
    std::size_t elt_s = sizeof(T);
    memcpy(m_bytes + m_offset, &val, elt_s);
    m_elements.emplace_back(std::pair{m_offset, elt_s});
    m_offset += elt_s;
    return *this;
  }

  std::byte* get() {
    return m_bytes;
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
  std::vector<std::pair<std::size_t, std::size_t>> m_elements;

  std::size_t m_offset;
};


#endif //HERMES_BYTEARRAY_H
