
mc_gcl_vdi_measure_include()
mc_gcl_vdi_worker_include()
mc_gcl_vdi_terminal_include()
mc_gcl_vdi_protocol_include()
mc_gcl_vdi_cycle_include()
mc_gcl_vdi_task_include()
mc_gcl_vdi_case_include()
mc_gcl_vdi_business_include()
mc_gcl_vdi_daemon_heartbeat_include()
mc_gcl_vdi_fix_measure_include()
mc_gcl_vdi_other_include()
mc_gcl_vdi_gcl_config_include()
mc_gcl_psm_include()
mc_gcl_shm_include()

add_library(gcl_vdi SHARED
        ${gsl_gcl_vdi_measure_filepaths}
        ${gsl_gcl_vdi_worker_filepaths}
        ${gsl_gcl_vdi_terminal_filepaths}
        ${gsl_gcl_vdi_protocol_filepaths}
        ${gsl_gcl_vdi_cycle_filepaths}
        ${gsl_gcl_vdi_task_filepaths}
        ${gsl_gcl_vdi_case_filepaths}
        ${gsl_gcl_vdi_business_filepaths}
        ${gsl_gcl_vdi_daemon_heartbeat_filepaths}
        ${gsl_gcl_vdi_fix_measure_filepaths}
        ${gsl_gcl_vdi_other_filepaths}
        ${gsl_gcl_vdi_gcl_config_filepaths}
        ${gsl_gcl_psm_filepaths}
        ${gsl_gcl_shm_filepaths}
        )

add_dependencies(gcl_vdi
        libccxx
        )

target_link_libraries(gcl_vdi
        libccxx
        )

add_definitions(-D"GM_BUILD_GCL_VDI_LIB")
add_definitions(-D"GM_MEASURE_MANAGER")
add_definitions(-D"GM_GCL_SHAREMEMORY")

set_target_properties(gcl_vdi PROPERTIES FOLDER gcl/lib)

