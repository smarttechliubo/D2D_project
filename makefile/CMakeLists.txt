cmake_minimum_required (VERSION 2.8)


project (D2D_project)
 

get_filename_component(PROJECT_ORIGIN_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)

include_directories (${PROJECT_ORIGIN_DIR}/common/inc 
                     ${PROJECT_ORIGIN_DIR}/interface 
                     ${PROJECT_ORIGIN_DIR}/platform/ocp_itti 
                     ${PROJECT_ORIGIN_DIR}/protocol/RLC/inc 
                     ${PROJECT_ORIGIN_DIR}/protocol/RRC 
                     ${PROJECT_ORIGIN_DIR}/protocol/RRC/inc 
                     ${PROJECT_ORIGIN_DIR}/protocol/RRC/inc/asn1c_inc  )

add_definitions(-DASN_DISABLE_OER_SUPPORT)
add_definitions(-DUSE_SYSLOG)
add_definitions(-DRLC_UT_DEBUG)

aux_source_directory (${PROJECT_ORIGIN_DIR}/common/src SRC_COMMON)
aux_source_directory (${PROJECT_ORIGIN_DIR}/platform/ocp_itti SRC_PLATFORM)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RRC SRC_PROTOCOL_RRC_DUMMY)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RRC/src SRC_PROTOCOL_RRC)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RRC/src/asn1c_src SRC_PROTOCOL_RRC_ASN)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/RLC/src SRC_PROTOCOL_RLC)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/IP SRC_PROTOCOL_IP)
aux_source_directory (${PROJECT_ORIGIN_DIR}/protocol/IP SRC_PROTOCOL_IP)
LINK_LIBRARIES(-lpthread)
LINK_LIBRARIES(-lrt)
add_compile_options(-g)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_ORIGIN_DIR}/target)
message("SRC_COMMON = " ${SRC_COMMON})
add_executable(D2D_project ${SRC_COMMON} ${SRC_PLATFORM} ${SRC_PROTOCOL_RRC} ${SRC_PROTOCOL_RLC} ${SRC_PROTOCOL_IP} ${SRC_PROTOCOL_RRC_DUMMY} ${SRC_PROTOCOL_RRC_ASN})


