
#1, psm
#2, shm
#3, vdi
#3.1, vdi.measure
#3.2, vdi.worker
#3.3, vdi.terminal
#3.4, vdi.protocol
#3.5, vdi.cycle
#3.6, vdi.task
#3.7, vdi.case
#3.8, vdi.business
#3.9, vdi.daemon_heartbeat
#3.10,vdi.fix_measure
#3.11,vdi.other
#3.12,vdi.gcl_config

set(gs_gcl_src_path ${gs_gcl_path}/src)

include_directories(${gs_gcl_src_path})


#1, psm
set(gsl_gcl_psm_files
        psm/psm_explain.h
        psm/general_access.h
        psm/psm_explain.cpp
        psm/general_access.cpp
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_psm_files}" gsl_gcl_psm_filepaths)

macro(mc_gcl_psm_include)
    source_group("gcl\\psm" FILES ${gsl_gcl_psm_filepaths})
endmacro()

#2, shm
set(gsl_gcl_shm_files
        shm/measure_sharememory.h
        shm/monsb_sharememory.h
        shm/ycadd_sharememory.h
        shm/straw_sharememory.h
        shm/fileinfo_sharememory.h
        shm/measure_sharememory_all.h
        shm/monsb_sharememory.cpp
        shm/ycadd_sharememory.cpp
        shm/straw_sharememory.cpp
        shm/fileinfo_sharememory.cpp
        shm/measure_sharememory_all.cpp
        )

mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_shm_files}" gsl_gcl_shm_filepaths)

macro(mc_gcl_shm_include)
    source_group("gcl\\shm" FILES ${gsl_gcl_shm_filepaths})
endmacro()

#3, vdi
set(gsl_gcl_vdi_business_files
        ###***business
        vdi/tbi_base.h
        vdi/cbi_base.h
        #SOURCES +=
        vdi/tbi_base.cpp
        vdi/tbi_factorymanager.cpp
        vdi/cbi_base.cpp
        vdi/cbi_factorymanager.cpp
        )

set(gsl_gcl_vdi_cases_files
        ###***cases
        vdi/cases_base.h
        vdi/send_control_cases_template.h
        vdi/send_monsb_control_cases.h
        vdi/send_measure_cases_template.h
        vdi/send_monsb_cases.h
        vdi/send_ycadd_cases.h
        vdi/send_message_cases.h
        vdi/send_file_cases.h
        vdi/send_custom_data_cases.h
        #SOURCES +=
        vdi/cases_base.cpp
        vdi/send_control_cases_template.cpp
        vdi/send_monsb_control_cases.cpp
        vdi/send_message_cases.cpp
        vdi/send_file_cases.cpp
        vdi/send_custom_data_cases.cpp
        )

set(gsl_gcl_vdi_daemon_heartbeat_files
        ###***daemon_heartbeat
        vdi/common_service.h
        #SOURCES +=
        vdi/common_service.cpp
        )

set(gsl_gcl_vdi_cycle_files
        ###***cycle
        vdi/cycle_base.h
        vdi/send_control_cycle_template.h
        vdi/send_monsb_control_cycle.h
        vdi/send_measure_cycle_template.h
        vdi/send_monsb_cycle.h
        vdi/send_ycadd_cycle.h
        #SOURCES +=
        vdi/cycle_base.cpp
        )

set(gsl_gcl_vdi_measure_files
        ###***measure
        vdi/measure_base.h
        vdi/measure_manager_template.h
        vdi/measure_template.h
        vdi/monsb_measure.h
        vdi/ycadd_measure.h
        vdi/straw_measure.h
        vdi/measure_manager_psm.h
        vdi/gcl_measure_server.h
        vdi/gcl_shm_server.h
        #SOURCES +=
        vdi/measure_base.cpp
        vdi/monsb_measure.cpp
        vdi/ycadd_measure.cpp
        vdi/straw_measure.cpp
        vdi/measure_manager_psm.cpp
        vdi/gcl_measure_server.cpp
        vdi/gcl_shm_server.cpp
        )

set(gsl_gcl_vdi_protocol_files
        ###***protocol
        vdi/protocol_global.h
        vdi/protocol_base.h
        vdi/protocol_common.h
        vdi/psm_protocol.h
        vdi/psm_manager.h
        #SOURCES +=
        vdi/protocol_base.cpp
        vdi/protocol_common.cpp
        vdi/psm_protocol.cpp
        vdi/protocol_factorymanager.cpp
        vdi/psm_manager.cpp
        )

set(gsl_gcl_vdi_task_files
        ###***task
        vdi/monsb_control.h
        vdi/monsb_control_task.h
        vdi/schedule_task.h
        vdi/schedule_task_manager.h
        #SOURCES +=
        vdi/monsb_control.cpp
        vdi/monsb_control_task.cpp
        vdi/schedule_task.cpp
        vdi/schedule_task_manager.cpp
        )

set(gsl_gcl_vdi_terminal_files
        ###***terminal
        vdi/terminal_base.h
        vdi/terminal_loader.h
        vdi/terminal_manager.h
        vdi/terminal_strings.h
        vdi/custom_terminal.h
        vdi/terminal_exe.h
        #SOURCES +=
        vdi/terminal_base.cpp
        vdi/terminal_loader.cpp
        vdi/terminal_manager.cpp
        vdi/terminal_factorymanager.cpp
        vdi/custom_terminal.cpp
        vdi/terminal_exe.cpp
        )

set(gsl_gcl_vdi_worker_files
        ###***worker
        vdi/worker_strings.h
        vdi/worker_base.h
        vdi/measure_address_worker.h
        vdi/address_worker_base.h
        vdi/ycadd_address_worker.h
        vdi/monsb_address_worker.h
        vdi/straw_address_worker.h
        #SOURCES +=
        vdi/worker_base.cpp
        vdi/address_worker_base.cpp
        vdi/ycadd_address_worker.cpp
        )

set(gsl_gcl_vdi_fix_measure_files
        ###***Fix Measure
        vdi/measure_application.h
        vdi/measure_application.cpp
        )

set(gsl_gcl_vdi_other_files
        ###***ga
        vdi/explain_xml.h
        #SOURCES +=
        vdi/explain_xml.cpp
        )

set(gsl_gcl_vdi_gcl_config_files
        ###***gcl_config
        #SOURCES +=
        common/gcl_config.cpp
        )

macro(mc_gcl_vdi_measure_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_measure_files}"          gsl_gcl_vdi_measure_filepaths          )
    source_group("gcl\\measure"          FILES ${gsl_gcl_vdi_measure_filepaths})
endmacro()
macro(mc_gcl_vdi_worker_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_worker_files}"           gsl_gcl_vdi_worker_filepaths           )
    source_group("gcl\\worker"           FILES ${gsl_gcl_vdi_worker_filepaths})
endmacro()
macro(mc_gcl_vdi_terminal_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_terminal_files}"         gsl_gcl_vdi_terminal_filepaths         )
    source_group("gcl\\terminal"         FILES ${gsl_gcl_vdi_terminal_filepaths})
endmacro()
macro(mc_gcl_vdi_protocol_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_protocol_files}"         gsl_gcl_vdi_protocol_filepaths         )
    source_group("gcl\\protocol"         FILES ${gsl_gcl_vdi_protocol_filepaths})
endmacro()
macro(mc_gcl_vdi_cycle_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_cycle_files}"            gsl_gcl_vdi_cycle_filepaths            )
    source_group("gcl\\cycle"            FILES ${gsl_gcl_vdi_cycle_filepaths})
endmacro()
macro(mc_gcl_vdi_task_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_task_files}"             gsl_gcl_vdi_task_filepaths             )
    source_group("gcl\\task"             FILES ${gsl_gcl_vdi_task_filepaths})
endmacro()
macro(mc_gcl_vdi_case_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_cases_files}"            gsl_gcl_vdi_case_filepaths             )
    source_group("gcl\\case"             FILES ${gsl_gcl_vdi_case_filepaths})
endmacro()
macro(mc_gcl_vdi_business_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_business_files}"         gsl_gcl_vdi_business_filepaths         )
    source_group("gcl\\business"         FILES ${gsl_gcl_vdi_business_filepaths})
endmacro()
macro(mc_gcl_vdi_daemon_heartbeat_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_daemon_heartbeat_files}" gsl_gcl_vdi_daemon_heartbeat_filepaths )
    source_group("gcl\\daemon_heartbeat" FILES ${gsl_gcl_vdi_daemon_heartbeat_filepaths})
endmacro()
macro(mc_gcl_vdi_fix_measure_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_fix_measure_files}"      gsl_gcl_vdi_fix_measure_filepaths      )
    source_group("gcl\\fix_measure"      FILES ${gsl_gcl_vdi_fix_measure_filepaths})
endmacro()
macro(mc_gcl_vdi_other_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_other_files}"            gsl_gcl_vdi_other_filepaths            )
    source_group("gcl\\other"            FILES ${gsl_gcl_vdi_other_filepaths})
endmacro()
macro(mc_gcl_vdi_gcl_config_include)
    mc_merge_file_path(${gs_gcl_src_path} "${gsl_gcl_vdi_gcl_config_files}"       gsl_gcl_vdi_gcl_config_filepaths       )
    source_group("gcl\\gcl_config"       FILES ${gsl_gcl_vdi_gcl_config_filepaths})
endmacro()
