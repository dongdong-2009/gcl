unit gcl_comm_sdk;

interface

type
  PPsmAttach=^PsmAttach;
  PsmAttach = packed Record
    reason : Integer;
    containerId : Integer;
    sourceUserId : Integer;
    targetUserID : Integer;
    tag : Integer;
  END;

  gci_fn_error_t = procedure(iErrorId : Integer); cdecl;
  gci_fn_message_command_t = procedure(const sCommand : PAnsiChar; const sParam : PAnsiChar; iParamLength : Integer; const oAttach : PPsmAttach); cdecl;
  gci_fn_file_transfer_t = procedure(const sFilePath : PAnsiChar; const oAttach : PPsmAttach); cdecl;
  gci_fn_realtime_data_request_t = procedure(const oAttach : PPsmAttach); cdecl;
  gci_fn_realtime_data_post_t = procedure(iParamType : Integer; const pData : PAnsiChar; iDataLength : Integer; iParamCount : Integer; const oAttach : PPsmAttach); cdecl;


  gcd_firstaddress_t = Record
    addressBegin : Integer;
    addressEnd : Integer;
  END;
  gcd_firstaddress_p=^gcd_firstaddress_t;

  //*yx
  //0x01010205
  gcd_address_vqt_intText1_t = Record
    address : Integer;
    value : array[0..7] of CHAR;
    quality : Integer;
    datetime : double;
  END;
  gcd_address_vqt_intText1_p=^gcd_address_vqt_intText1_t;

  //0x01010203
  gcd_address_int32_vqt_t = Record
    address : Integer;
    value : Integer;
    quality : Integer;
    datetime : double;
  END;
  gcd_address_int32_vqt_p=^gcd_address_int32_vqt_t;


  //0x01020205
  gcd_firstAddress_vqt_intText1_t = Record
    value : array[0..7] of CHAR;
    quality : Integer;
    datetime : double;
  END;
  gcd_firstAddress_vqt_intText1_p=^gcd_firstAddress_vqt_intText1_t;


  //*yc
  //0x0101021E
  gcd_address_vqt_realText15_t = Record
      address : Integer;
      value : array[0..15] of CHAR;
      quality : Integer;
      datetime : double;
  END;
  gcd_address_vqt_realText15_p=^gcd_address_vqt_realText15_t;

    //0x0101021C
  gcd_address_double_vqt_t = Record
      address : Integer;
      value : double;
      quality : Integer;
      datetime : double;
  END;
  gcd_address_double_vqt_p=^gcd_address_double_vqt_t;

  //0x0102021E
  gcd_firstAddress_vqt_realText15_t = Record
      value : array[0..15] of CHAR;
      quality : Integer;
      datetime : double;
  END;
  gcd_firstAddress_vqt_realText15_p=^gcd_firstAddress_vqt_realText15_t;


const
  gcl_sdk_dll  = 'gcl_sdk1.dll';


function gci_init(argc : integer; argv : array of PAnsiChar): integer ; cdecl ; external gcl_sdk_dll name 'gci_init';
function gci_terminal_status(iId : integer = 0): integer ; cdecl ; external gcl_sdk_dll name 'gci_terminal_status';
function gci_terminal_info(iId : integer = 0): PAnsiChar ; cdecl ; external gcl_sdk_dll name 'gci_terminal_info';
function gci_test(iTest : integer): integer ; cdecl ; external gcl_sdk_dll name 'gci_test';


function gci_last_error_id(): integer ; cdecl ; external gcl_sdk_dll name 'gci_last_error_id';
function gci_error_info(iErrorId : Integer = 0): PAnsiChar ; cdecl ; external gcl_sdk_dll name 'gci_error_info';

function gci_message_command_send(const sCommand : PAnsiChar; const sParam : PAnsiChar = nil; iParamLength : Integer = 0; const oAttach : PPsmAttach = nil): integer ; cdecl ; external gcl_sdk_dll name 'gci_message_command_send';

function gci_file_transfer_send(const sFilePath : PAnsiChar; const oAttach : PPsmAttach = nil): integer ; cdecl ; external gcl_sdk_dll name 'gci_file_transfer_send';

function gci_register_error_notify(fn : gci_fn_error_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_register_error_notify';
function gci_register_message_command_notify(fn : gci_fn_message_command_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_register_message_command_notify';
function gci_register_file_transfer_notify(fn : gci_fn_file_transfer_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_register_file_transfer_notify';
function gci_register_realtime_data_request_notify(fn : gci_fn_realtime_data_request_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_register_realtime_data_request_notify';
function gci_register_realtime_data_post_notify(fn : gci_fn_realtime_data_post_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_register_realtime_data_post_notify';

function gci_unregister_error_notify(fn : gci_fn_error_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_unregister_error_notify';
function gci_unregister_message_command_notify(fn : gci_fn_message_command_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_unregister_message_command_notify';
function gci_unregister_file_transfer_notify(fn : gci_fn_file_transfer_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_unregister_file_transfer_notify';
function gci_unregister_realtime_data_request_notify(fn : gci_fn_realtime_data_request_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_unregister_realtime_data_request_notify';
function gci_unregister_realtime_data_post_notify(fn : gci_fn_realtime_data_post_t): integer ; cdecl ; external gcl_sdk_dll name 'gci_unregister_realtime_data_post_notify';

function gci_realtime_data_request(const oAttach : PPsmAttach = nil): integer ; cdecl ; external gcl_sdk_dll name 'gci_realtime_data_request';
function gci_realtime_data_post(iParamType : Integer; const pData : PAnsiChar; iDataLength : Integer; iParamCount : Integer; const oAttach : PPsmAttach = nil): integer ; cdecl ; external gcl_sdk_dll name 'gci_realtime_data_post';


implementation


end.




