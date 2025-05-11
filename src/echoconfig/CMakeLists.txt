add_library(echoconfig STATIC
        ${PROJECT_SOURCE_DIR}/include/echoconfig/EchoAcpConfig.h
        EchoAcpConfig.cpp
        ${PROJECT_SOURCE_DIR}/include/echoconfig/EchoPcpConfig.h
        EchoPcpConfig.cpp
        ${PROJECT_SOURCE_DIR}/include/echoconfig/Circuit.h
        ${PROJECT_SOURCE_DIR}/include/echoconfig/Config.h
        Config.cpp
        ${PROJECT_SOURCE_DIR}/include/echoconfig/sheet_helpers.h
        sheet_helpers.cpp
        ${PROJECT_SOURCE_DIR}/include/echoconfig/Preset.h
        ${PROJECT_SOURCE_DIR}/include/echoconfig/Space.h
        ${PROJECT_SOURCE_DIR}/include/echoconfig/xml_helpers.h
        xml_helpers.cpp
)

include(qxlsx)
target_link_libraries(echoconfig PUBLIC
        Qt::Core
)
target_link_libraries(echoconfig PRIVATE
        QXlsx
)
