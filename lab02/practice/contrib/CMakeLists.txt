include(FetchContent)

FetchContent_Declare(
    oatpp
    QUIET
    OVERRIDE_FIND_PACKAGE
    GIT_REPOSITORY https://github.com/oatpp/oatpp.git
    GIT_TAG 1.3.1
)


FetchContent_MakeAvailable(oatpp)

