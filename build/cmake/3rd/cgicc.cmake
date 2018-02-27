
set(gs_3rd_cgicc_path ${gs_project_3rd_path}/cgicc)

set(gsl_3rd_cgicc_files
        cgicc/Cgicc.cpp
        cgicc/CgiEnvironment.cpp
        cgicc/CgiInput.cpp
        cgicc/CgiUtils.cpp
        cgicc/FormEntry.cpp
        cgicc/FormFile.cpp
        cgicc/HTMLAttribute.cpp
        cgicc/HTMLAttributeList.cpp
        cgicc/HTMLDoctype.cpp
        cgicc/HTMLElement.cpp
        cgicc/HTMLElementList.cpp
        cgicc/HTTPContentHeader.cpp
        cgicc/HTTPCookie.cpp
        cgicc/HTTPHeader.cpp
        cgicc/HTTPHTMLHeader.cpp
        cgicc/HTTPPlainHeader.cpp
        cgicc/HTTPRedirectHeader.cpp
        cgicc/HTTPResponseHeader.cpp
        cgicc/HTTPStatusHeader.cpp
        cgicc/HTTPXHTMLHeader.cpp
        cgicc/MStreamable.cpp
        cgicc/XHTMLDoctype.cpp
        cgicc/XMLPI.cpp
        contrib/FCgiIO.cpp
        #HEADERS +=
        cgicc/Cgicc.h
        cgicc/CgiDefs.h
        cgicc/CgiEnvironment.h
        cgicc/CgiInput.h
        cgicc/CgiUtils.h
        cgicc/FormEntry.h
        cgicc/FormFile.h
        cgicc/HTMLAtomicElement.h
        cgicc/HTMLAttribute.h
        cgicc/HTMLAttributeList.h
        cgicc/HTMLBooleanElement.h
        cgicc/HTMLClasses.h
        cgicc/HTMLDoctype.h
        cgicc/HTMLElement.h
        cgicc/HTMLElementList.h
        cgicc/HTTPContentHeader.h
        cgicc/HTTPCookie.h
        cgicc/HTTPHeader.h
        cgicc/HTTPHTMLHeader.h
        cgicc/HTTPPlainHeader.h
        cgicc/HTTPRedirectHeader.h
        cgicc/HTTPResponseHeader.h
        cgicc/HTTPStatusHeader.h
        cgicc/HTTPXHTMLHeader.h
        cgicc/MStreamable.h
        cgicc/XHTMLDoctype.h
        cgicc/XMLDeclaration.h
        cgicc/XMLPI.h
        contrib/FCgiIO.h
        )

mc_merge_file_path(${gs_3rd_cgicc_path} "${gsl_3rd_cgicc_files}" gsl_3rd_cgicc_filepaths)

macro(mc_3rd_cgicc_include)
    include_directories(${gs_3rd_cgicc_path})
    source_group("3rd\\cgicc" FILES ${gsl_3rd_cgicc_filepaths})
endmacro()
