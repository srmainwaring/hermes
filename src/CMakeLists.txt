message(STATUS "   ...SOURCE FILES")

set(FILES_LOCAL
#        Field.h
#        Field.cpp
#        Message.h
#        Message.cpp
#        Serializer.h
#        Serializer.cpp
#        CSVSerializer.h
#        CSVSerializer.cpp
#        FieldVisitors.h
#        FieldVisitors.cpp
        Messaging.h
        Messaging.cpp
        )

foreach(file ${FILES_LOCAL})
    list(APPEND HERMES_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(HERMES_SRC_FILES ${HERMES_SRC_FILES} PARENT_SCOPE)