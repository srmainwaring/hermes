#include <iostream>

#include "hermes/hermes.h"

using namespace hermes;

bool g_not_a_serializer_called = false;

class NotASerializer : public Serializer {
public:
    ~NotASerializer() {
        g_not_a_serializer_called = true;
    }
    void Initialize(const Message *msg) override {};
    void Serialize(const Message *msg) override {};
    void Finalize(const Message *msg) override {};
    void Send(const Message *msg) override {};
};

int main() {
    {
        // Create a unique pointer of serializer.
        // The destructor of NotASerializer should be called at the end of the
        // scope
        std::unique_ptr<Serializer> serializer(new NotASerializer());
        assert(!g_not_a_serializer_called);
    }
    assert(g_not_a_serializer_called);
    return 0;
}
