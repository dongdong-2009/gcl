
mc_gcl_psm_include()
mc_gcl_shm_include()
mc_gcl_vdi_daemon_heartbeat_include()

set(gsl_gcl_rtdbs_files
        main/gcl_rtdbs/main.cpp
        main/gcl_rtdbs/rtdb_logfile.h
        main/gcl_rtdbs/rtdb_logfile.cpp
        main/gcl_rtdbs/rtdb_service.h
        main/gcl_rtdbs/rtdb_service.cpp
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_rtdbs_files}" gsl_gcl_rtdbs_filepaths)

add_executable(gcl_svr_rtdbs
        ${gsl_gcl_rtdbs_filepaths}
        ${gsl_gcl_shm_filepaths}
        ${gsl_gcl_psm_filepaths}
        ${gsl_gcl_vdi_daemon_heartbeat_filepaths}
        )

add_dependencies(gcl_svr_rtdbs
        libccxx
        )

target_link_libraries(gcl_svr_rtdbs
        libccxx
        )

set_target_properties(gcl_svr_rtdbs PROPERTIES FOLDER gcl/main)
