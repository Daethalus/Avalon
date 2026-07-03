
file(GLOB_RECURSE AVALON_SOURCES
				Source/*.hpp
				Source/*.cpp
				Source/*.h
				Source/*.c)

add_library(Avalon STATIC ${AVALON_SOURCES})

target_include_directories(Avalon PUBLIC Source)

target_link_libraries(Avalon PUBLIC spdlog::spdlog)

if (WIN32)
	target_link_libraries(Avalon PRIVATE
					d3d12
					dxgi
					dxguid
	)
endif ()