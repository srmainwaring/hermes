message(STATUS "   ...SOURCE FILES")

set(FILES_LOCAL
        Field.h
        Field.cpp
        Message.h
        Message.cpp
        Serializer.h
        Serializer.cpp
        CSVSerializer.h
        CSVSerializer.cpp
        FieldVisitors.h
        FieldVisitors.cpp
        )

foreach(file ${FILES_LOCAL})
    list(APPEND FILES ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()

set(FILES ${FILES} PARENT_SCOPE)