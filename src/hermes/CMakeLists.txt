add_library(hermes
  hermes.h
  field_base.h
  field_base.cpp
  field.h

  visitor.h
  serialization_visitor.h

  serializer.h
  serializer/print_serializer.h
  serializer/print_serializer.cpp
  serializer/csv_serializer.h
  serializer/csv_serializer.cpp

  message.h
  message.cpp
)
target_link_libraries(hermes PUBLIC fmt Eigen3::Eigen)

target_include_directories(hermes PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(hermes PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)
target_compile_definitions(hermes PUBLIC HERMES_MAX_LOG_LEVEL=4)
