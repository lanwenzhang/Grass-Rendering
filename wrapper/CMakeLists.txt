#add cpp to WRAPPER variable
file(GLOB_RECURSE WRAPPER ./ *.cpp)

#Compile WRAPPER to lib
add_library(wrapper ${WRAPPER})