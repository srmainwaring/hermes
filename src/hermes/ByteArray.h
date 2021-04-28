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
  ByteArray() : m_bytes(nullptr), m_buffer_size(0), m_next(0) {}

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
    m_next = 0;
  }

  std::byte *data() {
    return m_bytes;
  }

  template<typename T>
  ByteArray &operator<<(T &val) {
    assert(("Buffer is full and should be reset", m_next != m_offsets.size()));
    memcpy(m_bytes + m_offsets[m_next], &val, sizeof(T));
    m_next +=1;
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

  std::size_t m_next;

};


#endif //HERMES_BYTEARRAY_H
