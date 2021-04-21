//
// Created by frongere on 19/04/2021.
//

#ifndef HERMES_PRINTSERIALIZER_H
#define HERMES_PRINTSERIALIZER_H

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

//            void visit(const Field<Message> *field) override {
//                auto msg = field->GetData();
//                msg.ApplyVisitor(*this);
//            }

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

#endif //HERMES_PRINTSERIALIZER_H
