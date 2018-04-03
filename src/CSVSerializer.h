//
// Created by frongere on 29/03/18.
//

#ifndef MESSAGING_POLYMORPHISM_CSVSERIALIZER_H
#define MESSAGING_POLYMORPHISM_CSVSERIALIZER_H

#include <iostream>
#include <sstream>
#include <fstream>

#include "Serializer.h"
#include "FieldVisitors.h"

class CSVSerializer : public Serializer {

private:

    std::string m_delimiter = ";";
    unsigned int c_delimiterSize = 1;

    mutable std::string m_filename = "";
    mutable std::ofstream m_file;

    mutable std::string c_msgStr;

    mutable bool c_isInit = true;

public:
    void SetDelimiter(std::string delimiter) {
        m_delimiter = std::move(delimiter);
        c_delimiterSize = (unsigned int)m_delimiter.size();
    }

    std::string GetDelimiter() const {
        return m_delimiter;
    }

    std::string GetFileName() const {
        return m_filename;
    }

    void SetFileName(std::string filename) {
        m_filename = std::move(filename);
    }

    void Initialize(const Message* msg) const override {

        if (m_filename.empty()) {
            BuildFileName(msg);
        }

        std::ostringstream ss;

        for (const auto& field : *msg) {
            ss << field->GetName() << " (" << field->GetUnit() << ")" << m_delimiter;
        }
        c_msgStr = std::move(ss.str());
        CloseMsg();

    }

    void Serialize(const Message* msg) const override {

        // TODO: voir si on utilise pas plutot un BasicMemoryWriter
        // Est-ce qu'on peut construire directement un format specifier a l'initialisation ?



//        BuildString visitor;  // TODO: le mettre en membre et appeler une methode clean ?

        MyVisitor visitor;

        std::ostringstream ss;

        for (const auto& field : *msg) {
            field->ApplyVisitor(visitor);
//            ss << visitor.GetString() << m_delimiter;
        }
//        c_msgStr = std::move(ss.str());
//        CloseMsg();

    }

    void Finalize(const Message* msg) const override {

    }


    void Send(const Message* msg) const override {
        if (c_isInit) {
            m_file.open(m_filename, std::ios::trunc);
            c_isInit = false;
        } else {
            m_file.open(m_filename, std::ios::app);
        }
        m_file << c_msgStr;
        m_file.close();

        Flush();
    }

private:
    void BuildFileName(const Message* msg) const {
        std::ostringstream ss;
        ss << msg->GetName() << ".csv";
        m_filename = std::move(ss.str());
    }

    void CloseMsg() const {
        c_msgStr.erase(c_msgStr.size()-c_delimiterSize);
        c_msgStr += "\n";
    }

    void Flush() const {
        c_msgStr = "";
    }


};


#endif //MESSAGING_POLYMORPHISM_CSVSERIALIZER_H
