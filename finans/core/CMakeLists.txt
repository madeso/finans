include_directories(${CMAKE_CURRENT_BINARY_DIR})

FILE(GLOB src_glob *.cc;*.h)

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
PROTOBUF_GENERATE_CPP(FINANS_PROTO_SRCS FINANS_PROTO_HDRS finans.proto)
set(generated ${FINANS_PROTO_SRCS} ${FINANS_PROTO_HDRS})

set(pbjson_src
	${CMAKE_SOURCE_DIR}/finans/external/pbjson/src/pbjson.cpp
	${CMAKE_SOURCE_DIR}/finans/external/pbjson/src/pbjson.hpp
	${CMAKE_SOURCE_DIR}/finans/external/pbjson/src/bin2ascii.h
	)
include_directories(${CMAKE_SOURCE_DIR}/finans/external/pbjson/src/)

set(src ${src_glob} finans.proto)
source_group("" FILES ${src})
source_group("generated" FILES ${generated})
source_group("generated" FILES ${pbjson_src})

add_library(FinansCore
	${src}
	${generated}
	${pbjson_src}
)

target_link_libraries(FinansCore
	${PROTOBUF_LIBRARIES}
)


