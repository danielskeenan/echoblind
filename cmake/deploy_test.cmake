# Because parts of the test depend on Qt, need to "deploy" it before it will run.
# Otherwise, the build will fail because catch_discover_tests tries to execute the test program POST_BUILD.
#
# Usage: deploy_test(TARGET <target_name>)
function(deploy_test)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs)
    cmake_parse_arguments(PARSE_ARGV 0 arg
            "${options}" "${oneValueArgs}" "${multiValueArgs}"
    )

    if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
        qt_generate_deploy_app_script(
                TARGET "${arg_TARGET}"
                OUTPUT_SCRIPT TEST_DEPLOY_SCRIPT_FILE
                NO_TRANSLATIONS
                NO_COMPILER_RUNTIME
                NO_UNSUPPORTED_PLATFORM_ERROR
        )
        add_custom_command(
                TARGET "${arg_TARGET}" POST_BUILD
                COMMAND "${CMAKE_COMMAND}" "-DCMAKE_INSTALL_PREFIX=$<TARGET_FILE_DIR:${arg_TARGET}>" -DQT_DEPLOY_BIN_DIR=.
                -DQT_DEPLOY_LIB_DIR=. -P "${TEST_DEPLOY_SCRIPT_FILE}"
                VERBATIM
        )
    endif ()
endfunction()
