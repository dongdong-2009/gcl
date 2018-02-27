
set(gsl_iec104_files
        iec104/iec104_protocol.h
        #SOURCES +=
        iec104/iec104_protocol.cpp
        )

mc_add_vxd_exe(gcl_iec104 "${gsl_iec104_files}")

