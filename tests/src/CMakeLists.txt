cmake_minimum_required(VERSION 3.10)
project(rss_subscriber_tests)

set(INCLUDE_DIR ../include)
set(EXT_INCLUDE_DIR ../../include)
set(HEADER_FILES ${INCLUDE_DIR}/minunit.h ${EXT_INCLUDE_DIR}/config.h ${EXT_INCLUDE_DIR}/feed_gather.h ${EXT_INCLUDE_DIR}/url_list.h)
set(SOURCE_FILES ../../src/config.c ../../src/feed_gather.c ../../src/url_list.c)
set(CMAKE_C_FLAGS_DEBUG_INIT "-g")
add_executable(test test.c  ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(test
	PRIVATE ${INCLUDE_DIR}
	PRIVATE ${EXT_INCLUDE_DIR})
target_link_libraries(test PRIVATE curl)
