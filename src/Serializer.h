//
// Created by frongere on 29/03/18.
//

#ifndef MESSAGING_POLYMORPHISM_SERIALIZER_H
#define MESSAGING_POLYMORPHISM_SERIALIZER_H

#include "Message.h"

class Serializer {

public:

    virtual void Initialize(const Message* msg) const = 0;
    virtual void Serialize(const Message* msg) const = 0;
    virtual void Finalize(const Message* msg) const = 0;
    virtual void Send(const Message* msg) const = 0;

};


#endif //MESSAGING_POLYMORPHISM_SERIALIZER_H
