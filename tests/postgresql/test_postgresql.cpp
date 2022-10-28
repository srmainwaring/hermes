#include "gtest/gtest.h"

#include <thread>

#include <spdlog/spdlog.h>
#include <pqxx/pqxx>

#include "hermes/hermes.h"

TEST(Hermes, PostgreSQL) {
  spdlog::set_level(spdlog::level::trace);

  struct MessageData {
    int i{};
    double d{};
    std::string str;
    bool bo{};
    std::chrono::time_point<std::chrono::system_clock> time;
  };
  MessageData data;
  hermes::Message m("MSG1", "Le premier message");

  m.AddField("Nb", "m/s", "Un entier", &data.i);
  m.AddField("pi", "km", "Un double", &data.d);
  m.AddField("name", "--", "Une chaine", &data.str);
  m.AddField("is_something", "--", "", &data.bo);
  m.AddField("time", "--", "Generation timestamp", &data.time);


  std::shared_ptr<pqxx::connection> connection;
  try {
    connection = std::make_shared<pqxx::connection>("user=postgres password=password host=localhost dbname=database");
  } catch (const pqxx::failure& err) {
    spdlog::info("Connection failed ! {}", err.what());
  }
  m.AddPSQLSerializer(connection);

  // Initialize
  m.Initialize();
  m.Send();

  // Using the message serialization
  for (int idx = 0; idx < 10; ++idx) {
    data.i += 1;
    data.d = static_cast<double>(data.i) / 100;
    data.str = fmt::format("Currend index is {}", idx);
    data.bo = idx < 4;
    data.time = std::chrono::system_clock::now();
    m.Serialize();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  m.Send();
}
