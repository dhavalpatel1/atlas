function(configure_test_target)
    message(STATUS "Configuring test target")
    add_custom_target(test)
endfunction(configure_test_target)

function(configure_test target shortName)
    message(STATUS "Configuring ${shortName} test")

    set(testTargetName "test.${shortName}")
    add_custom_target(${testTargetName} COMMAND ${target} VERBATIM USES_TERMINAL)
    add_dependencies(${testTargetName} ${target})

    if(TARGET test)
        add_dependencies(test ${testTargetName})
    endif()
endfunction(configure_test)