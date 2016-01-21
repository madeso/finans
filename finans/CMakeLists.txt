include_directories(${CMAKE_SOURCE_DIR})

if(${APPLE})
	add_definitions(-DFINANS_APPLE)
endif()

if(${UNIX})
	add_definitions(-DFINANS_UNIX)
else()
	add_definitions(-DFINANS_WINDOWS)
endif()

add_subdirectory(core)
add_subdirectory(core_test)
add_subdirectory(cmd)
# add_subdirectory(gui)
