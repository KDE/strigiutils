add_executable(index2dump indexdump.cpp ../tcharutils.cpp)
target_link_libraries(index2dump streamanalyzer ${CLUCENE1_LDFLAGS})
