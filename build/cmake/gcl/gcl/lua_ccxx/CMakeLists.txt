
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
        common/gcl_config.cpp
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_lua_ccxx_heads}" gsl_gcl_lua_ccxx_head_filepaths)
mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_lua_ccxx_sources}" gsl_gcl_lua_ccxx_source_filepaths)
source_group("ccxx_lua" FILES ${gsl_gcl_lua_ccxx_head_filepaths} ${gsl_gcl_lua_ccxx_source_filepaths})

mc_ccxx_lua_include()

add_library(gcl_lua_ccxx SHARED
        ${gsl_lua_header_filepaths} ${gsl_lua_source_filepaths}
        ${gsl_gcl_lua_ccxx_head_filepaths} ${gsl_gcl_lua_ccxx_source_filepaths}
        )

add_dependencies(gcl_lua_ccxx
        libccxx
        )

target_link_libraries(gcl_lua_ccxx
        libccxx
        )

mc_ccxx_lua_include()

set_target_properties(gcl_lua_ccxx PROPERTIES FOLDER gcl/lib)
