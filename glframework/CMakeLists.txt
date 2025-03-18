#add cpp to FW variable
file(GLOB_RECURSE FW ./ *.cpp)

#Compile FW to lib
add_library(fw ${FW})