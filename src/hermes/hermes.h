//
// Created by frongere on 25/04/18.
//

#ifndef PROJETDELAMUERTA_HERMES_H
#define PROJETDELAMUERTA_HERMES_H

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <functional>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "Eigen/Dense"


namespace hermes {

  // Forwards declarations
  template<class T>
  class Field;

  class Message;

  class Visitor;

  /*
   * Pure abstract class for field representation. This class is intended to be used as homogeneous element of a message
   * inside a std::vector<FieldBase> which is enclosed into the Message class.
   */
  class FieldBase {

   protected:
    std::string m_name = "";
    std::string m_unit = "";
    std::string m_description = "";

   public:
    FieldBase(std::string name, std::string unit, std::string description) :
        m_name(std::move(name)), m_unit(std::move(unit)), m_description(std::move(description)) {}

    FieldBase(std::string name, std::string description) :
        FieldBase(std::move(name), "", std::move(description)) {}

    std::string GetName() const { return m_name; }

    std::string GetUnit() const { return m_unit; }

    std::string GetDescription() const { return m_description; }

    virtual void Accept(Visitor &visitor) const = 0;

  };

  /*
   * Implementation class for Fields. It is templatged with the type of data it points to.
   */
  template<class T>
  class Field : public FieldBase {

   private:

    std::function<T()> m_getData;


   public:
    Field(std::string name, std::string unit, std::string description, T *data)
        : m_getData([data]() { return *data; }), FieldBase(name, unit, description) {}

    Field(std::string name, std::string unit, std::string description, std::function<T()> data)
        : m_getData([data]() { return data(); }), FieldBase(name, unit, description) {}

    void Accept(Visitor &visitor) const override;

    const T GetData() const {
      return m_getData();
    }

  };


  /*
   * Abstract base class for the serialization of messages. It must use visitor pattern to serialize following its
   * different stages
   */
  class Serializer {

   public:
    virtual ~Serializer() {}

    virtual void Initialize(const Message *msg) = 0;

    virtual void Serialize(const Message *msg) = 0;

    virtual void Finalize(const Message *msg) = 0;

    virtual void Send(const Message *msg) = 0;

  };

  class PrintSerializer;

  class CSVSerializer;

  class HDF5Serializer;

  /*
   * The message class that enclose a vector of fields.
   */
  class Message {

   private:
    std::string m_name;
    std::string m_description;

    typedef std::vector<std::unique_ptr<FieldBase>> VectorType;
    VectorType m_fields;

    unsigned int c_nbFields = 0;
    std::unordered_map<std::string, unsigned int> m_mapper;

    std::vector<std::unique_ptr<Serializer>> m_serializers;


   public:

    Message() = default;

    Message(const std::string &name, const std::string &description) : m_name(name), m_description(description) {}

    void SetName(std::string name) { m_name = name; }

    std::string GetName() const { return m_name; }

    void SetDescription(std::string description) { m_description = description; }

    std::string GetDescription() const { return m_description; }

    void SetNameAndDescription(std::string name, std::string description) {
      m_name = name;
      m_description = description;
    }

    template<class T>
    void AddField(std::string name, std::string unit, std::string description,
                  T *val) { // FIXME : pourquoi on renvoit le pointeur vers le champ ?
      m_fields.emplace_back(std::make_unique<Field<T>>(name, unit, description, val));
      m_mapper[name] = c_nbFields;
      c_nbFields++;
    }

    template<typename T>
    void AddField(std::string name, std::string unit, std::string description, std::function<T()> func) {
      m_fields.emplace_back(std::make_unique<Field<T>>(name, unit, description, func));
      m_mapper[name] = c_nbFields;
      c_nbFields++;
    }

    template<class T>
    Field<T> *GetField(std::string fieldName) {
      return m_fields[m_mapper[fieldName]].get();
    }

    Serializer *AddSerializer(Serializer *serializer) {

      m_serializers.push_back(std::unique_ptr<Serializer>(serializer));
      return m_serializers[m_serializers.size() - 1].get();
    }

    PrintSerializer *AddPrintSerializer();

    CSVSerializer *AddCSVSerializer(std::string CSVFile);

    #ifdef USE_HDF5
    HDF5Serializer *AddHDF5Serializer(std::string HDF5File, std::string HDF5Group);
    #endif


    void ApplyVisitor(Visitor &visitor) const {
      for (const auto &fieldUPtr : m_fields) {
        fieldUPtr->Accept(visitor);
      }
    }

    void Initialize() const {
      for (const auto &serializer : m_serializers) {
        serializer->Initialize(this);
      }
    }

    virtual void Serialize() const {
      for (const auto &serializer : m_serializers) {
        serializer->Serialize(this);
      }
    }

    void Finalize() const {
      for (const auto &serializer : m_serializers) {
        serializer->Finalize(this);
      }
    }

    void Send() const {
      for (const auto &serializer : m_serializers) {
        serializer->Send(this);
      }
    }


    // Iterators definition over the container
    typedef VectorType::iterator iterator;

    iterator begin() { return m_fields.begin(); }

    iterator end() { return m_fields.end(); }

    typedef VectorType::const_iterator const_iterator;

    const_iterator cbegin() { return m_fields.cbegin(); }

    const_iterator cend() { return m_fields.cend(); }

  };


  struct Visitor {

    virtual void visit(const Field<int> *field) = 0;

    virtual void visit(const Field<float> *field) = 0;

    virtual void visit(const Field<double> *field) = 0;

    virtual void visit(const Field<bool> *field) = 0;

    virtual void visit(const Field<std::string> *field) = 0;

    virtual void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) = 0;

    virtual void visit(const Field<std::vector<double>> *field) = 0;

    virtual void visit(const Field<std::function<double()>> *field) = 0;

  };

  template<class T>
  void Field<T>::Accept(Visitor &visitor) const {
    visitor.visit(this);
  }


  template<class C>
  class SerializationVisitor : public Visitor {
   protected:
    C *m_serializer;

    explicit SerializationVisitor(C *serializer) : m_serializer(serializer) {}
  };

#include "PrintSerializer.h"

#include "CSVSerializer.h"

#ifdef USE_HDF5
#include "HDF5Serializer.h"
#endif

  inline PrintSerializer *Message::AddPrintSerializer() {
    auto *printSerializer = AddSerializer(new PrintSerializer());
    return dynamic_cast<PrintSerializer *>(printSerializer);
  }

  inline CSVSerializer *Message::AddCSVSerializer(std::string CSVFile) {
    auto *csvSerializer = AddSerializer(new CSVSerializer(CSVFile));
    return dynamic_cast<CSVSerializer *>(csvSerializer);
  }

  #ifdef USE_HDF5
  inline HDF5Serializer *Message::AddHDF5Serializer(std::string HDF5File, std::string HDF5Group) {
    auto hdf5Serializer = AddSerializer(new HDF5Serializer(HDF5File, HDF5Group));
    return dynamic_cast<HDF5Serializer*>(hdf5Serializer);
  }
  #endif

}  // end namespace hermes

#endif //PROJETDELAMUERTA_HERMES_H
