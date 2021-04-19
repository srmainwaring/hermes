// ==========================================================================
// hermes
//
// Copyright (c) D-ICE Engineering.
// All rights reserved.
//
// Use of this source code is governed by a GPLv3 license that can be found
// in the LICENSE file of FRyDoM.
//
// ==========================================================================

#include "Messaging.h"

namespace hermes {

  CSVSerializer *Message::AddCSVSerializer(std::string CSVFile) {
    auto *csvSerializer = AddSerializer(new CSVSerializer(CSVFile));
    return dynamic_cast<CSVSerializer *>(csvSerializer);
  }

  PrintSerializer *Message::AddPrintSerializer() {
    auto *printSerializer = AddSerializer(new PrintSerializer());
    return dynamic_cast<PrintSerializer *>(printSerializer);
  }


}  // end namespace hermes
