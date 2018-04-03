//
// Created by frongere on 30/03/18.
//

#ifndef MESSAGING_POLYMORPHISM_VISITORS_H
#define MESSAGING_POLYMORPHISM_VISITORS_H

#include <iostream>
#include "Field.h"

#include "fmt/format.h"




class VisitorInterface {
public:
    virtual void visit(const IntField* f) const = 0;
    virtual void visit(const FloatField* f) const = 0;
    virtual void visit(const DoubleField* f) const = 0;
    virtual void visit(const StringField* f) const = 0;
};


//template <class T>
class MyVisitor : public VisitorInterface {

    fmt::MemoryWriter mw;

public:
    MyVisitor() {
        std::cout << "Build MyVisitor\n";
    }

    void visit(const IntField* f) const override {
        std::cout << "int\n";
    }
    void visit(const FloatField* f) const override {
        std::cout << "float\n";
    }
    void visit(const DoubleField* f) const override {
        std::cout << "double\n";
    }
    void visit(const StringField* f) const override {
        std::cout << "string\n";
    }

};

////template <class Derived>
//class MyString : public MyVisitor<MyString> {
//public:
//    MyString() {
//        std::cout << "Build MyString\n";
//    }
//
////    void visit
//
//
//
//};









//class MyVisitor : fmt::BasicArgFormatter<MyVisitor, char> {
//
//public:
//    MyVisitor(fmt::BasicFormatter<char, MyVisitor> &f, fmt::FormatSpec &s, const char& fmt) :
//            fmt::BasicArgFormatter<MyVisitor, char>(f, s, fmt) {}
//
//
//};


// A custom argument formatter that formats negative integers as unsigned
// with the ``x`` format specifier.
class CustomArgFormatter :
        public fmt::BasicArgFormatter<CustomArgFormatter, char> {
public:
    CustomArgFormatter(fmt::BasicFormatter<char, CustomArgFormatter> &f,
                       fmt::FormatSpec &s, const char *fmt)
            : fmt::BasicArgFormatter<CustomArgFormatter, char>(f, s, fmt) {}

    void visit_int(int value) {
        if (spec().type() == 'x')
            visit_uint(value); // convert to unsigned and format
        else
            fmt::BasicArgFormatter<CustomArgFormatter, char>::visit_int(value);
    }
};

//std::string myformat(const char *format_str, fmt::ArgList args) {
//    fmt::MemoryWriter writer;
//    // Pass custom argument formatter as a template arg to BasicFormatter.
//    fmt::BasicFormatter<char, CustomArgFormatter> formatter(args, writer);
//    formatter.format(format_str);
//    return writer.str();
//}
//FMT_VARIADIC(std::string, custom_format, const char *)




////        public VisitorInterface,
////        public fmt::BasicArgFormatter<BuildString, char> {
//
////    std::string m_str = "";
//
//public:
//
//    BuildString()
//
////    void VisitIntField(const IntField& f) {
//////        int val = f.GetValue();
//////        auto a = visit_int(val);
//////        m_str = a;
////    }
////    void VisitFloatField(const FloatField& f) { m_str = "float"; }
////    void VisitDoubleField(const DoubleField&) { m_str = "double"; }
////    void VisitStringField(const StringField& f) { m_str = "string"; }
////
////    const std::string& GetString() const { return m_str; }
//};



// An argument visitor based on the curiously recurring template pattern. --> TODO: voir a generaliser !!!
//class BuildString :
////        public VisitorInterface,
////        public fmt::BasicArgFormatter<BuildString, char> {
//
//    std::string m_str = "";
//
//public:
//
//    void VisitIntField(const IntField& f) {
//        int val = f.GetValue();
//        auto a = visit_int(val);
//        m_str = a;
//    }
//    void VisitFloatField(const FloatField& f) { m_str = "float"; }
//    void VisitDoubleField(const DoubleField&) { m_str = "double"; }
//    void VisitStringField(const StringField& f) { m_str = "string"; }
//
//    const std::string& GetString() const { return m_str; }
//};

















//class VisitorInterface {
//
//public:
//    virtual void VisitIntField(const IntField& f) = 0;
//    virtual void VisitFloatField(const FloatField& f) = 0;
//    virtual void VisitDoubleField(const DoubleField&) = 0;
//    virtual void VisitStringField(const StringField& f) = 0;
////    virtual void VisitVectorField(const VectorField& f) = 0;
//};
//
//class BuildString : public VisitorInterface {
//
//private:
//    std::string m_str = "";
//
//    std::string intFormat = "{:d}";
//    std::string doubleFormat = "{:3.12g}";
//    std::string stringFormat = "{}";
//
////    fmt::ArgFormatter<std::string> m_ArgFormatter;
//
//
//
//public:
//
////    BuildString() {
////
////        auto formatter = fmt::BasicFormatter<std::string>();
////
////        m_ArgFormatter = fmt::ArgFormatter<std::string>();
////    }
//
//    const std::string& GetString() const { return m_str; }
//
//    void VisitIntField(const IntField& f) override {
//        m_str = std::to_string(f.GetValue());
//    }
//
//    void VisitFloatField(const FloatField& f) override {
//        m_str = std::to_string(f.GetValue());
//    }
//
//    void VisitDoubleField(const DoubleField& f) override {
////        m_str = std::to_string(f.GetValue());
//        m_str = fmt::format("{:.12f}", f.GetValue());
//    }
//
//    void VisitStringField(const StringField& f) override {
//        m_str = f.GetValue();
//    }
//
////    void VisitVectorField(const VectorField& f) override {
//////        m_str = std::to_string(f.GetValue());
////        // TODO !!!
////    }
//
//
//};



#endif //MESSAGING_POLYMORPHISM_VISITORS_H
