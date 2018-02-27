
set(gsl_gcl_lua_ccxx_heads
        script/gcl_script.h
        script/gcl_script_global.h
        script/gcl_lua_global.h
        script/gcl_lua_efile.h
        script/gcl_lua_xml.h
        script/gcl_lua_base.h
        script/gcl_lua_common.h
        script/ccxx_lua_application.h
        script/ccxx_lua_filesystem.h
        script/ccxx_lua_time.h
        script/gcl_lua_fs.h
        script/gcl_lua_peg.h
        )

set(gsl_gcl_lua_ccxx_sources
        script/gcl_script.cpp
        script/gcl_lua_global.cpp
        script/gcl_lua_efile.cpp
        script/gcl_lua_xml.cpp
        script/gcl_lua_base.cpp
        script/gcl_lua_common.cpp
        script/ccxx_lua_application.cpp
        script/ccxx_lua_filesystem.cpp
        script/ccxx_lua_time.cpp
        script/gcl_lua_fs.cpp
        script/gcl_lua_peg.cpp
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_lua_ccxx_heads}" gsl_gcl_lua_ccxx_head_filepaths)
mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_lua_ccxx_sources}" gsl_gcl_lua_ccxx_source_filepaths)
source_group("ccxx_lua" FILES ${gsl_gcl_lua_ccxx_head_filepaths} ${gsl_gcl_lua_ccxx_source_filepaths})

set(gsl_gcl_lua_vdi_heads
        script/gcl_script_vxd.h
        script/gcl_lua_measure.h
        script/gcl_lua_ga.h
        script/gcl_lua_db.h
        script/gcl_lua_vxd.h
        script/gcl_lua_psm.h
        script/gcl_lua_shm.h
        script/gcl_lua_business.h
        script/scriptproxy.h
        script/gcl_lua_code.h
        script/ccxx_lua_tring.h
#        script/gcl_lua_omc.h
        script/gcl_lua_net_monitor.h
        )
set(gsl_gcl_lua_vdi_sources
        script/gcl_script_vxd.cpp
        script/gcl_lua_measure.cpp
        script/gcl_lua_ga.cpp
        script/gcl_lua_db.cpp
        script/gcl_lua_vxd.cpp
        script/gcl_lua_psm.cpp
        script/gcl_lua_shm.cpp
        script/gcl_lua_business.cpp
        script/scriptproxy.cpp
        script/gcl_lua_code.cpp
        script/ccxx_lua_tring.cpp
#        script/gcl_lua_omc.cpp
        script/gcl_lua_net_monitor.cpp
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_lua_vdi_heads}" gsl_gcl_lua_vdi_head_filepaths)
mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_lua_vdi_sources}" gsl_gcl_lua_vdi_source_filepaths)
source_group("gcl_lua" FILES ${gsl_gcl_lua_vdi_head_filepaths} ${gsl_gcl_lua_vdi_source_filepaths})

include_directories(${gs_project_basic_path})

add_library(gcl_lua_vdi SHARED
        ${gsl_lua_header_filepaths} ${gsl_lua_source_filepaths}
        ${gsl_gcl_lua_ccxx_head_filepaths} ${gsl_gcl_lua_ccxx_source_filepaths}
        ${gsl_gcl_lua_vdi_head_filepaths} ${gsl_gcl_lua_vdi_source_filepaths}
        )


add_dependencies(gcl_lua_vdi
        libccxx
        libccxx_database_odbc
        libccxx_database_sqlite
        gcl_vdi
#        OmcApi
        )

target_link_libraries(gcl_lua_vdi
        libccxx
        libccxx_database_odbc
        libccxx_database_sqlite
        gcl_vdi
#        OmcApi
        )

mc_ccxx_lua_include()

set_target_properties(gcl_lua_vdi PROPERTIES FOLDER gcl/lib)
