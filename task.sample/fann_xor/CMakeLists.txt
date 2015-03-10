add_executable(fann_xor src/fann_xor.cpp)
target_link_libraries(fann_xor floatfann)

add_executable(fann_digits_cascade src/fann_digits_cascade.cpp)
target_link_libraries(fann_digits_cascade floatfann)
