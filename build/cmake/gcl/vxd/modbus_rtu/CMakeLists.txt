
set(gsl_modbus_rtu_files
        modbus/modbus_global.h
        modbus/modbus_common.h
        modbus/modbus_asdu.h
        modbus/modbus_rtu.h
        #SOURCES +=
        modbus/modbus_common.cpp
        modbus/modbus_asdu.cpp
        modbus/modbus_rtu.cpp
        )

mc_add_vxd_exe(gcl_modbus_rtu "${gsl_modbus_rtu_files}")

