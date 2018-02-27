
set(gsl_modbus_tcp_files
        modbus/modbus_global.h
        modbus/modbus_common.h
        modbus/modbus_asdu.h
        modbus/modbus_tcp_head.h
        modbus/modbus_tcp.h
        #SOURCES +=
        modbus/modbus_common.cpp
        modbus/modbus_asdu.cpp
        modbus/modbus_tcp_head.cpp
        modbus/modbus_tcp.cpp
        )

mc_add_vxd_exe(gcl_modbus_tcp "${gsl_modbus_tcp_files}")

