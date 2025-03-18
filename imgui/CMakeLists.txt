#add cpp to FW variable
file(GLOB_RECURSE IMGUI ./ *.cpp)

#Compile FW to lib
add_library(imguilib ${IMGUI})