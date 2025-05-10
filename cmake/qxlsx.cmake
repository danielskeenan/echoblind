include(FetchContent)
FetchContent_Declare(qxlsx
        GIT_REPOSITORY "https://github.com/QtExcel/QXlsx.git"
        GIT_TAG "v1.5.0"
        SOURCE_SUBDIR "QXlsx"
)
FetchContent_MakeAvailable(qxlsx)