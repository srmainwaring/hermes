#include <iostream>

#include "src/Messaging.h"
#include "src/Serializer.h"
#include "fmt/format.h"
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


    // Adding two standard serializers
    CSVSerializer* csvSerializer = m.AddCSVSerializer();
    csvSerializer->SetDelimiter(" ; ");

//    PrintSerializer* printSerializer = m.AddPrintSerializer();

    // Adding a vector
    std::vector<double> vector;
    vector.reserve(10);

    double val = 0;
    double dt = 0.2;
    for (int idx=0; idx<10; ++idx) {
        vector.push_back(val);
        val += dt;
//        std::cout << val;
    }


    // Essai pour renseigner dans un message
    std::string fieldame;
    for (int idx=0; idx<vector.size(); ++idx) {
        fieldame = fmt::format("x_{:d}", idx);
//        std::cout << fieldame << std::endl;
        m.AddField(fieldame, "", "", &vector[idx]);

    }

    m.AddPrintSerializer();


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