//
// Created by frongere on 03/04/18.
//

#include "Messaging.h"

CSVSerializer *Message::AddCSVSerializer() {
    auto* csvSerializer = AddSerializer(new CSVSerializer());
    return dynamic_cast<CSVSerializer*>(csvSerializer);
}

PrintSerializer *Message::AddPrintSerializer() {
    auto* printSerializer = AddSerializer(new PrintSerializer());
    return dynamic_cast<PrintSerializer*>(printSerializer);
}
