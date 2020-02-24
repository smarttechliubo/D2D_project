cmake_minimum_required (VERSION 2.8)



project (D2D_project)


set(CMAKE_SYSTEM_NAME linux) 
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER "/usr/bin/arm-linux-gnueabihf-gcc")
add_definitions(-DFPGA_PLATFORM)

get_filename_component(PROJECT_ORIGIN_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

include_directories (${PROJECT_ORIGIN_DIR}/common/inc 
                     ${PROJECT_ORIGIN_DIR}/interface 
                     ${PROJECT_ORIGIN_DIR}/platform/ocp_itti 
	                 ${PROJECT_ORIGIN_DIR}/platform/osp_lib
		             ${PROJECT_ORIGIN_DIR}/protocol/MAC/inc
					 ${PROJECT_ORIGIN_DIR}/protocol/MAC/test/inc
                     ${PROJECT_ORIGIN_DIR}/protocol/RLC/inc 
                     ${PROJECT_ORIGIN_DIR}/protocol/RRC 
                     ${PROJECT_ORIGIN_DIR}/protocol/RRC/inc 
                     ${PROJECT_ORIGIN_DIR}/protocol/RRC/inc/asn1c_inc
                       )

add_definitions(-DASN_DISABLE_OER_SUPPORT)
add_definitions(-DUSE_SYSLOG)
add_definitions(-DUSING_OSP_LIB)


aux_source_directory (${PROJECT_ORIGIN_DIR}/common/src SRC_COMMON)
aux_source_directory (${PROJECT_ORIGIN_DIR}/platform/ocp_itti SRC_PLATFORM)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/MAC/src SRC_PROTOCOL_MAC)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/MAC/test/src SRC_PROTOCOL_PHY)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RRC SRC_PROTOCOL_RRC_DUMMY)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RRC/src SRC_PROTOCOL_RRC)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RRC/src/asn1c_src SRC_PROTOCOL_RRC_ASN)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RLC/src SRC_PROTOCOL_RLC)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/IP SRC_PROTOCOL_IP)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/IP SRC_PROTOCOL_IP)
LINK_LIBRARIES(-lpthread)
LINK_LIBRARIES(-lrt)


add_compile_options(-g)

option(LIB_D2D_SHARED "enable compile d2d_shared_lib" ON)

#set (EXECUTABLE_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
#message("SRC_COMMON = " ${SRC_COMMON})
#add_executable(D2D_project ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_RRC} ${SRC_PROTOCOL_RLC} ${SRC_PROTOCOL_IP} ${SRC_PROTOCOL_RRC_DUMMY} ${SRC_PROTOCOL_RRC_ASN})
if (LIB_D2D_SHARED) 
	message("------------------compile D2D_protocol_shared.so------------------------")
	set (LIBRARY_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
	add_library (D2D_protocol_shared SHARED ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_MAC} ${SRC_PROTOCOL_PHY} ${SRC_PROTOCOL_RRC} ${SRC_PROTOCOL_RLC} ${SRC_PROTOCOL_IP} ${SRC_PROTOCOL_RRC_DUMMY} ${SRC_PROTOCOL_RRC_ASN})
else ( )
	message("------------------compile executed file: D2D_protocol------------------------")
	link_directories (${PROJECT_ORIGIN_DIR}/platform/osp_lib)
	
 	LINK_LIBRARIES(-losp)
	LINK_LIBRARIES(-lm)
	LINK_LIBRARIES(-ldl)
	set (EXECUTABLE_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
	add_executable(D2D_project ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_MAC} ${SRC_PROTOCOL_PHY} ${SRC_PROTOCOL_RRC} ${SRC_PROTOCOL_RLC} ${SRC_PROTOCOL_IP} ${SRC_PROTOCOL_RRC_DUMMY} ${SRC_PROTOCOL_RRC_ASN})
	
	
endif ()
############################# output exe#####################
#set (EXECUTABLE_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
#message("SRC_COMMON = " ${SRC_COMMON})
#add_executable(D2D_project ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_RRC} ${SRC_PROTOCOL_RLC} ${SRC_PROTOCOL_IP} ${SRC_PROTOCOL_RRC_DUMMY} ${SRC_PROTOCOL_RRC_ASN})
