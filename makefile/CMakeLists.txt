cmake_minimum_required (VERSION 2.8)


project (D2D_project)
 

get_filename_component(PROJECT_ORIGIN_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

include_directories (${PROJECT_ORIGIN_DIR}/common/inc 
                     ${PROJECT_ORIGIN_DIR}/interface 
                     ${PROJECT_ORIGIN_DIR}/platform/ocp_itti 
					 ${PROJECT_ORIGIN_DIR}/platform/osp_lib
                     ${PROJECT_ORIGIN_DIR}/protocol/MAC/inc 
                     )

#add_definitions(-D)


aux_source_directory (${PROJECT_ORIGIN_DIR}/common/src SRC_COMMON)
aux_source_directory (${PROJECT_ORIGIN_DIR}/platform/ocp_itti/intertask_interface.c SRC_PLATFORM)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/MAC/src SRC_PROTOCOL_MAC)

LINK_LIBRARIES(-lpthread)
LINK_LIBRARIES(-lrt)


add_compile_options(-g)

#option(LIB_D2D_SHARED "enable compile d2d_shared_lib" ON)

#set (EXECUTABLE_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)

#add_executable(D2D_project ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_MAC})

message("------------------compile D2D_protocol_shared.so------------------------")
set (LIBRARY_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
add_library (D2D_protocol_shared SHARED ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_MAC})

#message("------------------compile executed file: D2D_protocol------------------------")
#link_directories (${PROJECT_ORIGIN_DIR}/platform/osp_lib)

#LINK_LIBRARIES(-losp)
#LINK_LIBRARIES(-lm)
#LINK_LIBRARIES(-ldl)
#set (EXECUTABLE_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
#add_executable(D2D_project ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_MAC})
