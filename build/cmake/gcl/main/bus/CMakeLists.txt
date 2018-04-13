

mc_gcl_shm_include()
mc_gcl_vdi_daemon_heartbeat_include()

set(gsl_gcl_bus_files
        main/gcl_bus/bus_terminal.h
        main/gcl_bus/bus_data_deal.h
        main/gcl_bus/bus_measure_publish.h
        main/gcl_bus/bus_strings.h
        main/gcl_bus/bus_measure_control.h
        main/gcl_bus/bus_general_access.h
        main/gcl_bus/main.cpp
        main/gcl_bus/bus_terminal.cpp
        main/gcl_bus/bus_data_deal.cpp
        main/gcl_bus/bus_measure_publish.cpp
        main/gcl_bus/bus_measure_control.cpp
        main/gcl_bus/bus_general_access.cpp
        psm/psm_explain.h
        psm/psm_explain.cpp
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_bus_files}" gsl_gcl_bus_filepaths)

add_executable(gcl_svr_bus
        ${gsl_gcl_bus_filepaths}
        ${gsl_gcl_shm_filepaths}
        ${gsl_gcl_vdi_daemon_heartbeat_filepaths}
        )

add_dependencies(gcl_svr_bus
        libccxx
        )

target_link_libraries(gcl_svr_bus
        libccxx
        )

set_target_properties(gcl_svr_bus PROPERTIES FOLDER gcl/main)
