file(GLOB_RECURSE AVALON_VIEWER_SOURCES
				Source/*.hpp
				Source/*.cpp
				Source/*.h
				Source/*.c)

add_executable(AvalonSceneViewer WIN32 ${AVALON_VIEWER_SOURCES})
target_link_libraries(AvalonSceneViewer PUBLIC Avalon)
target_link_libraries(AvalonSceneViewer PUBLIC SDL3::SDL3-static)