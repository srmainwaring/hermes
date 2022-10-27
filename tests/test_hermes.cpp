#include <gtest/gtest.h>
#include <iostream>

#include "hermes/hermes.h"
#include "hermes/version.h"

struct Obj {
  int i;
  double d;
  std::string str;
};

TEST(Hermes, Basic) {
  using namespace hermes;
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

  m2.AddField<double>("double2", "kk", "Second double", &d);

  m.AddField<float>("float", "", "Un float", &f);

  // Adding two standard serializers
  CSVSerializer* csvSerializer = m.AddCSVSerializer("test_csv_serializer.csv");
  csvSerializer->SetDelimiter(" ; ");

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
}
