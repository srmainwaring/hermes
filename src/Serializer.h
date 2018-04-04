//
// Created by frongere on 29/03/18.
//

#ifndef MESSAGING_POLYMORPHISM_SERIALIZER_H
#define MESSAGING_POLYMORPHISM_SERIALIZER_H

#include "Messaging.h"



class PrintSerializer : public Serializer {


private:
    PrintVisitor m_v;


public:
    void Initialize(Message* msg) override {

        for (std::unique_ptr<FieldBase>& fb : *msg) {
            std::cout << fb->GetName() << "\n";
        }

    }

    void Serialize(Message* msg) override {

    }

    void Finalize(Message* msg) override {

    }

    void Send(Message* msg) override {

    }



};




#endif //MESSAGING_POLYMORPHISM_SERIALIZER_H
