//
// Created by frongere on 03/04/18.
//

#ifndef HERMES_POLYMORPHISM_MESSAGING_H
#define HERMES_POLYMORPHISM_MESSAGING_H

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

   public:
    FieldBase(std::string name, std::string unit, std::string description, unsigned int precision) :
        m_name(std::move(name)), m_unit(std::move(unit)), m_description(std::move(description)),
        m_precision(precision) {}

    FieldBase(std::string name, std::string description, unsigned int precision) :
        FieldBase(std::move(name), "", std::move(description), precision) {}

    std::string GetName() const { return m_name; }

    std::string GetUnit() const { return m_unit; }

    std::string GetDescription() const { return m_description; }

    unsigned int precision() const { return m_precision; }

    virtual void Accept(Visitor &visitor) const = 0;

   protected:
    std::string m_name;
    std::string m_unit;
    std::string m_description;
    unsigned int m_precision;

  };

  /*
 * Implementation class for Fields. It is templated with the type of data it points to.
   */
  template<class T>
  class Field : public FieldBase {

   private:

    std::function<T()> m_getData;


   public:
    Field(std::string name, std::string unit, std::string description, T *data, unsigned int precision)
        : m_getData([data]() { return *data; }), FieldBase(name, unit, description, precision) {}

    Field(std::string name, std::string unit, std::string description, std::function<T()> data, unsigned int precision)
        : m_getData([data]() { return data(); }), FieldBase(name, unit, description, precision) {}

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

  class CSVSerializer;

  class PrintSerializer;

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
    void AddField(std::string name, std::string unit, std::string description, T *val, unsigned int precision = 5) {
      // FIXME : pourquoi on renvoit le pointeur vers le champ ?
      m_fields.emplace_back(std::make_unique<Field<T>>(name, unit, description, val, precision));
      m_mapper[name] = c_nbFields;
      c_nbFields++;
    }

    template<typename T>
    void
    AddField(std::string name, std::string unit, std::string description, std::function<T()> func, unsigned int precision = 5) {
      m_fields.emplace_back(std::make_unique<Field<T>>(name, unit, description, func, precision));
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

    CSVSerializer *AddCSVSerializer(std::string CSVFile);

    PrintSerializer *AddPrintSerializer();


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


  class PrintSerializer : public Serializer {

    class InitVisitor : public SerializationVisitor<PrintSerializer> {

      inline void visit(FieldBase *field) {
        fmt::format_to(m_serializer->m_buffer, "  * {:>10s} ({:^5s}) : {:50s}\n",
                       field->GetName(), field->GetUnit(), field->GetDescription());
      }

     public:
      explicit InitVisitor(PrintSerializer *serializer) :
          SerializationVisitor<PrintSerializer>(serializer) {}

      void visit(const Field<int> *field) override { visit((FieldBase *) field); }

      void visit(const Field<float> *field) override { visit((FieldBase *) field); }

      void visit(const Field<double> *field) override { visit((FieldBase *) field); }

      void visit(const Field<bool> *field) override { visit((FieldBase *) field); }

      void visit(const Field<std::string> *field) override { visit((FieldBase *) field); }

      void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override { visit((FieldBase *) field); }

      void visit(const Field<std::vector<double>> *field) override { visit((FieldBase *) field); }

      void visit(const Field<std::function<double()>> *field) override { visit((FieldBase *) field); }

    };

    class SerializeVisitor : public SerializationVisitor<PrintSerializer> {

     public:
      explicit SerializeVisitor(PrintSerializer *serializer) :
          SerializationVisitor<PrintSerializer>(serializer) {}

      void visit(const Field<int> *field) override {  // TODO: parametrer les formats...
        fmt::format_to(m_serializer->m_buffer, "  * {:>10s} ({:^5s}) : {:d}\n",
                       field->GetName(), field->GetUnit(), field->GetData());
      }

      void visit(const Field<float> *field) override {  // TODO: parametrer les formats...
        fmt::format_to(m_serializer->m_buffer, "  * {:>10s} ({:^5s}) : {:.12g}\n",
                       field->GetName(), field->GetUnit(), field->GetData());
      }

      void visit(const Field<double> *field) override {  // TODO: parametrer les formats...
        fmt::format_to(m_serializer->m_buffer, "  * {:>10s} ({:^5s}) : {:.12g}\n",
                       field->GetName(), field->GetUnit(), field->GetData());
      }

      void visit(const Field<bool> *field) override {
        fmt::format_to(m_serializer->m_buffer, "  * {:>10s} : {:d}\n",
                       field->GetName(), field->GetData());
      }

      void visit(const Field<std::string> *field) override {  // TODO: parametrer les formats...
        fmt::format_to(m_serializer->m_buffer, "  * {:>10s} ({:^5s}) : {:s}\n",
                       field->GetName(), field->GetUnit(), field->GetData());
      }

      void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
        auto vector = field->GetData();
        fmt::format_to(m_serializer->m_buffer, "  * {:>10s} ({:^5s}) : {:.12g}\t{:.12g}\t{:.12g}\n",
                       field->GetName(), field->GetUnit(), vector[0], vector[1], vector[2]);
      }

      void visit(const Field<std::vector<double>> *field) override {
        auto vector = field->GetData();
        fmt::format_to(m_serializer->m_buffer, "  * {:>10s} ({:^5s}) : ",
                       field->GetName(), field->GetUnit());
        for (const auto &element : vector) {
          fmt::format_to(m_serializer->m_buffer, "{}{:.12g}\t", m_serializer->m_buffer.data(), element);
        }
        fmt::format_to(m_serializer->m_buffer, "{}\n", m_serializer->m_buffer.data());
      }

      void visit(const Field<std::function<double(void)>> *field) override {
        fmt::format_to(m_serializer->m_buffer, "  * {:>10s} ({:^5s}) : {:.12g}\n",
                       field->GetName(), field->GetUnit(), (field->GetData())());
      }

    };

    void WriteHeader(const Message *msg) {
      fmt::format_to(m_buffer, "\n[{}] : {}\n", msg->GetName(), msg->GetDescription());
    }

   public:

    PrintSerializer() : m_serializeVisitor(this) {}

    void Initialize(const Message *msg) override {
      // Writing message header
      WriteHeader(msg);

      InitVisitor visitor(this);
      msg->ApplyVisitor(visitor);
    }

    void Serialize(const Message *msg) override {
      WriteHeader(msg);
      msg->ApplyVisitor(m_serializeVisitor);
    }

    void Finalize(const Message *msg) override {

    }

    void Send(const Message *msg) override {
      std::cout << fmt::to_string(m_buffer);
      m_buffer.clear();
    }

   private:
    fmt::memory_buffer m_buffer;
    SerializeVisitor m_serializeVisitor;

  };


  class CSVSerializer : public Serializer {

   private:

    class InitVisitor : public SerializationVisitor<CSVSerializer> {

      inline void visit(FieldBase *field) {
        fmt::format_to(m_serializer->m_buffer, "{}{}", field->GetName(), m_serializer->m_delimiter);
      }

     public:
      explicit InitVisitor(CSVSerializer *serializer) :
          SerializationVisitor<CSVSerializer>(serializer) {}

      void visit(const Field<int> *field) override { visit((FieldBase *) field); }

      void visit(const Field<float> *field) override { visit((FieldBase *) field); }

      void visit(const Field<double> *field) override { visit((FieldBase *) field); }

      void visit(const Field<bool> *field) override { visit((FieldBase *) field); }

      void visit(const Field<std::string> *field) override { visit((FieldBase *) field); }

      void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
        auto name = field->GetName();
        fmt::format_to(m_serializer->m_buffer, "{}{}{}",
                       name + "_0" + m_serializer->m_delimiter,
                       name + "_1" + m_serializer->m_delimiter,
                       name + "_2" + m_serializer->m_delimiter); // FIXME : permettre de fixer des regles d'indicage...
      }

      void visit(const Field<std::vector<double>> *field) override {
        auto n = field->GetData().size();
        auto name = field->GetName();
        for (int i = 0; i < n; i++) {
          fmt::format_to(m_serializer->m_buffer, "{}_{}{}", name, i, m_serializer->m_delimiter);
        }
      }

      void visit(const Field<std::function<double()>> *field) { visit((FieldBase *) field); }


    };

    class UnitLineVisitor : public SerializationVisitor<CSVSerializer> {

      inline void visit(FieldBase *field) {
        fmt::format_to(m_serializer->m_buffer, "{}{}", field->GetUnit(), m_serializer->m_delimiter);
      }

     public:
      explicit UnitLineVisitor(CSVSerializer *serializer) :
          SerializationVisitor<CSVSerializer>(serializer) {}

      void visit(const Field<int> *field) override { visit((FieldBase *) field); }

      void visit(const Field<float> *field) override { visit((FieldBase *) field); }

      void visit(const Field<double> *field) override { visit((FieldBase *) field); }

      void visit(const Field<bool> *field) override { visit((FieldBase *) field); }

      void visit(const Field<std::string> *field) override { visit((FieldBase *) field); }

      void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
        std::string unit_ = field->GetUnit() + m_serializer->m_delimiter;
        fmt::format_to(m_serializer->m_buffer, "{}{}{}", unit_, unit_, unit_);
      }

      void visit(const Field<std::vector<double>> *field) override {
        auto n = field->GetData().size();
        auto unit_ = field->GetUnit();
        for (int i = 0; i < n; i++) {
          fmt::format_to(m_serializer->m_buffer, "{}{}", unit_, m_serializer->m_delimiter);
        }
      }

      void visit(const Field<std::function<double()>> *field) { visit((FieldBase *) field); }
    };

    class SerializeVisitor : public SerializationVisitor<CSVSerializer> {

     public:
      explicit SerializeVisitor(CSVSerializer *serializer) :
          SerializationVisitor<CSVSerializer>(serializer) {}

      void visit(const Field<int> *field) override {
        fmt::format_to(m_serializer->m_buffer, "{:d}{:s}", field->GetData(), m_serializer->m_delimiter);
      }

      void visit(const Field<float> *field) override {
        fmt::format_to(m_serializer->m_buffer, "{:.{}f}{:s}", field->GetData(), field->precision(), m_serializer->m_delimiter);
      }

      void visit(const Field<double> *field) override {
        fmt::format_to(m_serializer->m_buffer, "{:.{}f}{:s}", field->GetData(), field->precision(), m_serializer->m_delimiter);
      }

      void visit(const Field<bool> *field) override {
        fmt::format_to(m_serializer->m_buffer, "{:d}{:s}", field->GetData(), m_serializer->m_delimiter);
      }

      void visit(const Field<std::string> *field) override {
        fmt::format_to(m_serializer->m_buffer, "{:s}{:s}", field->GetData(), m_serializer->m_delimiter);
      }

      void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
        auto precision = field->precision();
        auto vector = field->GetData();
        fmt::format_to(m_serializer->m_buffer, "{:.{}f}{:s}{:.{}f}{:s}{:.{}f}{:s}",
                       vector[0], precision, m_serializer->m_delimiter,
                       vector[1], precision, m_serializer->m_delimiter,
                       vector[2], precision, m_serializer->m_delimiter
        );
      }

      void visit(const Field<std::vector<double>> *field) override {
        auto precision = field->precision();
        auto vector = field->GetData();
        for (const auto &element: vector) {
          fmt::format_to(m_serializer->m_buffer, "{:.{}f}{}", element, precision, m_serializer->m_delimiter);
        }
      }

      void visit(const Field<std::function<double()>> *field) {
        fmt::format_to(m_serializer->m_buffer, "{:.{}f}{:s}", (field->GetData())(), field->precision(),
                       m_serializer->m_delimiter);
      }

    };

    std::string GetCSVLine() {
      // Adding the final line break
      fmt::format_to(m_buffer, "\n");
      return fmt::to_string(m_buffer);
    }

   public:

    explicit CSVSerializer(std::string CSVFile) : m_serializeVisitor(this), m_CSVFile(CSVFile) {}

    void SetDelimiter(const std::string &delimiter) { m_delimiter = delimiter; }

    void Initialize(const Message *msg) override {
      // Building the file name based on rule on message

      InitVisitor visitor(this);
      msg->ApplyVisitor(visitor);
      fmt::format_to(m_buffer, "\n");

      UnitLineVisitor unitLineVisitor(this);
      msg->ApplyVisitor(unitLineVisitor);

    }

    void Serialize(const Message *msg) override {
      msg->ApplyVisitor(m_serializeVisitor);
    }

    void Finalize(const Message *msg) override {}

    void Send(const Message *msg) override {

      if (c_IsInitialized) {
        m_file.open(m_CSVFile, std::ios::app);
      } else {
        m_file.open(m_CSVFile, std::ios::trunc);
        c_IsInitialized = true;
      }

      m_file << GetCSVLine();

      m_file.close();

      m_buffer.clear();
    }

   private:

    std::string m_delimiter = ";";

    fmt::memory_buffer m_buffer;

    SerializeVisitor m_serializeVisitor;

    std::string m_CSVFile;
    std::ofstream m_file;

    bool c_IsInitialized = false;

  };

}  // end namespace hermes

#endif //HERMES_POLYMORPHISM_MESSAGING_H
