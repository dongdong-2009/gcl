

include(./../../../3rd/cgicc.cmake)
include(./../../../3rd/fcgi.cmake)

mc_3rd_cgicc_include()
mc_3rd_fcgi_include()
mc_gcl_psm_include()
mc_gcl_vdi_daemon_heartbeat_include()

set(gsl_fastcgi_files
        main/fastcgi/main_filesystem.cpp
        main/fastcgi/readme.md
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_fastcgi_files}" gsl_fastcgi_filepaths)

add_executable(gcl_svr_fastcgi_filesystem
        ${gsl_fastcgi_filepaths}
        ${gsl_3rd_cgicc_filepaths}
        ${gsl_3rd_fcgi_filepaths}
        ${gsl_gcl_psm_filepaths}
        ${gsl_gcl_vdi_daemon_heartbeat_filepaths}
        )

add_dependencies(gcl_svr_fastcgi_filesystem
        libccxx
        libccxx_database_odbc
        libccxx_database_sqlite
        )

target_link_libraries(gcl_svr_fastcgi_filesystem
        libccxx
        libccxx_database_odbc
        libccxx_database_sqlite
        )

set_target_properties(gcl_svr_fastcgi_filesystem PROPERTIES FOLDER gcl/main)
