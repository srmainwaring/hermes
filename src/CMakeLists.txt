#
#target_sources(hermes
#        PRIVATE
#            ${CMAKE_CURRENT_LIST_DIR}/hermes/Messaging.cpp
#)

target_include_directories(hermes INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(hermes INTERFACE ${CMAKE_CURRENT_BINARY_DIR})
