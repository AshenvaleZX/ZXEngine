cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(ZXEngine C CXX)

################################################################################
# Set target arch type.
################################################################################
if(WIN32)
    set(CMAKE_VS_PLATFORM_NAME "x64")
    message("${CMAKE_VS_PLATFORM_NAME} architecture in use")
endif()

if(MSVC)
    add_compile_options("/source-charset:utf-8" "/execution-charset:utf-8")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Common utils
################################################################################
include(Helper/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(Helper/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(ZXEngine)