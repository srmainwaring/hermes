//
// Created by frongere on 29/03/18.
//

#ifndef MESSAGING_POLYMORPHISM_MESSAGE_H
#define MESSAGING_POLYMORPHISM_MESSAGE_H

#include <vector>
#include <memory>

#include "Field.h"


class Serializer;

class Message {

private:

    std::string m_name;
    std::string m_UUID;
    std::string m_ParentUUID;


    typedef std::vector<std::unique_ptr<FieldBase>> FieldList;
    FieldList m_fieldList;

    std::vector<std::shared_ptr<Serializer>> m_serializerList;

public:
    template <class Owner>
    Message(std::string msgName, const Owner owner) : m_name(std::move(msgName)) {}

    const std::string& GetName() const {
        return m_name;
    }

    void AddField(std::string fieldName, std::string unit, std::string description, int* valPtr);
    void AddField(std::string fieldName, std::string unit, std::string description, float* valPtr);
    void AddField(std::string fieldName, std::string unit, std::string description, double* valPtr);
    void AddField(std::string fieldName, std::string unit, std::string description, std::string* valPtr);

    void AddSerializer(std::shared_ptr<Serializer> serializer) {
        m_serializerList.push_back(serializer);
    }

    void Initialize() const;
    void Serialize() const;
    void Finalize() const;
    void Send() const;

    // Iterator definition
//    typedef FieldList::iterator iterator;
    typedef FieldList::const_iterator const_iterator;
    const_iterator begin() const { return m_fieldList.begin(); }
    const_iterator end() const { return  m_fieldList.end(); }
//    const_iterator begin() { return m_fieldList.begin(); }




};


#endif //MESSAGING_POLYMORPHISM_MESSAGE_H
