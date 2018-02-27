
set(gs_vxd_path ${gs_gcl_src_path}/vxd)

set(gsl_vxd_files
        vxd/vxdcommon.h
        #SOURCES +=
        vxd/vxdcommon.cpp
        vxd/vxdmain.cpp
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_vxd_files}" gsl_vxd_filepaths)

macro(mc_add_vxd_exe sExeName sFileNames)
    mc_merge_file_path(${gs_vxd_path} "${sFileNames}" gsl_vxd_logic_filepaths)

    source_group("gcl_vxd" FILES ${gsl_vxd_filepaths})
    source_group("gcl_vxd_logic" FILES ${gsl_vxd_logic_filepaths})

    add_executable(${sExeName}
            ${gsl_vxd_filepaths}
            ${gsl_vxd_logic_filepaths}
            )

    add_dependencies(${sExeName}
            libccxx
            gcl_vdi
            gcl_lua_vdi
            )

    target_link_libraries(${sExeName}
            libccxx
            gcl_vdi
            gcl_lua_vdi
            )

    set_target_properties(${sExeName} PROPERTIES FOLDER gcl/vxd)
endmacro()

add_subdirectory(iec104)
add_subdirectory(modbus_rtu)
add_subdirectory(modbus_tcp)
add_subdirectory(vxd_frame)

