//
// Created by frongere on 29/03/18.
//

#ifndef MESSAGING_POLYMORPHISM_FIELD_H
#define MESSAGING_POLYMORPHISM_FIELD_H

#include <string>
#include <vector>

class VisitorInterface;

class FieldBase {

protected:
    std::string m_name;
    std::string m_unit;
    std::string m_description;

public:
    FieldBase(std::string name, std::string unit, std::string description) :
    m_name(std::move(name)),
    m_unit(std::move(unit)),
    m_description(std::move(description)) {}

    const std::string& GetName() const { return m_name; }
    const std::string& GetUnit() const { return m_unit; }
    const std::string& GetDescription() const { return m_description; }

    virtual void ApplyVisitor(VisitorInterface& visitor) const = 0;

};

/*
 * Field to hold an int
 */
class IntField : public FieldBase {
    int* m_valPtr;

public:
    IntField(std::string name, std::string unit, std::string description, int* ptr) :
            FieldBase(std::move(name), std::move(unit), std::move(description)),
            m_valPtr(ptr) {}

    int& GetValue() const {
        return *m_valPtr;
    }

    void ApplyVisitor(VisitorInterface& visitor) const override;
};

/*
 * Field to hold a float
 */
class FloatField : public FieldBase {
    float* m_valPtr;

public:
    FloatField(std::string name, std::string unit, std::string description, float* ptr) :
            FieldBase(std::move(name), std::move(unit), std::move(description)),
            m_valPtr(ptr) {}

    float& GetValue() const {
        return *m_valPtr;
    }

    void ApplyVisitor(VisitorInterface& visitor) const override;
};

/*
 * Field to hold a double
 */
class DoubleField : public FieldBase {
    double* m_valPtr;

public:
    DoubleField(std::string name, std::string unit, std::string description, double* ptr) :
            FieldBase(std::move(name), std::move(unit), std::move(description)),
            m_valPtr(ptr) {}

    double& GetValue() const {
        return *m_valPtr;
    }

    void ApplyVisitor(VisitorInterface& visitor) const override;
};

/*
 * Field to hold a std::string
 */
class StringField : public FieldBase {
    std::string* m_valPtr;

public:
    StringField(std::string name, std::string unit, std::string description, std::string* ptr) :
            FieldBase(std::move(name), std::move(unit), std::move(description)),
            m_valPtr(ptr) {}

    std::string& GetValue() const {
        return *m_valPtr;
    }

    void ApplyVisitor(VisitorInterface& visitor) const override;
};

/*
 * Field to hold a std::vector of int
 */
class VectorField : public FieldBase {
    std::vector<int>* m_valPtr;

public:
    VectorField(std::string name, std::string unit, std::string description, std::vector<int>* ptr) :
            FieldBase(std::move(name), std::move(unit), std::move(description)),
            m_valPtr(ptr) {}

    std::vector<int>& GetValue() const {
        return *m_valPtr;
    }

    void ApplyVisitor(VisitorInterface& visitor) const override;
};


#endif //MESSAGING_POLYMORPHISM_FIELD_H
