
###add qt ui
include(./../../../qt.cmake)

set(qt_sources
        sdk_qt/main.cpp
        sdk_qt/sdk_test_win.cpp
        sdk_qt/sdk_measure_win.cpp
        sdk_qt/sdk_common.cpp
        sdk_qt/cxglobal.cpp
        sdk_qt/cxstring.cpp
        sdk_qt/cxtime.cpp
        sdk_qt/cxfile.cpp
        sdk_qt/cxcrc.cpp
        )

set(qt_headers
        sdk_qt/sdk_test_win.h
        sdk_qt/sdk_measure_win.h
        sdk_qt/cxglobal.h
        sdk_qt/cxstring.h
        sdk_qt/cxtime.h
        sdk_qt/cxfile.h
        sdk_qt/cxcrc.h
        sdk_qt/sdk_common.h
        sdk_qt/gcl_common_sdk.h
        sdk_qt/psm_common.h
        sdk_qt/psm_struct.h
        )

set(qt_forms
        sdk_qt/sdktestwin.ui
        )

mc_merge_file_path(${gs_gcl_test_path} "${qt_sources}" qt_source_paths)
mc_merge_file_path(${gs_gcl_test_path} "${qt_headers}" qt_header_paths)
mc_merge_file_path(${gs_gcl_test_path} "${qt_forms}" qt_form_paths)

source_group("sdk_qt" FILES ${qt_source_paths} ${qt_header_paths} ${qt_form_paths})

add_executable(gcl_sdk_tool
        ${qt_source_paths} ${qt_header_paths} ${qt_form_paths}
        )

set_target_properties(gcl_sdk_tool PROPERTIES FOLDER gcl/test)

add_dependencies(gcl_sdk_tool
        gcl_sdk
        )

target_link_libraries(gcl_sdk_tool
        gcl_sdk
        ${Qt5Core_LIBRARIES}
        ${Qt5Widgets_LIBRARIES}
        ${Qt5Gui_LIBRARIES}
        )

