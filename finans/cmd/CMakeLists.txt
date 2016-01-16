FILE(GLOB src_glob *.cc;*.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories(${CMAKE_SOURCE_DIR}/finans/external/tclap-1.2.1/include/)

set(src ${src_glob})
source_group("" FILES ${src})

add_executable(FinansCmd
	${src}
)

target_link_libraries(FinansCmd
	FinansCore
)
target_link_libraries(FinansCmd
	FinansCore
)
