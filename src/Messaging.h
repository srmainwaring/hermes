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

    std::string GetName() { return m_name; }
    std::string GetDescription() { return m_description; }

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



class PrintVisitor : public Visitor {
public:
    void visit(const Field<int>* field) override {
        std::cout << "I am an int : "<< *field->GetData() << "\n";
    }
    void visit(const Field<double>* field) override {
        std::cout << "I am a double : "<< *field->GetData() << "\n";
    }
    void visit(const Field<std::string>* field) override {
        std::cout << "I am a string : "<< *field->GetData() << "\n";
    }
    void visit(const Field<Message>* field) override {
        std::cout << "\tI am a message and I am visiting myself:\n";
        field->GetData()->Accept(*this);
    }
};





class CSVSerializer : public Serializer {

private:

    class CSVVisitor : public Visitor {
    protected:
        CSVSerializer* m_serializer;

        std::string m_str="";

        explicit CSVVisitor(CSVSerializer* serializer) : m_serializer(serializer) {}

    public:
        void PostProcStr() {
            m_str.erase(m_str.size()-m_serializer->m_delimiter.size());
            m_str += "\n";
        }

        std::string GetCSVLine() {
            PostProcStr();
            return m_str;
        }
    };

    class InitVisitor : public CSVVisitor {

        inline void visit(FieldBase* field) {
            std::ostringstream ss;
            ss << field->GetName() << m_serializer->m_delimiter;
            m_str += ss.str(); // TODO : Voir pour utiliser fmt en mode write
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
            std::ostringstream ss;
            ss << *field->GetData() << m_serializer->m_delimiter;
            m_str += ss.str();
        }
        void visit(const Field<double>* field) override {
            std::ostringstream ss;
            ss << *field->GetData() << m_serializer->m_delimiter;
            m_str += ss.str();
        }
        void visit(const Field<std::string>* field) override {
            std::ostringstream ss;
            ss << *field->GetData() << m_serializer->m_delimiter;
            m_str += ss.str();
        }
        void visit(const Field<Message>* field) override {
            field->GetData()->Accept(*this);
        }

    };


public:

    CSVSerializer() : m_serializeVisitor(this) {}

    void SetDelimiter(const std::string&& delimiter) { m_delimiter = delimiter; }


    void Initialize(const Message* msg) override {
        InitVisitor visitor(this);
        msg->Accept(visitor);
        m_str = visitor.GetCSVLine();

        // TODO: initialiser le fichier !!
    }

    void Serialize(const Message* msg) override {
        msg->Accept(m_serializeVisitor);
        m_str = m_serializeVisitor.GetCSVLine();
    }
    void Finalize(const Message* msg) override {}
    void Send(const Message* msg) override {
        std::cout << m_str;
    }

private:

    std::string m_delimiter=";";
    std::string m_str = "";

    SerializeVisitor m_serializeVisitor;


};



#endif //MESSAGING_POLYMORPHISM_MESSAGING_H
