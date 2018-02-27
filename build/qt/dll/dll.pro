TEMPLATE      = subdirs

SUBDIRS       = \
    gcl_ccxx \
    gcl_vdi \
    gcl_script \
    gcl_ga \               # dll
####gcl_sdk \               # dll, this do not use, now use gcl_ga instead of this
####gcl_sdk_main \         # exe
####gcl_share_memory \
    gcl_lua \
####can't compile    gcl_shm_sdk \
####can't compile   gcl_file_sys \
#    gcl_lfs \
#    gcl_lpeg \
    gcl_graphics \
    gcl_database_odbc \
    gcl_database_sqlite \
####    gcl_omc \             #do not use
####    gcl_net_monitor \     #do not use
    gcl_script_vxd \
