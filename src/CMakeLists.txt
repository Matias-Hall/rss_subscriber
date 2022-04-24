cmake_minimum_required(VERSION 3.10)
project(rss_subscriber)

set(CMAKE_C_FLAGS_DEBUG_INIT "-g")
set(CMAKE_C_FLAGS_RELEASE "-O2")

if (MSVC)
	add_compile_options(/W4)
else()
	add_compile_options(
		-Wall -Wextra -pedantic
		-Wno-unused-parameter)
endif()
set(I_DIR ../include)
set(HEADER_FILES ${I_DIR}/config.h ${I_DIR}/feed_gather.h ${I_DIR}/url_list.h)
set(S_DIR ../src)
set(SOURCE_FILES ${S_DIR}/config.c ${S_DIR}/feed_gather.c ${S_DIR}/url_list.c)
add_executable(rss_subscriber main.c ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(rss_subscriber
	PRIVATE ${I_DIR}
	PRIVATE /usr/include/libxml2)
target_link_libraries(rss_subscriber
	PRIVATE curl
	PRIVATE xml2)

install(TARGETS rss_subscriber
	CONFIGURATIONS Release
	DESTINATION bin/)
