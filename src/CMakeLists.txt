FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

# Components this app calls into. IDF adds their public include dirs to our
# compile flags, so they also land in compile_commands.json for clangd.
idf_component_register(SRCS ${app_sources}
                       REQUIRES esp_adc esp_driver_gpio)
