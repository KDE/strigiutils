include_directories(
        ../streamanalyzer
        ../streams
	../streams/compat
	${strigi_BINARY_DIR}/src/streams/compat
	${BZIP2_INCLUDE_DIR}
)

add_executable(xmlindexer
        xmlindexer.cpp
        tagmapping.cpp
	xmlindexwriter.cpp
)

target_link_libraries(xmlindexer streamanalyzer)

add_executable(cgixmlindexer
        tagmapping.cpp
	xmlindexwriter.cpp
	cgixmlindexer.cpp)

target_link_libraries(cgixmlindexer streamanalyzer)

install(TARGETS xmlindexer RUNTIME DESTINATION bin)

add_executable(peranalyzerxml
	peranalyzerxml.cpp
	tagmapping.cpp
	xmlindexwriter.cpp
)
target_link_libraries(peranalyzerxml streamanalyzer)

add_executable(peranalyzerxml peranalyzerxml.cpp xmlindexwriter.cpp
    tagmapping.cpp)
target_link_libraries(peranalyzerxml streamanalyzer)

add_executable(perfieldxml perfieldxml.cpp xmlindexwriter.cpp
    tagmapping.cpp)
target_link_libraries(perfieldxml streamanalyzer)

# is this still broken on win32?
if(NOT WIN32)
  add_executable(ontoprint ontoprint.cpp)
  target_link_libraries(ontoprint streamanalyzer)
endif(NOT WIN32)

# register all tests based on the data in the testdata directory
FILE(GLOB_RECURSE allfiles ../../testdata/analyzers/*/config)
FOREACH(file ${allfiles})
  GET_FILENAME_COMPONENT(testdir ${file} PATH)
  GET_FILENAME_COMPONENT(dir ${testdir} NAME)
  FILE(GLOB_RECURSE ofiles "${testdir}/*")
  FOREACH(ofile ${ofiles})
    STRING(REPLACE "/analyzers/${dir}/" "/data/" ifile ${ofile})
    IF(NOT ${ifile} MATCHES "/.svn/" AND NOT ${ifile} MATCHES "config$")
      STRING(REPLACE "*/" "" testname ${ofile})
      STRING(REPLACE ${testdir} "" testname ${ofile})
      ADD_TEST("${dir}${testname}" peranalyzerxml -c ${testdir}/config
        -r ${ofile} ${ifile})
    ENDIF(NOT ${ifile} MATCHES "/.svn/" AND NOT ${ifile} MATCHES "config$")
  ENDFOREACH(ofile ${ofiles})
ENDFOREACH(file ${allfiles})
