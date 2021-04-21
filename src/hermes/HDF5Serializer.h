//
// Created by frongere on 19/04/2021.
//

#ifndef HERMES_HDF5SERIALIZER_H
#define HERMES_HDF5SERIALIZER_H

class HDF5Serializer : public Serializer {

 public:
  HDF5Serializer(std::string HDF5File, std::string HDF5Group) {
    // TODO
  }

  void Initialize(const Message *msg) override {
    // TODO
  }

  void Serialize(const Message *msg) override {
    // TODO
  }

  void Finalize(const Message *msg) override {
    // TODO
  }

  void Send(const Message *msg) override {
    // TODO
  }

};


#endif //HERMES_HDF5SERIALIZER_H
