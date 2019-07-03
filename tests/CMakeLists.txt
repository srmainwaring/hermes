

list(APPEND tests
        test_hermes
        test_eigenVectorsSerialization
        test_serializerDestructor
        )

message(STATUS "Hermes tests...")
foreach(test ${tests})
    message(STATUS "   ...${test}")
    add_executable(${test} "${test}.cpp")
    target_link_libraries(${test} hermes)
endforeach()


IF(MINGW)
	#=== Copy the DLL Libs to the output *.exe files ===
	SET (DLL_Libs
		${CMAKE_BINARY_DIR}/libhermes.dll
		${CMAKE_BINARY_DIR}/bin/libfmt.dll
	)
	FOREACH (depLib ${DLL_Libs})
		ADD_CUSTOM_COMMAND (
			TARGET test_hermes
			POST_BUILD
			COMMAND ${CMAKE_COMMAND}
			ARGS -E copy ${depLib} "${CMAKE_BINARY_DIR}/tests/"
		)
	ENDFOREACH (depLib)
ENDIF(MINGW)