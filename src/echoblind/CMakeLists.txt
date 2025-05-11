qt_add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE
        main.cpp
        FileSelectorWidget.h
        FileSelectorWidget.cpp
        MainWindow.h
        MainWindow.cpp
        Settings.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        echoconfig
        Qt::Widgets
        Qt::Svg
)

install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES "$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>"
        TYPE BIN
)
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    qt_generate_deploy_app_script(
            TARGET ${PROJECT_NAME}
            OUTPUT_SCRIPT DEPLOY_SCRIPT
            NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${DEPLOY_SCRIPT})
endif ()
