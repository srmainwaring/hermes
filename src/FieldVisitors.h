//
// Created by frongere on 30/03/18.
//

#ifndef MESSAGING_POLYMORPHISM_VISITORS_H
#define MESSAGING_POLYMORPHISM_VISITORS_H

#include "Field.h"

struct VisitorInterface {

    virtual void VisitIntField(const IntField& f) = 0;
    virtual void VisitFloatField(const FloatField& f) = 0;
    virtual void VisitDoubleField(const DoubleField&) = 0;
    virtual void VisitStringField(const StringField& f) = 0;
//    virtual void VisitVectorField(const VectorField& f) = 0;
};

struct BuildString : public VisitorInterface {

    std::string m_str = "";

public:
    void VisitIntField(const IntField& f) override {
        m_str = std::to_string(f.GetValue());
    }

    void VisitFloatField(const FloatField& f) override {
        m_str = std::to_string(f.GetValue());
    }

    void VisitDoubleField(const DoubleField& f) override {
        m_str = std::to_string(f.GetValue());
    }

    void VisitStringField(const StringField& f) override {
        m_str = f.GetValue();
    }

//    void VisitVectorField(const VectorField& f) override {
////        m_str = std::to_string(f.GetValue());
//        // TODO !!!
//    }


};



#endif //MESSAGING_POLYMORPHISM_VISITORS_H
