//
// Created by frongere on 29/03/18.
//

#include "Field.h"
#include "Message.h"
#include "Serializer.h"


void Message::AddField(std::string fieldName, std::string unit, std::string description, int *valPtr) {
    m_fieldList.emplace_back(std::make_unique<IntField>(fieldName, unit, description, valPtr));
}

void Message::AddField(std::string fieldName, std::string unit, std::string description, float *valPtr) {
    m_fieldList.emplace_back(std::make_unique<FloatField>(fieldName, unit, description, valPtr));
}

void Message::AddField(std::string fieldName, std::string unit, std::string description, double *valPtr) {
    m_fieldList.emplace_back(std::make_unique<DoubleField>(fieldName, unit, description, valPtr));
}

void Message::AddField(std::string fieldName, std::string unit, std::string description, std::string *valPtr) {
    m_fieldList.emplace_back(std::make_unique<StringField>(fieldName, unit, description, valPtr));
}

void Message::Initialize() const {
    for (const auto& serializer : m_serializerList) {
        serializer->Initialize(this);
    }
}

void Message::Serialize() const {
    for (const auto& serializer : m_serializerList) {
        serializer->Serialize(this);
    }
}

void Message::Finalize() const {
    for (const auto& serializer : m_serializerList) {
        serializer->Finalize(this);
    }
}

void Message::Send() const {
    for (const auto& serializer : m_serializerList) {
        serializer->Send(this);
    }
}