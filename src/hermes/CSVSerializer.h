//
// Created by frongere on 19/04/2021.
//

#ifndef HERMES_CSVSERIALIZER_H
#define HERMES_CSVSERIALIZER_H

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
      fmt::format_to(m_serializer->m_buffer, "{:.12g}{:s}", field->GetData(), m_serializer->m_delimiter);
    }

    void visit(const Field<double> *field) override {
      fmt::format_to(m_serializer->m_buffer, "{:.12g}{:s}", field->GetData(), m_serializer->m_delimiter);
    }

    void visit(const Field<bool> *field) override {
      fmt::format_to(m_serializer->m_buffer, "{:d}{:s}", field->GetData(), m_serializer->m_delimiter);
    }

    void visit(const Field<std::string> *field) override {
      fmt::format_to(m_serializer->m_buffer, "{:s}{:s}", field->GetData(), m_serializer->m_delimiter);
    }

    void visit(const Field<Eigen::Matrix<double, 3, 1>> *field) override {
      auto vector = field->GetData();
      fmt::format_to(m_serializer->m_buffer, "{:.12g}{:s}{:.12g}{:s}{:.12g}{:s}",
                     vector[0], m_serializer->m_delimiter,
                     vector[1], m_serializer->m_delimiter,
                     vector[2], m_serializer->m_delimiter
      );
    }

    void visit(const Field<std::vector<double>> *field) override {
      auto vector = field->GetData();
      for (const auto &element: vector) {
        fmt::format_to(m_serializer->m_buffer, "{:.12g}{}", element, m_serializer->m_delimiter);
      }
    }

    void visit(const Field<std::function<double()>> *field) {
      fmt::format_to(m_serializer->m_buffer, "{:.12g}{:s}", (field->GetData())(), m_serializer->m_delimiter);
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

#endif //HERMES_CSVSERIALIZER_H
