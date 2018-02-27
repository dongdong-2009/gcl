unit gcl_comm_sdk_test_win;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, gcl_comm_sdk, ExtCtrls, Spin;

type
  Tgcl_comm_sdk_test_frm = class(TForm)
    infoEd: TMemo;
    Panel1: TPanel;
    register_error_bn: TButton;
    initBn: TButton;
    cleanupBn: TButton;
    erroridBn: TButton;
    testBn: TButton;
    SpinEdit1: TSpinEdit;
    connectBn: TButton;
    disconnectBn: TButton;
    isconnectedBn: TButton;
    yx_sendBn: TButton;
    register_message_command_bn: TButton;
    register_file_transfer_bn: TButton;
    register_realtime_container_data_bn: TButton;
    yc_send_bn: TButton;
    file_transfer_send_bn: TButton;
    message_command_send_bn: TButton;
    procedure FormCreate(Sender: TObject);
    procedure cleanupBnClick(Sender: TObject);
    procedure connectBnClick(Sender: TObject);
    procedure disconnectBnClick(Sender: TObject);
    procedure erroridBnClick(Sender: TObject);
    procedure file_transfer_send_bnClick(Sender: TObject);
    procedure initBnClick(Sender: TObject);
    procedure isconnectedBnClick(Sender: TObject);
    procedure message_command_send_bnClick(Sender: TObject);
    procedure register_error_bnClick(Sender: TObject);
    procedure register_file_transfer_bnClick(Sender: TObject);
    procedure register_message_command_bnClick(Sender: TObject);
    procedure register_realtime_container_data_bnClick(Sender: TObject);
    procedure testBnClick(Sender: TObject);
    procedure yc_send_bnClick(Sender: TObject);
    procedure yx_sendBnClick(Sender: TObject);
  private
    procedure outInfo(sInfo: string);
    { Private declarations }
  public
    { Public declarations }
  end;

  procedure deal_error(iErrorId : Integer); stdcall;
  procedure deal_message_comand(const sCommand : PAnsiChar; const sParam : PAnsiChar; iParamLength : Integer; const sContainerCode : PAnsiChar; iTag : Integer); stdcall;
  procedure deal_file_transfer(const sFilePath : PAnsiChar; iTag : Integer); stdcall;
  procedure deal_realtime_container_data(const sContainerCode : PAnsiChar; iTag : Integer); stdcall;

var
  gcl_comm_sdk_test_frm: Tgcl_comm_sdk_test_frm;

  fn_deal_error: gci_fn_error_t ;
  fn_deal_message_command: gci_fn_message_command_t     ;
  fn_deal_file_transfer: gci_fn_file_transfer_t        ;
  fn_deal_realtime_container_data: gci_fn_realtime_container_data_t     ;



  gi_error_id : integer;

  //µ±Ç°ÒÇÆ÷IDºÅ
  f_iContainerId : Integer;

implementation

{$R *.dfm}

procedure deal_error(iErrorId : Integer);  StdCall;
begin
  gi_error_id := iErrorId;
  gcl_comm_sdk_test_frm.outInfo(Format('hello deal_error -> [iErrorId : %d]!!!', [iErrorId]));
//  PostMessage(1, 0, 0, 0);
end;

procedure deal_message_comand(const sCommand : PAnsiChar; const sParam : PAnsiChar; iParamLength : Integer; const sContainerCode : PAnsiChar; iTag : Integer); stdcall;
//var
//  sCommand1 : string;
//  sParam1 : string;
//  sContainerCode1 : string;
begin
//  sCommand1 := sCommand;
//  sParam1 := sParam;
//  sContainerCode1 := sContainerCode;
  gcl_comm_sdk_test_frm.outInfo(Format('hello deal_message_comand -> [sCommand : %s] [sParam : %s] [sContainerCode : %s] [iTag : %d]!!!', [sCommand, sParam, sContainerCode, iTag]));
end;

procedure deal_file_transfer(const sFilePath : PAnsiChar; iTag : Integer); stdcall;
begin
  gcl_comm_sdk_test_frm.outInfo(Format('hello deal_message_comand -> [sFilePath : %s] [iTag : %d]!!!', [sFilePath, iTag]));
end;

procedure deal_realtime_container_data(const sContainerCode : PAnsiChar; iTag : Integer); stdcall;
begin

end;


procedure Tgcl_comm_sdk_test_frm.FormCreate(Sender: TObject);
begin
   gi_error_id := 0;
   fn_deal_error := gci_fn_error_t(0);
end;

procedure Tgcl_comm_sdk_test_frm.cleanupBnClick(Sender: TObject);
var
  iResult : integer;
begin
  iResult := gci_cleanup();
  showmessage(inttostr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.connectBnClick(Sender: TObject);
var
  sInFilePath : string;
  sOutFilePath : string;
begin
  sInFilePath := 'c:/0.txt';
  SetLength(sOutFilePath, 255);
  outInfo( IntToStr( gci_connect(PAnsiChar(AnsiString(sInFilePath)), PAnsiChar(AnsiString(sOutFilePath))) ) );
end;

procedure Tgcl_comm_sdk_test_frm.disconnectBnClick(Sender: TObject);
begin
  outInfo( IntToStr( gci_disconnect( ) ) );
end;

procedure Tgcl_comm_sdk_test_frm.erroridBnClick(Sender: TObject);
begin
  showmessage(inttostr(gi_error_id));
end;

procedure Tgcl_comm_sdk_test_frm.file_transfer_send_bnClick(Sender: TObject);
var
  sFilePath : string;
  iResult : Integer;
begin
  sFilePath := 'c:/0.txt';
  iResult := gci_file_transfer_send(PAnsiChar(AnsiString(sFilePath)));
  outInfo(IntToStr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.initBnClick(Sender: TObject);
var
  s : string;
  argv : array of PAnsiChar;
  iResult : Integer;
begin
  setlength(argv, ParamCount + 1);
  s := paramstr(0);
  argv[0] := PAnsiChar(AnsiString(s));

  iResult := gci_init( ParamCount + 1, argv );
  outInfo(inttostr( iResult ));
end;

procedure Tgcl_comm_sdk_test_frm.isconnectedBnClick(Sender: TObject);
begin
  outInfo( IntToStr( gci_is_connected( ) ) );
end;

procedure Tgcl_comm_sdk_test_frm.message_command_send_bnClick(Sender: TObject);
var
  sCommand : string;
  iResult : Integer;
begin
  sCommand := 'leadto_home';
  iResult := gci_message_command_send(PAnsiChar(AnsiString(sCommand)));
  outInfo(IntToStr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.outInfo(sInfo: string);
begin
  self.infoEd.Lines.Append(sInfo);
  Self.infoEd.Lines.Append('');
end;

procedure Tgcl_comm_sdk_test_frm.register_error_bnClick(Sender: TObject);
var
  iResult : Integer;
begin
  fn_deal_error := @deal_error;
  iResult := gci_register_error_notify(fn_deal_error);
  outInfo(IntToStr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.register_file_transfer_bnClick(Sender:
    TObject);
var
  iResult : Integer;
begin
  fn_deal_file_transfer := @deal_file_transfer;
  iResult := gci_register_file_transfer_notify(fn_deal_file_transfer);
  outInfo(IntToStr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.register_message_command_bnClick(Sender:
    TObject);
var
  iResult : Integer;
begin
  fn_deal_message_command := @deal_message_comand;
  iResult := gci_register_message_command_notify(fn_deal_message_command);
  outInfo(IntToStr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.register_realtime_container_data_bnClick(
    Sender: TObject);
var
  iResult : Integer;
begin
  fn_deal_realtime_container_data := @deal_realtime_container_data;
  iResult := gci_register_file_transfer_notify(fn_deal_realtime_container_data);
  outInfo(IntToStr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.testBnClick(Sender: TObject);
begin
  gci_test( Self.SpinEdit1.Value );
end;

procedure Tgcl_comm_sdk_test_frm.yc_send_bnClick(Sender: TObject);
var
  address : gct_firstaddress_t;
  values : array[0..19] of gct_vqt_real_text15_t;
  iResult : Integer;
  i : Integer;
begin
  address.addressBegin := 1;
  address.addressEnd := 20;
  for i := 0 to Length(values)-1 do
  begin
    values[i].quality := 1;
    values[i].datetime := Now;
    values[i].value := '1.234';
  end;

  iResult := gci_yc_firstaddress_vqt_text15_send(address, @(values[0]), 20);
  outInfo(IntToStr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.yx_sendBnClick(Sender: TObject);
var
  address : gct_firstaddress_t;
  values : array[0..19] of gct_vqt_int_text1_t;
  iResult : Integer;
  i : Integer;
begin
  address.addressBegin := 1;
  address.addressEnd := 20;
  for i := 0 to Length(values)-1 do
  begin
    values[i].quality := 1;
    values[i].datetime := Now;
    values[i].value := '1';
  end;

  iResult := gci_yx_firstaddress_vqt_text1_send(address, @(values[0]), 20);
  outInfo(IntToStr(iResult));
end;

end.
