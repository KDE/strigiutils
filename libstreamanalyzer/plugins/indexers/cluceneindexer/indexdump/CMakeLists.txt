add_executable(indexdump indexdump.cpp ../tcharutils.cpp)
target_link_libraries(indexdump streamanalyzer ${CLUCENE_LIBRARY})
