//
// Created by frongere on 19/04/2021.
//

#ifndef HERMES_HDF5SERIALIZER_H
#define HERMES_HDF5SERIALIZER_H

#include <H5PacketTable.h>

#include "ByteArray.h"

class HDF5Serializer : public Serializer {

 private:

  class BufferInitVisitor : public SerializationVisitor<HDF5Serializer> {

   public:
    explicit BufferInitVisitor(HDF5Serializer *serializer) : SerializationVisitor<HDF5Serializer>(serializer) {}

    void visit(const Field<int> *field) override {
      m_serializer->m_buffer.insert<int>();
    }

    void visit(const Field<float> *field) override {
      m_serializer->m_buffer.insert<float>();
    }

    void visit(const Field<double> *field) override {
      m_serializer->m_buffer.insert<double>();
    }

    void visit(const Field<bool> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<std::string> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<std::vector<double>> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<std::function<double()>> *field) {
      // TODO
      assert(false);
    }
  };

  class HDF5CompoundInitVisitor : public SerializationVisitor<HDF5Serializer> {

   private:
    inline void visit(FieldBase *field, hid_t hdf5_dtype) {
      H5Tinsert(m_serializer->m_hdf5_compound_dtype, field->GetName().c_str(),
                m_serializer->m_buffer.offset(m_counter), hdf5_dtype);
      m_counter++;
    }

   public:
    explicit HDF5CompoundInitVisitor(HDF5Serializer *serializer) :
        SerializationVisitor<HDF5Serializer>(serializer),
        m_counter(0) {}

    void visit(const Field<int> *field) override {
      visit((FieldBase *) field, H5T_NATIVE_INT);
    }

    void visit(const Field<float> *field) override {
      visit((FieldBase *) field, H5T_NATIVE_FLOAT);
    }

    void visit(const Field<double> *field) override {
      visit((FieldBase *) field, H5T_NATIVE_DOUBLE);
    }

    void visit(const Field<bool> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<std::string> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<std::vector<double>> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<std::function<double()>> *field) {
      // TODO
      assert(false);
    }

   private:
    size_t m_counter;

  };

  class SerializeVisitor : public SerializationVisitor<HDF5Serializer> {

   public:
    explicit SerializeVisitor(HDF5Serializer *serializer) : SerializationVisitor<HDF5Serializer>(serializer) {}

    void visit(const Field<int> *field) override {
      auto val = field->GetData();
      m_serializer->m_buffer << val;
    }

    void visit(const Field<float> *field) override {
      auto val = field->GetData();
      m_serializer->m_buffer << val;
    }

    void visit(const Field<double> *field) override {
      auto val = field->GetData();
      m_serializer->m_buffer << val;
    }

    void visit(const Field<bool> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<std::string> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<std::vector<double>> *field) override {
      // TODO
      assert(false);
    }

    void visit(const Field<std::function<double()>> *field) {
      // TODO
      assert(false);
    }

  };

 public:
  // TODO: voir a exposer chunk_size et compression_level pour avoir un controle global
  HDF5Serializer(hid_t HDF5File, std::string HDF5Group) :
      m_serializeVisitor(this),
      m_hdf5_file_id(HDF5File),
      m_HDF5Group(HDF5Group),
      m_is_initialized(false),
      m_chunk_size(100),
      m_compression_level(0) {}

  void Initialize(const Message *msg) override {
    if (!m_is_initialized) {
      // HDF5 file checking for existence
      // TODO

      /*
       * Il faut :
       *    - une premiere visite pour initialiser le ByteArray buffer
       *    - une seconde visite pour initialiser le COMPOUND_DATATYPE HDF5 a l'aide du ByteArray
       *
       */

      // Initializing the message buffer
      BufferInitVisitor buffer_init_visitor(this);
      msg->ApplyVisitor(buffer_init_visitor);
      m_buffer.allocate();

      // Initializing the HDF5_COMPOUND datatype
      m_hdf5_compound_dtype = H5Tcreate(H5T_COMPOUND, m_buffer.size());
      HDF5CompoundInitVisitor hdf5_compound_init_visitor(this);
      msg->ApplyVisitor(hdf5_compound_init_visitor);

      // Creating the packet table into the file
      m_table_id = H5PTcreate_fl(m_hdf5_file_id, msg->GetName().c_str(), m_hdf5_compound_dtype,
                                 m_chunk_size, m_compression_level);

      m_is_initialized = true;
    }
  }

  void Serialize(const Message *msg) override {
    msg->ApplyVisitor(m_serializeVisitor);
    m_buffer.reset();
    // TODO: voir a etre capable s'etendre le buffer au besoin pour un flush periodique niveau ByteArray
  }

  void Send(const Message *msg) override {
    /// Appending the record to the packet table
    herr_t err = H5PTappend(m_table_id, 1, m_buffer.data());
    if (err < 0)
      fprintf(stderr, "Error adding record.");
  }

  void Finalize(const Message *msg) override {
    H5PTclose(m_table_id);
  }

 private:
  hid_t m_hdf5_file_id;
  std::string m_HDF5Group;

  SerializeVisitor m_serializeVisitor;

  bool m_is_initialized;

  hsize_t m_chunk_size;
  int m_compression_level;

  ByteArray m_buffer;
  hid_t m_hdf5_compound_dtype;
  hid_t m_table_id;

  int m_nflush;

};


#endif //HERMES_HDF5SERIALIZER_H
