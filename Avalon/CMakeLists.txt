
file(GLOB_RECURSE AVALON_SOURCES
				Source/*.hpp
				Source/*.cpp
				Source/*.h
				Source/*.c)

add_library(Avalon STATIC ${AVALON_SOURCES})

target_include_directories(Avalon PUBLIC Source)

target_link_libraries(Avalon PUBLIC spdlog::spdlog)