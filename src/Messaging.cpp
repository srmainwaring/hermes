//
// Created by frongere on 03/04/18.
//

#include "Messaging.h"

namespace hermes {

    CSVSerializer *Message::AddCSVSerializer() {
        auto *csvSerializer = AddSerializer(new CSVSerializer());
        return dynamic_cast<CSVSerializer *>(csvSerializer);
    }

    PrintSerializer *Message::AddPrintSerializer() {
        auto *printSerializer = AddSerializer(new PrintSerializer());
        return dynamic_cast<PrintSerializer *>(printSerializer);
    }


}  // end namespace hermes