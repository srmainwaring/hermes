//
// Created by frongere on 29/03/18.
//

#include "Field.h"
#include "FieldVisitors.h"

void IntField::ApplyVisitor(VisitorInterface& visitor) const {
    visitor.VisitIntField(*this);
}

void FloatField::ApplyVisitor(VisitorInterface& visitor) const {
    visitor.VisitFloatField(*this);
}

void DoubleField::ApplyVisitor(VisitorInterface& visitor) const {
    visitor.VisitDoubleField(*this);
}

void StringField::ApplyVisitor(VisitorInterface& visitor) const {
    visitor.VisitStringField(*this);
}

void VectorField::ApplyVisitor(VisitorInterface& visitor) const {
    // TODO !!!
}
