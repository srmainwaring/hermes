//
// Created by frongere on 03/04/18.
//

#ifndef MESSAGING_POLYMORPHISM_MESSAGING_H
#define MESSAGING_POLYMORPHISM_MESSAGING_H

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "fmt/format.h"

// Forwards declarations
template <class T>
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

    virtual void Accept(Visitor& visitor) const = 0;

};

/*
 * Implementation class for Fields. It is templatged with the type of data it points to.
 */
template <class T>
class Field : public FieldBase {

private:

    const T* m_data = nullptr;

public:
    Field(std::string name, std::string unit, std::string description, T* data)
            : m_data(data), FieldBase(name, unit, description) {}

    void Accept(Visitor& visitor) const override;

    const T* GetData() const {
        return m_data;
    }

};

/*
 * Abstract base class for the serialization of messages. It must use visitor pattern to serialize following its
 * different stages
 */
class Serializer {

public:

    virtual void Initialize(const Message* msg) = 0;
    virtual void Serialize(const Message* msg) = 0;
    virtual void Finalize(const Message* msg) = 0;
    virtual void Send(const Message* msg) = 0;

};


/*
 * The message class that enclose a vector of fields.
 */
class Message {

private:
    std::string m_name;
    std::string m_description;

    typedef std::vector<std::unique_ptr<FieldBase>> VectorType;
    VectorType m_fields;

    std::vector<std::shared_ptr<Serializer>> m_serializers;


public:

    Message(std::string&& name, std::string&& description) : m_name(name), m_description(description) {}

    std::string GetName() const { return m_name; }
    std::string GetDescription() const { return m_description; }

    template <class T>
    void Add(std::string&& name, std::string&& unit, std::string&& description, T* val) {
        m_fields.emplace_back(std::make_unique<Field<T>>(name, unit, description, val));
    }

    void AddSerializer(std::shared_ptr<Serializer> serializer) {
        m_serializers.push_back(serializer);
    }

    void Accept(Visitor& visitor) const {
        for (const auto& fieldUPtr : m_fields) {
            fieldUPtr->Accept(visitor);
        }
    }

    void Initialize() const {
        for (const auto& serializer : m_serializers) {
            serializer->Initialize(this);
        }
    }

    void Serialize() const {
        for (const auto& serializer : m_serializers) {
            serializer->Serialize(this);
        }
    }

    void Finalize() const {
        for (const auto& serializer : m_serializers) {
            serializer->Finalize(this);
        }
    }

    void Send() const {
        for (const auto& serializer : m_serializers) {
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

    virtual void visit(const Field<int>* field) = 0;
    virtual void visit(const Field<double>* field) = 0;
    virtual void visit(const Field<std::string>* field) = 0;
    virtual void visit(const Field<Message>* field) = 0;

};

template <class T>
void Field<T>::Accept(Visitor &visitor) const {
    visitor.visit(this);
}



//class PrintVisitor : public Visitor {
//public:
//    void visit(const Field<int>* field) override {
//        std::cout << "I am an int : "<< *field->GetData() << "\n";
//    }
//    void visit(const Field<double>* field) override {
//        std::cout << "I am a double : "<< *field->GetData() << "\n";
//    }
//    void visit(const Field<std::string>* field) override {
//        std::cout << "I am a string : "<< *field->GetData() << "\n";
//    }
//    void visit(const Field<Message>* field) override {
//        std::cout << "\tI am a message and I am visiting myself:\n";
//        field->GetData()->Accept(*this);
//    }
//};





class PrintSerializer : public Serializer {

    class PrintVisitor : public Visitor {
    protected:
        std::string m_str="";

    public:
        std::string GetOutput() {
            return m_str;
        }
    };

    class InitVisitor : public PrintVisitor {

        inline void visit(FieldBase* field) {
            std::ostringstream ss;
            ss << "  * " << field->GetName() << " (" << field->GetUnit() << ") : " << field->GetDescription() << "\n";
            m_str += ss.str(); // TODO : Voir pour utiliser fmt en mode write
        }

    public:
//        explicit InitVisitor(CSVSerializer* serializer) : CSVVisitor(serializer) {}

        void visit(const Field<int>* field) override { visit((FieldBase*)field); }
        void visit(const Field<double>* field) override { visit((FieldBase*)field); }
        void visit(const Field<std::string>* field) override { visit((FieldBase*)field); }
        void visit(const Field<Message>* field) override {
            auto msg = field->GetData();

            std::ostringstream ss;
            ss << "\n\t[" << msg->GetName() << "] : " << msg->GetDescription() << "\n";
            m_str += ss.str();

            msg->Accept(*this);
        }
    };

    class SerializeVisitor : public PrintVisitor {

    public:
//        explicit SerializeVisitor(CSVSerializer* serializer) : CSVVisitor(serializer) {}

        void visit(const Field<int>* field) override {
            std::ostringstream ss;
//            ss
//            ss << *field->GetData() << m_serializer->m_delimiter;
//            m_str += ss.str();
        }
        void visit(const Field<double>* field) override {
//            std::ostringstream ss;
//            ss << *field->GetData() << m_serializer->m_delimiter;
//            m_str += ss.str();
        }
        void visit(const Field<std::string>* field) override {
//            std::ostringstream ss;
//            ss << *field->GetData() << m_serializer->m_delimiter;
//            m_str += ss.str();
        }
        void visit(const Field<Message>* field) override {
//            field->GetData()->Accept(*this);
        }

    };

    void MessageHeader(const Message* msg) {
        std::ostringstream ss;
        ss << "\n[" << msg->GetName() << "] : " << msg->GetDescription() << "\n";
        m_str += ss.str();
    }

public:
    void Initialize(const Message* msg) override {
        MessageHeader(msg);

        InitVisitor visitor;
        msg->Accept(visitor);
        m_str += visitor.GetOutput();
    }

    void Serialize(const Message* msg) override {
        MessageHeader(msg);

        SerializeVisitor visitor;
        msg->Accept(visitor);
        m_str = visitor.GetOutput();
    }

    void Finalize(const Message* msg) override {

    }

    void Send(const Message* msg) override {
        std::cout << m_str << std::endl;
    }

private:
    std::string m_str;

};



class CSVSerializer : public Serializer {

private:

    class CSVVisitor : public Visitor {
    protected:
        CSVSerializer* m_serializer;

        fmt::MemoryWriter m_w;

        explicit CSVVisitor(CSVSerializer* serializer) : m_serializer(serializer) {}

    public:
        void Clear() {
            m_w.clear();
        }

        const char* GetCSVLine() {
            return m_w.c_str();
        }
    };

    class InitVisitor : public CSVVisitor {

        inline void visit(FieldBase* field) {
            m_w << field->GetName() << m_serializer->m_delimiter;
        }

    public:
        explicit InitVisitor(CSVSerializer* serializer) : CSVVisitor(serializer) {}

        void visit(const Field<int>* field) override { visit((FieldBase*)field); }
        void visit(const Field<double>* field) override { visit((FieldBase*)field); }
        void visit(const Field<std::string>* field) override { visit((FieldBase*)field); }
        void visit(const Field<Message>* field) override {
            field->GetData()->Accept(*this);
        }
    };

    class SerializeVisitor : public CSVVisitor {

    public:
        explicit SerializeVisitor(CSVSerializer* serializer) : CSVVisitor(serializer) {}

        void visit(const Field<int>* field) override {
            m_w << *field->GetData() << m_serializer->m_delimiter;
        }
        void visit(const Field<double>* field) override {
            m_w << *field->GetData() << m_serializer->m_delimiter;
        }
        void visit(const Field<std::string>* field) override {
            m_w << *field->GetData() << m_serializer->m_delimiter;
        }
        void visit(const Field<Message>* field) override {
            field->GetData()->Accept(*this);
        }

    };

    void BuildFileName(const Message* msg) {
        m_filename = fmt::format("{}.csv", msg->GetName());
    }

    std::string GetLine() {
        // Post-processing the string to remove the trailing delimiter and adding a final line break
        std::string str = m_w.str();
        str.erase(str.size()-m_delimiter.size());
        str += "\n";
        return str;
    }

public:

    CSVSerializer() : m_serializeVisitor(this) {}

    void SetDelimiter(const std::string&& delimiter) { m_delimiter = delimiter; }

    void Initialize(const Message* msg) override {
        // Building the file name based on rule on message
        BuildFileName(msg);

        InitVisitor visitor(this);
        msg->Accept(visitor);
        m_w.clear();
        m_w << visitor.GetCSVLine();

    }

    void Serialize(const Message* msg) override {
        m_serializeVisitor.Clear();
        msg->Accept(m_serializeVisitor);
        m_w.clear();
        m_w << m_serializeVisitor.GetCSVLine();
    }
    void Finalize(const Message* msg) override {}
    void Send(const Message* msg) override {

        if (c_IsInitialized) {
            m_file.open(m_filename, std::ios::app);
        } else {
            m_file.open(m_filename, std::ios::trunc);
            c_IsInitialized = true;
        }

        m_file << GetLine();
        m_file.close();
        m_w.clear();

    }

private:

    std::string m_delimiter=";";

    fmt::MemoryWriter m_w;

    SerializeVisitor m_serializeVisitor;

    std::string m_filename;
    std::ofstream m_file;

    bool c_IsInitialized = false;

};



#endif //MESSAGING_POLYMORPHISM_MESSAGING_H
