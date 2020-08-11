cmake_minimum_required (VERSION 2.8)


project (D2D_project)
 

<<<<<<< HEAD
=======
set(CMAKE_SYSTEM_NAME linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER "/usr/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_SYSTEM_PROCESSOR arm64)
set(CMAKE_C_COMPILER "/usr/bin/aarch64-linux-gnu-gcc")
#add_definitions(-DFPGA_PLATFORM)


option(address "This is a default option for RRC_DST" OFF)

if (${RRC_DST} STREQUAL "OFF")
	 message("------The RRC is SOURCE MODE ------------")
	 add_definitions(-DRRC_SOURCE)
else(${RRC_DST} STREQUAL "ON")
     message("------The RRC is  DESTINATION MODE ------------")
endif(${RRC_DST} STREQUAL "OFF")


option(address "This is a default option for RRC_RLC_UT" OFF)

if (${RRC_RLC_UT} STREQUAL "OFF")
	message("------The RRC_RLC_UT is OFF------------")
else(${RRC_RLC_UT} STREQUAL "ON")
	add_definitions(-DRLC_ONLY_DC_TEST)
	add_definitions(-DRLC_UT_DEBUG)
	add_definitions(-DRLC_RX_UT)
    message("------The RRC_RLC_UT is ON-------------")
endif(${RRC_DST} STREQUAL "OFF")


>>>>>>> master
get_filename_component(PROJECT_ORIGIN_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

include_directories (${PROJECT_ORIGIN_DIR}/common/inc 
                     ${PROJECT_ORIGIN_DIR}/interface 
<<<<<<< HEAD
                     ${PROJECT_ORIGIN_DIR}/platform/ocp_itti 
					 ${PROJECT_ORIGIN_DIR}/platform/osp_lib
                     ${PROJECT_ORIGIN_DIR}/protocol/MAC/inc 
                     )

#add_definitions(-D)


aux_source_directory (${PROJECT_ORIGIN_DIR}/common/src SRC_COMMON)
aux_source_directory (${PROJECT_ORIGIN_DIR}/platform/ocp_itti/intertask_interface.c SRC_PLATFORM)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/MAC/src SRC_PROTOCOL_MAC)

=======
                     ${PROJECT_ORIGIN_DIR}/platform/ocp_itti 
	             ${PROJECT_ORIGIN_DIR}/platform/osp_lib
		     ${PROJECT_ORIGIN_DIR}/protocol/MAC/inc
		     #${PROJECT_ORIGIN_DIR}/protocol/MAC/test/inc
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
#aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/MAC/test/src SRC_PROTOCOL_PHY)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RRC SRC_PROTOCOL_RRC_DUMMY)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RRC/src SRC_PROTOCOL_RRC)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RRC/src/asn1c_src SRC_PROTOCOL_RRC_ASN)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RLC/src SRC_PROTOCOL_RLC)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/IP SRC_PROTOCOL_IP)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/IP SRC_PROTOCOL_IP)
>>>>>>> master
LINK_LIBRARIES(-lpthread)
LINK_LIBRARIES(-lrt)


add_compile_options(-g)

<<<<<<< HEAD
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
=======
option(LIB_D2D_SHARED "enable compile d2d_shared_lib" ON)

#set (EXECUTABLE_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
#message("SRC_COMMON = " ${SRC_COMMON})
#add_executable(D2D_project ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_RRC} ${SRC_PROTOCOL_RLC} ${SRC_PROTOCOL_IP} ${SRC_PROTOCOL_RRC_DUMMY} ${SRC_PROTOCOL_RRC_ASN})
if (LIB_D2D_SHARED) 
	message("------------------compile D2D_protocol_shared.so------------------------")
	set (LIBRARY_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
	add_library (D2D_protocol_shared SHARED ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_MAC} ${SRC_PROTOCOL_RRC} ${SRC_PROTOCOL_RLC} ${SRC_PROTOCOL_IP} ${SRC_PROTOCOL_RRC_DUMMY} ${SRC_PROTOCOL_RRC_ASN})
else ( )
	message("------------------compile executed file: D2D_protocol------------------------")
	link_directories (${PROJECT_ORIGIN_DIR}/platform/osp_lib)
	
 	LINK_LIBRARIES(-losp)
	LINK_LIBRARIES(-lm)
	LINK_LIBRARIES(-ldl)
	set (EXECUTABLE_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
	add_executable(D2D_project ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_MAC} ${SRC_PROTOCOL_RRC} ${SRC_PROTOCOL_RLC} ${SRC_PROTOCOL_IP} ${SRC_PROTOCOL_RRC_DUMMY} ${SRC_PROTOCOL_RRC_ASN})
	
	
endif ()
############################# output exe#####################
#set (EXECUTABLE_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
#message("SRC_COMMON = " ${SRC_COMMON})
#add_executable(D2D_project ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_RRC} ${SRC_PROTOCOL_RLC} ${SRC_PROTOCOL_IP} ${SRC_PROTOCOL_RRC_DUMMY} ${SRC_PROTOCOL_RRC_ASN})
>>>>>>> master
