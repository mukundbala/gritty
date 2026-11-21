cmake_minimum_required(VERSION 3.22)
project(gritty VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(GRITTY_USE_SYSTEM_DEPS "Use system tl-expected and mdspan instead of vendored copies" OFF)

add_library(gritty INTERFACE)
add_library(gritty::gritty ALIAS gritty)

target_include_directories(gritty INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>)
target_compile_features(gritty INTERFACE cxx_std_20)
target_compile_definitions(gritty INTERFACE MDSPAN_USE_PAREN_OPERATOR=1)   # every consumer must agree (ODR)

if(GRITTY_USE_SYSTEM_DEPS)
  find_package(tl-expected CONFIG REQUIRED)
  find_package(mdspan CONFIG REQUIRED)
  target_link_libraries(gritty INTERFACE tl::expected std::mdspan)
  target_compile_definitions(gritty INTERFACE GRITTY_USE_SYSTEM_DEPS=1)
endif()

install(TARGETS gritty EXPORT grittyTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(EXPORT grittyTargets NAMESPACE gritty::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

configure_package_config_file(cmake/grittyConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/grittyConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/grittyConfigVersion.cmake
  COMPATIBILITY SameMinorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/grittyConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/grittyConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
