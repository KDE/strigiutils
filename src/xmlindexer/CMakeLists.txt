include_directories(
        ../streamanalyzer
        ../streams
	${strigi_BINARY_DIR}/src/streams/compat
	${BZIP2_INCLUDE_DIR}
)

add_executable(xmlindexer
        xmlindexer.cpp
        indexer.cpp
        tagmapping.cpp
	xmlindexwriter.cpp
)

target_link_libraries(xmlindexer streamanalyzer)

install(TARGETS xmlindexer RUNTIME DESTINATION bin)

add_executable(peranalyzerxml
	peranalyzerxml.cpp
	indexer.cpp
	tagmapping.cpp
	xmlindexwriter.cpp
)
target_link_libraries(peranalyzerxml streamanalyzer)

add_executable(peranalyzerxml peranalyzerxml.cpp indexer.cpp xmlindexwriter.cpp
    tagmapping.cpp)
target_link_libraries(peranalyzerxml streamanalyzer)

add_executable(perfieldxml perfieldxml.cpp indexer.cpp xmlindexwriter.cpp
    tagmapping.cpp)
target_link_libraries(perfieldxml streamanalyzer)

if(NOT WIN32)
  add_executable(ontoprint ontoprint.cpp)
  target_link_libraries(ontoprint streamanalyzer)
endif(NOT WIN32)

# look for all config files
FILE(GLOB_RECURSE allfiles ../../testdata/analyzers/*/config)
FOREACH(file ${allfiles})
  MESSAGE("configfile ${analyzers}")
  STRING(REPLACE "/config" "*bz2" ofile ${file})
  FILE(GLOB_RECURSE ofiles ${ofile})
  # TODO add the list of analyzers as an argument
  FOREACH(ofile ${ofiles})
    STRING(REPLACE "/analyzers/" "/data/" ifile ${ofile})
    STRING(REPLACE "*/" "" testname ${ofile})
    ADD_TEST(${testname} peranalyzerxml -a ${analyzers} -r ${ofile} ${ifile})
  ENDFOREACH(ofile ${ofiles})
ENDFOREACH(file ${allfiles})
