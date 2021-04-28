//
// Created by frongere on 19/04/2021.
//

#ifndef HERMES_HDF5SERIALIZER_H
#define HERMES_HDF5SERIALIZER_H

#include <H5PacketTable.h>

#include "ByteArray.h"


//// TODO: mettre dans un header dedie
//using h5_id = hid_t;
//class HDF5File {
// public:
//  HDF5File(const std::string filename) : m_filename(filename) {
//
//  }
// private:
//  std::string m_filename;
//  h5_id m_file_id;
//};


class HDF5Serializer : public Serializer {

 private:

  class InitVisitor : public SerializationVisitor<HDF5Serializer> {
    inline void visit(FieldBase *field) {
      // TODO
    }

   public:
    explicit InitVisitor(HDF5Serializer *serializer) : SerializationVisitor<HDF5Serializer>(serializer) {}

    void visit(const Field<int> *field) override { visit((FieldBase *) field); }

    void visit(const Field<float> *field) override { visit((FieldBase *) field); }

    void visit(const Field<double> *field) override { visit((FieldBase *) field); }

    void visit(const Field<bool> *field) override { visit((FieldBase *) field); }

    void visit(const Field <std::string> *field) override { visit((FieldBase *) field); }

    void visit(const Field <Eigen::Matrix<double, 3, 1>> *field) override {
      // TODO
    }

    void visit(const Field <std::vector<double>> *field) override {
      // TODO
    }

    void visit(const Field <std::function<double()>> *field) { visit((FieldBase *) field); }
  };

  class SerializeVisitor : public SerializationVisitor<HDF5Serializer> {

   public:
    explicit SerializeVisitor(HDF5Serializer *serializer) : SerializationVisitor<HDF5Serializer>(serializer) {}

    void visit(const Field<int> *field) override {
      // TODO
    }

    void visit(const Field<float> *field) override {
      // TODO
    }

    void visit(const Field<double> *field) override {
      // TODO
    }

    void visit(const Field<bool> *field) override {
      // TODO
    }

    void visit(const Field <std::string> *field) override {
      // TODO
    }

    void visit(const Field <Eigen::Matrix<double, 3, 1>> *field) override {
      // TODO
    }

    void visit(const Field <std::vector<double>> *field) override {
      // TODO
    }

    void visit(const Field <std::function<double()>> *field) {
      // TODO
    }

  };

 public:
  HDF5Serializer(hid_t HDF5File, std::string HDF5Group) :
      m_serializeVisitor(this),
      m_HDF5File(HDF5File),
      m_HDF5Group(HDF5Group),
      m_is_initialized(false) {}

  void Initialize(const Message *msg) override {
    if (!m_is_initialized) {
      // HDF5 file creation
      // TODO

      // Buffer initialization (inserting fields and memory allocation)
      // TODO

      // HDF5 datatype creation
      // TODO

      // Verification that the group exists and if not, create it (recursively)
      // TODO

      // HDF5 Packet table creation
      // TODO






      m_is_initialized = true;
    }
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

 private:

  ByteArray m_buffer;

  hid_t m_HDF5File;
  std::string m_HDF5Group;

  SerializeVisitor m_serializeVisitor;

  bool m_is_initialized;


};


#endif //HERMES_HDF5SERIALIZER_H
