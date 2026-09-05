FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

# Components this app calls into. IDF adds their public include dirs to our
# compile flags, so they also land in compile_commands.json for clangd.
# The lib/ modules are not listed: PlatformIO resolves those itself.
idf_component_register(SRCS ${app_sources}
                       REQUIRES esp_driver_gpio)
