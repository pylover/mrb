cmake_minimum_required(VERSION 3.7)
project(mrb 
    VERSION 1.1.0
    LANGUAGES C
)


configure_file(manifest.h.in manifest.h)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE=1")


include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)


# Static Library
add_library(mrb STATIC mrb.c)


# Install
install(TARGETS mrb DESTINATION "lib")
install(FILES mrb.h DESTINATION "include")


# CPack
set(CPACK_PACKAGE_FILE_NAME "libmrb-${PROJECT_VERSION}")
set(CPACK_SET_DESTDIR true)
set(CPACK_PACKAGE_NAME libmrb)
set(CPACK_PACKAGE_CONTACT "Vahid Mardani <vahid.mardani@gmail.com>")
set(CPACK_GENERATOR DEB)
include(CPack)


# Testing
include(CTest)
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")


# Test Magic ringbuffer
add_executable(mrb_test mrb_test.c)
target_link_libraries(mrb_test PUBLIC mrb clog)
add_test(NAME mrb_test COMMAND mrb_test)
