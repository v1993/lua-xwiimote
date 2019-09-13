cmake_minimum_required(VERSION 3.8)
project(lua-xwiimote C)
set(CMAKE_C_STANDARD 11)
include(GNUInstallDirs)

if (NOT LUA)
	message(STATUS "Standalone build, finding Lua ourselfes")
	find_package(Lua "5.3" EXACT REQUIRED)
	set(LUA_INCDIR "${LUA_INCLUDE_DIR}")
endif()

find_package(PkgConfig REQUIRED)

pkg_check_modules(XWIIMOTE REQUIRED libxwiimote)

add_library(${PROJECT_NAME} SHARED
	"src/main.c"

	"src/event.c"
	"src/interface.c"
	"src/monitor.c"
)

target_include_directories(${PROJECT_NAME} PRIVATE "${LUA_INCDIR}" ${XWIIMOTE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${XWIIMOTE_LIBRARIES})
target_compile_options(${PROJECT_NAME} PRIVATE ${XWIIMOTE_CFLAGS_OTHER})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "xwiimote")

install(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_LIBDIR}")
