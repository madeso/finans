FILE(GLOB src_glob *.cc;*.h)

enable_testing()
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})

set(src ${src_glob})
source_group("" FILES ${src})

add_executable(FinansCoreTest
	${src}
)

target_link_libraries(FinansCoreTest FinansCore ${GTEST_BOTH_LIBRARIES} ${GMOCK_LIBRARY})

add_test(AllTestsInFinansCoreTest FinansCoreTest)

add_custom_command(TARGET FinansCoreTest
                   POST_BUILD
                   COMMAND FinansCoreTest)
