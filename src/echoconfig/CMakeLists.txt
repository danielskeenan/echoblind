add_library(echoconfig STATIC
        ${PROJECT_SOURCE_DIR}/include/echoconfig/Circuit.h
        ${PROJECT_SOURCE_DIR}/include/echoconfig/Config.h
        Config.cpp
        ${PROJECT_SOURCE_DIR}/include/echoconfig/Preset.h
)

find_package(Boost CONFIG COMPONENTS bimap REQUIRED)
target_link_libraries(echoconfig PUBLIC
        Qt::Core
        Boost::bimap
)