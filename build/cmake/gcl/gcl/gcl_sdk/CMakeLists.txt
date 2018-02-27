
set(gsl_ga_api_heads
        ga/ga_api.h
        ga/gcl_ga.h
        )
set(gsl_ga_api_sources
        ga/gcl_ga.cpp
        ga/ga_api.cpp
        )
mc_merge_file_path(${gs_gcl_src_path} "${gsl_ga_api_heads}" gsl_ga_api_head_filepaths)
mc_merge_file_path(${gs_gcl_src_path} "${gsl_ga_api_sources}" gsl_ga_api_source_filepaths)
source_group("ga_api" FILES ${gsl_ga_api_head_filepaths} ${gsl_ga_api_source_filepaths})
add_definitions(-D"GCI_API_IMPL_EXPORT")

mc_ccxx_include()
mc_gcl_psm_include()
mc_gcl_shm_include()
mc_gcl_vdi_fix_measure_include()

#use cc4000 's path construction
add_definitions(-D"GM_BUILD_CCXX_CORE_CODE")
add_definitions(-D"PROJECT_PLATFORM_CC4000")

add_library(gcl_sdk SHARED
        ${gsl_ga_api_head_filepaths} ${gsl_ga_api_source_filepaths}
        ${gs_ccxx_head_filepaths} ${gsl_ccxx_source_filepaths}
        ${gsl_gcl_psm_filepaths}
        ${gsl_gcl_shm_filepaths}
        ${gsl_gcl_vdi_fix_measure_filepaths}
        )

if (MSVC)
    add_definitions(-D"_CRT_SECURE_NO_WARNINGS")
    add_definitions( "/wd4819" )
endif ()

if (WIN32)
    target_link_libraries(gcl_sdk wsock32 ws2_32 psapi)
endif ()

if (UNIX)
    target_link_libraries(gcl_sdk dl rt uuid)
endif ()

set_target_properties(gcl_sdk PROPERTIES FOLDER gcl/lib)

#mc_post_build_copyfile(gcl_sdk)
