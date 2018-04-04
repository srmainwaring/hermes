//
// Created by frongere on 03/04/18.
//

#ifndef MESSAGING_POLYMORPHISM_MESSAGING_H
#define MESSAGING_POLYMORPHISM_MESSAGING_H

#include <string>
#include <memory>
#include <vector>
#include <iostream>
//#include "Serializer.h"


template <class T>
class Field;

class Message;

//template <class U>
class Visitor;




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

//    template <class U>
    virtual void Accept(Visitor& visitor) const = 0;

};  // TODO: voir si on conserve cet intermediaire...




template <class T>
class Field : public FieldBase {

private:

    T* m_data = nullptr;

public:
    Field(std::string name, std::string unit, std::string description, T* data)
            : m_data(data), FieldBase(name, unit, description) {}

//    template <class U>
    void Accept(Visitor& visitor) const override;

    const T* GetData() const {
        return m_data;
    }

};


class Serializer {

public:

    virtual void Initialize(Message* msg) = 0;
    virtual void Serialize(Message* msg) = 0;
    virtual void Finalize(Message* msg) = 0;
    virtual void Send(Message* msg) = 0;

};



class Message {  // Message ne derive plus de FieldBase

private:
    std::string m_name;
    std::string m_description;

    typedef std::vector<std::unique_ptr<FieldBase>> VectorType;
    VectorType m_vector;

    std::vector<std::unique_ptr<Serializer>> m_serializers;


public:

//    explicit Message(std::string&& name, std::string&& description) : FieldBase(name, description) {}
    Message(std::string&& name, std::string&& description) : m_name(name), m_description(description) {}

    std::string GetName() { return m_name; }

    template <class T>
    void Add(std::string&& name, std::string&& unit, std::string&& description, T* val) {
        m_vector.emplace_back(std::make_unique<Field<T>>(name, unit, description, val));
    }

    void AddSerializer(Serializer* serializer) {
        m_serializers.push_back(std::unique_ptr<Serializer>(serializer));
    }

//    template <class U>
    void Accept(Visitor& visitor) const {
        for (const auto& fbUPtr : m_vector) {
            fbUPtr->Accept(visitor);
        }
    }

    // Iterators definition over the container
    typedef VectorType::iterator iterator;
    iterator begin() { return m_vector.begin(); }
    iterator end() { return m_vector.end(); }
    typedef VectorType::const_iterator const_iterator;
    const_iterator cbegin() { return m_vector.cbegin(); }
    const_iterator cend() { return m_vector.cend(); }

};

//template <class U>
class Visitor {

private:
    bool m_recursive = true;

public:

    void RecursiveON() { m_recursive = true; }
    void ResursiveOFF() { m_recursive = false; }

    virtual void visit(const Field<int>* field) {}
    virtual void visit(const Field<double>* field) {}
    virtual void visit(const Field<std::string>* field) {}
    virtual void visit(const Field<Message>* field) {
        if (m_recursive) {
            field->GetData()->Accept(*this);
        } else {
            std::cout << "NON RECURSIVE";
        }
    }

};



template <class T>
//template <class U>
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
//    void visit(const Field<Message>* field) override {
//        std::cout << "\tI am a message and I am visiting myself:\n";
//        field->GetData()->Accept(*this);
//
//    }
//    void visit(const Field<Message>* field) override {
//        std::cout << "DE LA BALLE BABY";
//    }
};



#endif //MESSAGING_POLYMORPHISM_MESSAGING_H
