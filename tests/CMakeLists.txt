
include(Add_diemer)

list(APPEND tests
        test_get_version
        test_hermes
        test_eigenVectorsSerialization
        test_serializerDestructor
        )

if (HERMES_USE_HDF5)
    list(APPEND tests
            test_hdf5PT
            test_hdf5TB
            test_hdf5PT_fields
            test_byte_serialize
            )
endif ()


message(STATUS "Hermes tests...")
foreach (test ${tests})
    message(STATUS "   ...${test}")
    add_executable(${test} "${test}.cpp")
    target_link_libraries(${test} hermes diemer)
endforeach ()
