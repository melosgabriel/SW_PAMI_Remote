# Create an interface/static library called lib_stepper
add_library(lib_stepper STATIC
    stepper.c
)

# Tell CMake to compile stepper.pio into stepper.pio.h specifically for this library
pico_generate_pio_header(lib_stepper ${CMAKE_CURRENT_LIST_DIR}/stepper.pio)

# Tell CMake where the header files (.h) for this library live
target_include_directories(lib_stepper PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link the standard Pico hardware dependencies required by the library
target_link_libraries(lib_stepper 
    pico_stdlib 
    hardware_pio
)