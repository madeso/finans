FILE(GLOB src_glob *.cc;*.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(src ${src_glob})
source_group("" FILES ${src})

add_executable(FinansCmd
	${src}
)

target_link_libraries(FinansCmd
	FinansCore
)
