
set(gs_3rd_fcgi_path ${gs_project_3rd_path}/fcgi)
set(gs_3rd_fcgi_include_path ${gs_project_3rd_path}/fcgi/include)

set(gsl_3rd_fcgi_files
        include/fastcgi.h
        include/fcgi_config.h
        include/fcgi_stdio.h
        include/fcgiapp.h
        include/fcgimisc.h
        include/fcgio.h
        include/fcgios.h
        #SOURCES +=
        libfcgi/fcgio.cpp
        libfcgi/fcgi_stdio.c
        libfcgi/fcgiapp.c
        )

if (WIN32)
    set(gsl_3rd_fcgi_files
            ${gsl_3rd_fcgi_files}
            libfcgi/os_win32.c
            )
else ()
    set(gsl_3rd_fcgi_files
            ${gsl_3rd_fcgi_files}
            libfcgi/os_unix.c
            )
endif ()

mc_merge_file_path(${gs_3rd_fcgi_path} "${gsl_3rd_fcgi_files}" gsl_3rd_fcgi_filepaths)

macro(mc_3rd_fcgi_include)
    include_directories(${gs_3rd_fcgi_include_path})
    source_group("3rd\\fcgi" FILES ${gsl_3rd_fcgi_filepaths})
endmacro()
