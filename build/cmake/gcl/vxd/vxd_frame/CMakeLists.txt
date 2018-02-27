
set(gsl_vxd_frame_files
        vxd_frame/trans_protocol.h
        #SOURCES +=
        vxd_frame/trans_protocol.cpp
        )

mc_add_vxd_exe(gcl_vxd_frame "${gsl_vxd_frame_files}")

