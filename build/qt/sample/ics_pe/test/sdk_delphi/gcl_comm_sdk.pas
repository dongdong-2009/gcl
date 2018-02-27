unit gcl_comm_sdk;

interface

type

  gci_fn_error_t = procedure(iErrorId : Integer); stdcall;
  gci_fn_message_command_t = procedure(const sCommand : PAnsiChar; const sParam : PAnsiChar; iParamLength : Integer; const sContainerCode : PAnsiChar; iTag : Integer); stdcall;
  gci_fn_file_transfer_t = procedure(const sFilePath : PAnsiChar; iTag : Integer); stdcall;
  gci_fn_realtime_container_data_t = procedure(const sContainerCode : PAnsiChar; iTag : Integer); stdcall;


  gct_firstaddress_t = Record
    addressBegin : Integer;
    addressEnd : Integer;
  END;
  gct_firstaddress_p=^gct_firstaddress_t;

  //*yx
  gct_address_vqt_int_text1_t = Record
    address : Integer;
    quality : Integer;
    datetime : double;
    value : array[0..1] of CHAR;
  END;
  gct_address_vqt_int_text1_p=^gct_address_vqt_int_text1_t;

  gct_vqt_int_text1_t = Record
    quality : Integer;
    datetime : double;
    value : array[0..1] of CHAR;
  END;
  gct_vqt_int_text1_p=^gct_vqt_int_text1_t;

  //*yc
  gct_address_vqt_real_text15_t = Record
      address : Integer;
      quality : Integer;
      datetime : double;
      value : array[0..15] of CHAR;
  END;
  gct_address_vqt_real_text15_p=^gct_address_vqt_real_text15_t;

  gct_vqt_real_text15_t = Record
      quality : Integer;
      datetime : double;
      value : array[0..15] of CHAR;
  END;
  gct_vqt_real_text15_p=^gct_vqt_real_text15_t;



const
  gcl_sdk_dll  = 'gcl_sdk.dll';

function gci_init(argc : integer; argv : array of PAnsiChar): integer ; stdcall ; external gcl_sdk_dll name '_gci_init@8';
function gci_cleanup(): integer ; stdcall ; external gcl_sdk_dll name '_gci_cleanup@0';
function gci_test(iTest : integer): integer ; stdcall ; external gcl_sdk_dll name '_gci_test@4';

function gci_last_error_id(): integer ; stdcall ; external gcl_sdk_dll name '_gci_last_error_id@0';
function gci_last_error_info(const pInfo : PAnsiChar; iLength : PInteger): integer ; stdcall ; external gcl_sdk_dll name '_gci_last_error_info@8';
function gci_error_msg(iErrorId : Integer = 0): PAnsiChar ; stdcall ; external gcl_sdk_dll name '_gci_error_msg@4';

function gci_connect(const sInFilePath : PAnsiChar; sOutFilePath : PAnsiChar): integer ; stdcall ; external gcl_sdk_dll name '_gci_connect@8';
function gci_disconnect(): integer ; stdcall ; external gcl_sdk_dll name '_gci_disconnect@0';
function gci_is_connected(): integer ; stdcall ; external gcl_sdk_dll name '_gci_is_connected@0';
function gci_connected_active_time(iTag : integer = 0): integer ; stdcall ; external gcl_sdk_dll name '_gci_connected_active_time@4';
function gci_set_default_containercode(const sContainerCode : PAnsiChar): integer ; stdcall ; external gcl_sdk_dll name '_gci_set_default_containercode@4';

function gci_message_command_send(const sCommand : PAnsiChar; const sParam : PAnsiChar = nil; iParamLength : Integer = 0; const sContainerCode : PAnsiChar = nil; iTag : Integer = 0): integer ; stdcall ; external gcl_sdk_dll name '_gci_message_command_send@20';
function gci_message_command_response(const sCommand : PAnsiChar; const sParam : PAnsiChar = nil; iParamLength : Integer = 0; const sContainerCode : PAnsiChar = nil; iTag : Integer = 0): integer ; stdcall ; external gcl_sdk_dll name '_gci_message_command_response@20';

function gci_register_error_notify(fn : gci_fn_error_t): integer ; stdcall ; external gcl_sdk_dll name '_gci_register_error_notify@4';
function gci_register_file_transfer_notify(fn : gci_fn_file_transfer_t): integer ; stdcall ; external gcl_sdk_dll name '_gci_register_file_transfer_notify@4';
function gci_register_message_command_notify(fn : gci_fn_message_command_t): integer ; stdcall ; external gcl_sdk_dll name '_gci_register_message_command_notify@4';
function gci_register_realtime_container_data_notify(fn : gci_fn_realtime_container_data_t): integer ; stdcall ; external gcl_sdk_dll name '_gci_register_realtime_container_data_notify@4';

function gci_unregister_realtime_container_data_notify(fn : gci_fn_realtime_container_data_t): integer ; stdcall ; external gcl_sdk_dll name '_gci_unregister_realtime_container_data_notify@4';
function gci_unregister_error_notify(fn : gci_fn_error_t): integer ; stdcall ; external gcl_sdk_dll name '_gci_unregister_error_notify@4';
function gci_unregister_file_transfer_notify(fn : gci_fn_file_transfer_t): integer ; stdcall ; external gcl_sdk_dll name '_gci_unregister_file_transfer_notify@4';
function gci_unregister_message_command_notify(fn : gci_fn_message_command_t): integer ; stdcall ; external gcl_sdk_dll name '_gci_unregister_message_command_notify@4';

function gci_user_is_logined(const sUserId : PAnsiChar): integer ; stdcall ; external gcl_sdk_dll name '_gci_user_is_logined@4';
function gci_user_login(const sInCardId : PAnsiChar; sOutUserId : PAnsiChar): integer ; stdcall ; external gcl_sdk_dll name '_gci_user_login@8';
function gci_user_logout(const sUserId : PAnsiChar): integer ; stdcall ; external gcl_sdk_dll name '_gci_user_logout@4';

function gci_file_transfer_response(const sFilePath : PAnsiChar; iTag : integer = 0): integer ; stdcall ; external gcl_sdk_dll name '_gci_file_transfer_response@8';
function gci_file_transfer_send(const sFilePath : PAnsiChar; iTag : integer = 0): integer ; stdcall ; external gcl_sdk_dll name '_gci_file_transfer_send@8';

function gci_yx_address_vqt_text1_send(const pData : gct_address_vqt_int_text1_p; iCount : Integer; sContainerCode : PAnsiChar = nil): integer ; stdcall ; external gcl_sdk_dll name '_gci_yx_address_vqt_text1_send@12';
function gci_yx_address_vqt_text1_response(const pData : gct_address_vqt_int_text1_p; iCount : Integer; sContainerCode : PAnsiChar = nil): integer ; stdcall ; external gcl_sdk_dll name '_gci_yx_address_vqt_text1_response@12';
function gci_yx_firstaddress_vqt_text1_send(firstAddress : gct_firstaddress_t; const pData : gct_vqt_int_text1_p; iCount : Integer; sContainerCode : PAnsiChar = nil): integer ; stdcall ; external gcl_sdk_dll name '_gci_yx_firstaddress_vqt_text1_send@20';
function gci_yx_firstaddress_vqt_text1_response(firstAddress : gct_firstaddress_t; const pData : gct_vqt_int_text1_p; iCount : Integer; sContainerCode : PAnsiChar = nil): integer ; stdcall ; external gcl_sdk_dll name '_gci_yx_firstaddress_vqt_text1_response@20';

function gci_yc_address_vqt_text15_send(const pData : gct_address_vqt_real_text15_p; iCount : Integer; sContainerCode : PAnsiChar = nil): integer ; stdcall ; external gcl_sdk_dll name '_gci_yc_address_vqt_text15_send@12';
function gci_yc_address_vqt_text15_response(const pData : gct_address_vqt_real_text15_p; iCount : Integer; sContainerCode : PAnsiChar = nil): integer ; stdcall ; external gcl_sdk_dll name '_gci_yc_address_vqt_text15_response@12';
function gci_yc_firstaddress_vqt_text15_send(firstAddress : gct_firstaddress_t; const pData : gct_vqt_real_text15_p; iCount : Integer; sContainerCode : PAnsiChar = nil): integer ; stdcall ; external gcl_sdk_dll name '_gci_yc_firstaddress_vqt_text15_send@20';
function gci_yc_firstaddress_vqt_text15_response(firstAddress : gct_firstaddress_t; const pData : gct_vqt_real_text15_p; iCount : Integer; sContainerCode : PAnsiChar = nil): integer ; stdcall ; external gcl_sdk_dll name '_gci_yc_firstaddress_vqt_text15_response@20';


implementation

end.






