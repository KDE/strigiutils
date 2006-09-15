include_directories( ../)

ADD_EXECUTABLE(filtertest filtertest.cpp)

TARGET_LINK_LIBRARIES(filtertest filters)

