include(FetchContent)

FetchContent_Declare(
    httpparser
    QUIET
    OVERRIDE_FIND_PACKAGE
    GIT_REPOSITORY  https://github.com/aaronpo97/httpparser.git
    GIT_TAG         14141ee52f7b1fd0adde3f5f3af0873b3a8bdef3
)


FetchContent_MakeAvailable(httpparser)
