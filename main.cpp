#include <iostream>

#include "src/Messaging.h"
#include "src/Serializer.h"

//using namespace std;

struct Obj {
    int i;
    double d;
    std::string str;
};



int main() {

    Obj o;
    o.i = 2;
    o.d = 3.14989796584651681;
    o.str = "coucou";

    Message m("message1", "Le premier message");


    m.Add<int>("entier", "m/s", "Un entier", &o.i);
    m.Add<double>("double", "km", "Un double", &o.d);
    m.Add<std::string>("string", "--", "Une chaine", &o.str);

    Message m2("message2", "Un second message");

    int i = 23;
    double d = 1.414;

    m2.Add<int>("entier2", "", "Second entier", &i);
    m2.Add<double>("double2", "kk", "Second double", &d);


    m.Add<Message>("message", "", "Second message", &m2);


//    PrintVisitor v;
//    m.Accept(v);


//    PrintSerializer ser;
//    ser.Initialize(&m);

    auto serializer = std::make_shared<CSVSerializer>();
    serializer->SetDelimiter(",");

    m.AddSerializer(serializer);


    m.Initialize();
    m.Send();


    m.Serialize();
    m.Send();








//    Message m("my message", o);
//
//    m.AddField("Entier", "no unit", "Represente un entier", &o.i);
//    m.AddField("double", "no unit", "Represente un double", &o.d);
//    m.AddField("String", "no unit", "Represente une string", &o.str);
//
//    m.AddSerializer(std::make_shared<CSVSerializer>());
//
//
//
//    m.Initialize();
//    m.Send();
//
//    for (uint i=0; i<3; ++i) {
//        m.Serialize();
//        m.Send();
//    }
//
//
//
////    // Test visitor
////    BuildString stringbuilder;
////
////    IntField field("essai", "m/s", "coucou", &o.i);
////
//////    field.ApplyVisitor(&stringbuilder);
//
//
////    auto a = MyString();
//
//    Derived a;





    return 0;

}