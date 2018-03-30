#include <iostream>

#include "src/Message.h"
#include "src/CSVSerializer.h"
#include "src/FieldVisitors.h"

using namespace std;

struct Obj {
    int i;
    double d;
    string str;
};



int main() {

    Obj o;
    o.i = 2;
    o.d = 3.1498979;
    o.str = "coucou";



    Message m("my message", o);

    m.AddField("Entier", "no unit", "Represente un entier", &o.i);
    m.AddField("double", "no unit", "Represente un double", &o.d);
    m.AddField("String", "no unit", "Represente une string", &o.str);

    m.AddSerializer(std::make_shared<CSVSerializer>());



    m.Initialize();
    m.Send();

    for (uint i=0; i<3; ++i) {
        m.Serialize();
        m.Send();
    }



//    // Test visitor
//    BuildString stringbuilder;
//
//    IntField field("essai", "m/s", "coucou", &o.i);
//
////    field.ApplyVisitor(&stringbuilder);






    return 0;

}