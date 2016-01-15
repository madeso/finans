FILE(GLOB src_glob *.cc;*.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(src ${src_glob})
source_group("" FILES ${src})

add_library(FinansCore
	${src}
)
