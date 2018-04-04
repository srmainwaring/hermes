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

    Message m("MSG1", "Le premier message");


    m.AddField<int>("Nb", "m/s", "Un entier", &o.i);
    m.AddField<double>("pi", "km", "Un double", &o.d);
    m.AddField<std::string>("name", "--", "Une chaine", &o.str);

    Message m2("MSG2", "Un second message");

    int i = -23;
    double d = -1.414;
    float f = 1.397e12;


    auto field_entier2 = m2.AddField<int>("entier2", "", "Second entier", &i);


    m2.AddField<double>("double2", "kk", "Second double", &d);


    m.AddField<Message>("message", "", "Second message", &m2);

    m.AddField<float>("float", "", "Un float", &f);

    // Building a CSV serializer
    auto csvSerializer = std::make_shared<CSVSerializer>();
//    csvSerializer->SetDelimiter(",");
    m.AddSerializer(csvSerializer);

    // Building a print serializer
    auto printSerializer = std::make_shared<PrintSerializer>();
    m.AddSerializer(printSerializer);

    // Initializing the message
    m.Initialize();
    m.Send();

    // Using the message serialization
    for (int idx=0; idx<10; ++idx) {
        m.Serialize();
        m.Send();
    }









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