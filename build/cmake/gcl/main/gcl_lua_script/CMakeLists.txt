
set(gsl_script_main_files
        main/script/main_script_vxd.cpp
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_script_main_files}" gsl_script_main_filepaths)

source_group("gcl_script_main" FILES ${gsl_script_main_filepaths})

add_executable(gcl_svr_script_vxd
        ${gsl_script_main_filepaths}
        )

add_dependencies(gcl_svr_script_vxd
        libccxx
        libccxx_database_odbc
        libccxx_database_sqlite
        gcl_vdi
        gcl_lua_vdi
        )

target_link_libraries(gcl_svr_script_vxd
        libccxx
        libccxx_database_odbc
        libccxx_database_sqlite
        gcl_vdi
        gcl_lua_vdi
        )

set_target_properties(gcl_svr_script_vxd PROPERTIES FOLDER gcl/main)
