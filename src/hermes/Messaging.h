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

#include "fmt/format.h"

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
                : m_getData([data]() { return *data;}), FieldBase(name, unit, description) {}

        Field(std::string name, std::string unit, std::string description, std::function<T()> data)
                : m_getData([data]() { return data(); } ), FieldBase(name, unit, description) {}

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

        Message(std::string &&name, std::string &&description) : m_name(name), m_description(description) {}

        void SetName(std::string name) { m_name = name; }
        std::string GetName() const { return m_name; }

        void SetDescription(std::string description) { m_description = description; }
        std::string GetDescription() const { return m_description; }

        void SetNameAndDescription(std::string name, std::string description) {
            m_name = name;
            m_description = description;
        }

        template<class T>
        Field<T> *AddField(std::string name, std::string unit, std::string description, T *val) { // FIXME : pourquoi on renvoit le pointeur vers le champ ?
            m_fields.emplace_back(std::make_unique<Field<T>>(name, unit, description, val));
            m_mapper[name] = c_nbFields;
            c_nbFields++;
        }

        template <typename T>
        Field<T> *AddField(std::string name, std::string unit, std::string description, std::function<T()> func) {
            m_fields.emplace_back(std::make_unique<Field<T>>(name, unit, description, func));
            m_mapper[name] = c_nbFields;
            c_nbFields++;
        }

        template <class T>
        Field<T>* GetField(std::string fieldName) {
            return m_fields[m_mapper[fieldName]].get();
        }

        Serializer *AddSerializer(Serializer *serializer) {

            m_serializers.push_back(std::unique_ptr<Serializer>(serializer));
            return m_serializers[m_serializers.size() - 1].get();
        }

        CSVSerializer *AddCSVSerializer();

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

//        virtual void visit(const Field<Message> *field) = 0;

        virtual void visit(const Field<std::function<double ()>> *field) = 0;

//    virtual void visit(const Field<std::vector<int>>* field) = 0;

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
                m_serializer->m_w.write("  * {:>10s} ({:^5s}) : {:50s}\n",
                                        field->GetName(), field->GetUnit(), field->GetDescription());
            }

        public:
            explicit InitVisitor(PrintSerializer *serializer) :
                    SerializationVisitor<PrintSerializer>(serializer) {}

            void visit(const Field<int> *field) override { visit((FieldBase *) field); }

            void visit(const Field<float> *field) override { visit((FieldBase *) field); }

            void visit(const Field<double> *field) override { visit((FieldBase *) field); }

            void visit(const Field<bool> *field) override  {visit((FieldBase *) field); }

            void visit(const Field<std::string> *field) override { visit((FieldBase *) field); }

//            void visit(const Field<Message> *field) override {
//                auto msg = field->GetData();
//                m_serializer->m_w.write("\n[{}] : {}\n", msg.GetName(), msg.GetDescription());
//                msg.ApplyVisitor(*this);
//            }

            void visit(const Field<std::function<double ()>> *field) override { visit((FieldBase *) field); }



        };

        class SerializeVisitor : public SerializationVisitor<PrintSerializer> {

        public:
            explicit SerializeVisitor(PrintSerializer *serializer) :
                    SerializationVisitor<PrintSerializer>(serializer) {}

            void visit(const Field<int> *field) override {  // TODO: parametrer les formats...
                m_serializer->m_w.write("  * {:>10s} ({:^5s}) : {:d}\n",
                                        field->GetName(), field->GetUnit(), field->GetData());
            }

            void visit(const Field<float> *field) override {  // TODO: parametrer les formats...
                m_serializer->m_w.write("  * {:>10s} ({:^5s}) : {:.12g}\n",
                                        field->GetName(), field->GetUnit(), field->GetData());
            }

            void visit(const Field<double> *field) override {  // TODO: parametrer les formats...
                m_serializer->m_w.write("  * {:>10s} ({:^5s}) : {:.12g}\n",
                                        field->GetName(), field->GetUnit(), field->GetData());
            }

            void visit(const Field<bool> *field) override {
                m_serializer->m_w.write("  * {:>10s} : {:d}\n",
                                        field->GetName(), field->GetData());
            }

            void visit(const Field<std::string> *field) override {  // TODO: parametrer les formats...
                m_serializer->m_w.write("  * {:>10s} ({:^5s}) : {:s}\n",
                                        field->GetName(), field->GetUnit(), field->GetData());
            }

//            void visit(const Field<Message> *field) override {
//                auto msg = field->GetData();
//                msg.ApplyVisitor(*this);
//            }

            void visit(const Field<std::function<double (void)>> *field) override {
                m_serializer->m_w.write("  * {:>10s} ({:^5s}) : {:.12g}\n",
                                        field->GetName(), field->GetUnit(), (field->GetData())());
            }

        };

        void WriteHeader(const Message *msg) {
            m_w.write("\n[{}] : {}\n", msg->GetName(), msg->GetDescription());
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
            std::cout << m_w.c_str();
            m_w.clear();
        }

    private:
        fmt::MemoryWriter m_w;
        SerializeVisitor m_serializeVisitor;

    };


    class CSVSerializer : public Serializer {

    private:

        class InitVisitor : public SerializationVisitor<CSVSerializer> {

            inline void visit(FieldBase *field) {
                m_serializer->m_w << field->GetName() << m_serializer->m_delimiter;
            }

        public:
            explicit InitVisitor(CSVSerializer *serializer) :
                    SerializationVisitor<CSVSerializer>(serializer) {}

            void visit(const Field<int> *field) override { visit((FieldBase *) field); }

            void visit(const Field<float> *field) override { visit((FieldBase *) field); }

            void visit(const Field<double> *field) override { visit((FieldBase *) field); }

            void visit(const Field<bool> *field) override { visit((FieldBase *) field); }

            void visit(const Field<std::string> *field) override { visit((FieldBase *) field); }

//            void visit(const Field<Message> *field) override {
//                field->GetData().ApplyVisitor(*this);
//            }

            void visit(const Field<std::function<double ()>> *field) { visit((FieldBase *) field); }


        };

        class UnitLineVisitor : public SerializationVisitor<CSVSerializer> {

            inline void visit(FieldBase *field) {
                m_serializer->m_w << field->GetUnit() << m_serializer->m_delimiter;
            }

        public:
            explicit UnitLineVisitor(CSVSerializer *serializer) :
                    SerializationVisitor<CSVSerializer>(serializer) {}

            void visit(const Field<int> *field) override { visit((FieldBase *) field); }

            void visit(const Field<float> *field) override { visit((FieldBase *) field); }

            void visit(const Field<double> *field) override { visit((FieldBase *) field); }

            void visit(const Field<bool> *field) override { visit((FieldBase *) field); }

            void visit(const Field<std::string> *field) override { visit((FieldBase *) field); }

//            void visit(const Field<Message> *field) override {
//                field->GetData().ApplyVisitor(*this);
//            }

            void visit(const Field<std::function<double ()>> *field) { visit((FieldBase *) field); }
        };

        class SerializeVisitor : public SerializationVisitor<CSVSerializer> {

        public:
            explicit SerializeVisitor(CSVSerializer *serializer) :
                    SerializationVisitor<CSVSerializer>(serializer) {}

            void visit(const Field<int> *field) override {
                m_serializer->m_w.write("{:d}{:s}", field->GetData(), m_serializer->m_delimiter);
            }

            void visit(const Field<float> *field) override {
                m_serializer->m_w.write("{:.12g}{:s}", field->GetData(), m_serializer->m_delimiter);
            }

            void visit(const Field<double> *field) override {
                m_serializer->m_w.write("{:.12g}{:s}", field->GetData(), m_serializer->m_delimiter);
            }

            void visit(const Field<bool> *field) override {
                m_serializer->m_w.write("{:d}{:s}", field->GetData(), m_serializer->m_delimiter);
            }

            void visit(const Field<std::string> *field) override {
                m_serializer->m_w.write("{:s}{:s}", field->GetData(), m_serializer->m_delimiter);
            }

//            void visit(const Field<Message> *field) override {
//                field->GetData().ApplyVisitor(*this);
//            }

            void visit(const Field<std::function<double ()>> *field) {
                m_serializer->m_w.write("{:.12g}{:s}", (field->GetData())(), m_serializer->m_delimiter);
            }

        };

        void BuildFileName(const Message *msg) {
            m_filename = fmt::format("{}.csv", msg->GetName());
        }

        std::string GetCSVLine() {
            // Adding the final line break
            m_w << "\n";
            return m_w.str();
        }

    public:

        CSVSerializer() : m_serializeVisitor(this) {}

        void SetDelimiter(const std::string &&delimiter) { m_delimiter = delimiter; }

        void Initialize(const Message *msg) override {
            // Building the file name based on rule on message
            BuildFileName(msg);

            InitVisitor visitor(this);
            msg->ApplyVisitor(visitor);
            m_w << "\n";

            UnitLineVisitor unitLineVisitor(this);
            msg->ApplyVisitor(unitLineVisitor);

        }

        void Serialize(const Message *msg) override {
            msg->ApplyVisitor(m_serializeVisitor);
        }

        void Finalize(const Message *msg) override {}

        void Send(const Message *msg) override {

            if (c_IsInitialized) {
                m_file.open(m_filename, std::ios::app);
            } else {
                m_file.open(m_filename, std::ios::trunc);
                c_IsInitialized = true;
            }

            m_file << GetCSVLine();

            m_file.close();

            m_w.clear();
        }

    private:

        std::string m_delimiter = ";";

        fmt::MemoryWriter m_w;

        SerializeVisitor m_serializeVisitor;

        std::string m_filename;
        std::ofstream m_file;

        bool c_IsInitialized = false;

    };

}  // end namespace hermes

#endif //HERMES_POLYMORPHISM_MESSAGING_H
