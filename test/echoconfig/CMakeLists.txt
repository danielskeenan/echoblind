add_executable(echoconfig_test
        EchoPcpConfigTest.cpp
)

find_package(Catch2 3 REQUIRED)
target_link_libraries(echoconfig_test PRIVATE
        Catch2::Catch2WithMain
        echoconfig
        Qt::Core
        Qt::Xml
        QXlsx
)
target_compile_definitions(echoconfig_test PRIVATE "RESOURCES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/resources\"")

include(deploy_test)
deploy_test(TARGET echoconfig_test)

include(CTest)
include(Catch)
catch_discover_tests(echoconfig_test)
