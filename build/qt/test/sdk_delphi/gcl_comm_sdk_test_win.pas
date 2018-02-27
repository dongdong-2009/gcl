unit gcl_comm_sdk_test_win;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, gcl_comm_sdk, ExtCtrls, Spin, ComCtrls, Grids, IniFiles;

type
  Tgcl_comm_sdk_test_frm = class(TForm)
    infoEd: TMemo;
    initBn: TButton;
    cleanupBn: TButton;
    erroridBn: TButton;
    isconnectedBn: TButton;
    yx_sendBn: TButton;
    yc_send_bn: TButton;
    file_transfer_send_bn: TButton;
    file_transfer_send_ed: TEdit;
    param_ed: TMemo;
    GroupBox1: TGroupBox;
    GroupBox2: TGroupBox;
    GroupBox3: TGroupBox;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    TabSheet3: TTabSheet;
    TabSheet4: TTabSheet;
    TabSheet5: TTabSheet;
    StringGrid1: TStringGrid;
    Button1: TButton;
    Panel2: TPanel;
    TabSheet6: TTabSheet;
    TabSheet7: TTabSheet;
    PageControl2: TPageControl;
    Splitter1: TSplitter;
    Panel1: TPanel;
    command_ed: TComboBox;
    message_command_send_bn: TButton;
    Panel3: TPanel;
    testBn: TButton;
    delphiTestBn: TButton;
    SpinEdit1: TSpinEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    SpinEdit2: TSpinEdit;
    Button2: TButton;
    timerSendMessage: TTimer;
    sourceIdEd: TEdit;
    targetIdEd: TEdit;
    SpinEdit3: TSpinEdit;
    Label4: TLabel;
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure cleanupBnClick(Sender: TObject);
    procedure connectBnClick(Sender: TObject);
    procedure delphiTestBnClick(Sender: TObject);
    procedure erroridBnClick(Sender: TObject);
    procedure file_transfer_send_bnClick(Sender: TObject);
    procedure initBnClick(Sender: TObject);
    procedure isconnectedBnClick(Sender: TObject);
    procedure message_command_send_bnClick(Sender: TObject);
    procedure register_error_bnClick(Sender: TObject);
    procedure register_file_transfer_bnClick(Sender: TObject);
    procedure register_message_command_bnClick(Sender: TObject);
    procedure testBnClick(Sender: TObject);
    procedure timerSendMessageTimer(Sender: TObject);
    procedure yc_send_bnClick(Sender: TObject);
    procedure yx_sendBnClick(Sender: TObject);
  private
    timerSendMessageTimes: Integer;
    procedure doInit;
    procedure doQueryConnectStatus;
    procedure doSendYx;
    procedure outInfo(sInfo: string);
    { Private declarations }
  public
    { Public declarations }
  end;

  procedure deal_error(iErrorId : Integer); cdecl;
  procedure deal_message_comand(const sCommand : PAnsiChar; const sParam :
      PAnsiChar; iParamLength : Integer; const oAttach : PPsmAttach); cdecl;
  procedure deal_file_transfer(const sFilePath: PAnsiChar; const oAttach:
      PPsmAttach); cdecl;
  procedure deal_realtime_data_request(const oAttach: PPsmAttach); cdecl;
  procedure deal_realtime_data_post(iParamType: Integer; const pData: PAnsiChar;
      iDataLength, iParamCount: Integer; const oAttach: PPsmAttach); cdecl;

var
  gcl_comm_sdk_test_frm: Tgcl_comm_sdk_test_frm;

  fn_deal_error: gci_fn_error_t ;
  fn_deal_message_command: gci_fn_message_command_t     ;
  fn_deal_file_transfer: gci_fn_file_transfer_t        ;
  fn_deal_realtime_data_request: gci_fn_realtime_data_request_t     ;
  fn_deal_realtime_data_post: gci_fn_realtime_data_post_t     ;



  gi_error_id : integer;

  //当前仪器ID号
  f_iContainerId : Integer;

implementation

uses
  gcl_comm_sdk_unit;

{$R *.dfm}

procedure deal_error(iErrorId : Integer);
begin
  gi_error_id := iErrorId;
  gcl_comm_sdk_test_frm.outInfo(Format('deal_error -> [iErrorId : %d]!!!', [iErrorId]));
//  PostMessage(1, 0, 0, 0);
end;

procedure deal_message_comand(const sCommand : PAnsiChar; const sParam :
    PAnsiChar; iParamLength : Integer; const oAttach : PPsmAttach);
var
  sCommand1 : string;
  sParam1 : string;
  sAttach : string;
begin
  sCommand1 := sCommand;
  sParam1 := sParam;

  if (oAttach <> nil) then
  begin
    sAttach := Format('reason=%d , containerId=%d , sourceUserId=%d , targetUserID=%d , tag=%d',
      [oAttach.reason, oAttach.containerId, oAttach.sourceUserId, oAttach.targetUserID, oAttach.tag]);
  end;
  gcl_comm_sdk_test_frm.outInfo(Format('deal_message_comand -> [sCommand : %s] [sParam : %s] %s!!!', [sCommand1, sParam1, sAttach]));
end;

procedure deal_file_transfer(const sFilePath: PAnsiChar; const oAttach:
    PPsmAttach);
var
  sFilePath1 : string;
  sAttach : string;
begin
  sFilePath1 := sFilePath;
  if (oAttach <> nil) then
  begin
    sAttach := Format('reason=%d , containerId=%d , sourceUserId=%d , targetUserID=%d , tag=%d',
      [oAttach.reason, oAttach.containerId, oAttach.sourceUserId, oAttach.targetUserID, oAttach.tag]);
  end;
  gcl_comm_sdk_test_frm.outInfo(Format('deal_file_transfer -> [sFilePath : %s] %s!!!', [sFilePath1, sAttach]));
end;

procedure deal_realtime_data_request(const oAttach: PPsmAttach);
var
  sAttach : string;
begin
  if (oAttach <> nil) then
  begin
    sAttach := Format('reason=%d , containerId=%d , sourceUserId=%d , targetUserID=%d , tag=%d',
      [oAttach.reason, oAttach.containerId, oAttach.sourceUserId, oAttach.targetUserID, oAttach.tag]);
  end;
  gcl_comm_sdk_test_frm.outInfo(Format('deal_realtime_data_request -> %s!!!', [sAttach]));
end;

procedure deal_realtime_data_post(iParamType: Integer; const pData: PAnsiChar;
    iDataLength, iParamCount: Integer; const oAttach: PPsmAttach);
begin
  gcl_comm_sdk_test_frm.outInfo(Format('deal_realtime_data_post -> [ParamType : %d] - [DataLength : %d] - [Yx or Yc Count : %d]!!!', [iParamType, iDataLength, iParamCount]));
end;


procedure Tgcl_comm_sdk_test_frm.Button1Click(Sender: TObject);
begin
  doSendYx();
end;

procedure Tgcl_comm_sdk_test_frm.Button2Click(Sender: TObject);
begin
  if (not timerSendMessage.Enabled) then
  begin
    timerSendMessageTimes := 0;
    timerSendMessage.Interval := SpinEdit2.Value;
  end;
  timerSendMessage.Enabled := not timerSendMessage.Enabled;
  if (timerSendMessage.Enabled) then
  begin
      Button2.Caption := '(ms) 停止定时发';
  end
  else
  begin
      Button2.Caption := '(ms) 开始定时发';
  end;
end;

procedure Tgcl_comm_sdk_test_frm.FormCreate(Sender: TObject);
begin
   gi_error_id := 0;
   fn_deal_error := gci_fn_error_t(0);
end;

procedure Tgcl_comm_sdk_test_frm.cleanupBnClick(Sender: TObject);
begin
  Self.infoEd.Clear();
end;

procedure Tgcl_comm_sdk_test_frm.connectBnClick(Sender: TObject);
begin
  outInfo( IntToStr( gci_terminal_status()) );
end;

procedure Tgcl_comm_sdk_test_frm.delphiTestBnClick(Sender: TObject);
 var
  sCommand : string;
  sParam : string;
  iParam : Integer;
begin
//  sCommand := 'leadto_home';
  sCommand := Self.command_ed.Text;
  Self.param_ed.Lines.Delimiter := #13;
  sParam := Self.param_ed.Lines.DelimitedText;
  iParam := Length(sParam);
  ShowMessage(sParam);
end;

procedure Tgcl_comm_sdk_test_frm.erroridBnClick(Sender: TObject);
var
  sr : string;
begin
  sr := gci_terminal_info();
  sr := StringReplace(sr, #10, #13#10, [rfReplaceAll]);
  outInfo(sr);
end;

procedure Tgcl_comm_sdk_test_frm.file_transfer_send_bnClick(Sender: TObject);
var
  sFilePath : string;
  iResult : Integer;
  attach : PsmAttach;
begin
  attach.reason := 6;
  attach.containerId := 7;
  attach.sourceUserId := StrToInt(sourceIdEd.Text);
  attach.targetUserID := StrToInt(targetIdEd.Text);
  attach.tag := 10;
//  sFilePath := 'c:/0.txt';
  sFilePath := Self.file_transfer_send_ed.Text;
  iResult := gci_file_transfer_send(PAnsiChar(AnsiString(sFilePath)), @attach);
  outInfo(IntToStr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.isconnectedBnClick(Sender: TObject);
begin
  doQueryConnectStatus;
end;

procedure Tgcl_comm_sdk_test_frm.message_command_send_bnClick(Sender: TObject);
var
  sCommand : string;
  sParam : string;
  iParam : Integer;
  iResult : Integer;
  attach : PsmAttach;
  i : Integer;
begin
//  sCommand := 'leadto_home';
  sCommand := Self.command_ed.Text;
  Self.param_ed.Lines.Delimiter := #13;
  sParam := Self.param_ed.Lines.DelimitedText;
  iParam := Length(sParam);
  attach.reason := 6;
  attach.containerId := 7;
  attach.sourceUserId := StrToInt(sourceIdEd.Text);
  attach.targetUserID := StrToInt(targetIdEd.Text);
  attach.tag := 10;
  for i := 1 to SpinEdit3.Value do
  begin
    iResult := gci_message_command_send(PAnsiChar(AnsiString(sCommand)), PAnsiChar(AnsiString(sParam)), iParam, @attach);
    outInfo('send message ' + sCommand + '. Result=' + IntToStr(iResult));
  end;
end;

procedure Tgcl_comm_sdk_test_frm.doInit;
var
  s : string;
  argv : array of PAnsiChar;
  iResult : Integer;
begin
  setlength(argv, ParamCount + 1);
  s := paramstr(0);
  argv[0] := PAnsiChar(AnsiString(s));

  iResult := gci_init( ParamCount + 1, argv );
  outInfo( 'gci_init : ' + inttostr( iResult ));

  if (iResult > 0) then
  begin
    fn_deal_error := @deal_error;
    iResult := gci_register_error_notify(fn_deal_error);
    outInfo('gci_register_error_notify : ' + inttostr( iResult ));

    fn_deal_message_command := @deal_message_comand;
    iResult := gci_register_message_command_notify(fn_deal_message_command);
    outInfo( 'gci_register_message_command_notify : ' + inttostr( iResult ) );

    fn_deal_file_transfer := @deal_file_transfer;
    iResult := gci_register_file_transfer_notify(fn_deal_file_transfer);
    outInfo( 'gci_register_file_transfer_notify : ' + inttostr( iResult ) );

    fn_deal_realtime_data_request := @deal_realtime_data_request;
    iResult := gci_register_realtime_data_request_notify(fn_deal_realtime_data_request);
    outInfo( 'gci_register_realtime_data_request_notify : ' + inttostr( iResult ) );

    fn_deal_realtime_data_post := @deal_realtime_data_post;
    iResult := gci_register_realtime_data_post_notify(fn_deal_realtime_data_post);
    outInfo( 'gci_register_realtime_data_post_notify : ' + inttostr( iResult ) );
  end;
end;

procedure Tgcl_comm_sdk_test_frm.doQueryConnectStatus;
begin
  if (gci_terminal_status() = 0) then
  begin
    outInfo( '正常收到心跳回应！！！' );
  end
  else
  begin
    outInfo( '没有收到心跳回应！！！' );
  end;
end;

procedure Tgcl_comm_sdk_test_frm.doSendYx;
var
  StringGrid : TStringGrid;
  i : Integer;
begin

end;

procedure Tgcl_comm_sdk_test_frm.initBnClick(Sender: TObject);
begin
  doInit();
end;

procedure Tgcl_comm_sdk_test_frm.outInfo(sInfo: string);
begin
  if ( self.infoEd.Lines.Count > 2000 ) then
    self.infoEd.Clear();
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

procedure Tgcl_comm_sdk_test_frm.testBnClick(Sender: TObject);
var
  i: Integer;
  ss : TStringList;
  ini : TIniFile;
begin
  try
    ss := TStringList.Create();
    ini := TIniFile.Create('c:\userinfo.ini');
    ini.ReadSections(ss);
    for i := 0 to ss.Count-1 do
    begin
      ShowMessage(ss[i]);
    end;
  finally
    ini.Free;
    ss.Free;
  end;

//  gci_test( Self.SpinEdit1.Value );
end;

procedure Tgcl_comm_sdk_test_frm.timerSendMessageTimer(Sender: TObject);
var
  sCommand : string;
  sParams : TStringList;
  sParam : string;
  iParam : Integer;
  iResult : Integer;
  attach : PsmAttach;
  i : Integer;
begin
//  sCommand := 'leadto_home';
  for i := 1 to SpinEdit3.Value do
  begin
    sParams := TStringList.Create;
    try
      sCommand := Self.command_ed.Text;
      sParams.AddStrings(param_ed.Lines);
      Inc(timerSendMessageTimes);
      sParams.Add(IntToStr(timerSendMessageTimes)) ;
      sParams.Delimiter := #13;
      sParam := sParams.DelimitedText;
      iParam := Length(sParam);
      attach.reason := 6;
      attach.containerId := 7;
      attach.sourceUserId := StrToInt(sourceIdEd.Text);
      attach.targetUserID := StrToInt(targetIdEd.Text);
      attach.tag := 10;
      iResult := gci_message_command_send(PAnsiChar(AnsiString(sCommand)), PAnsiChar(AnsiString(sParam)), iParam, @attach);
      outInfo('send message ' + sCommand + '. Result=' + IntToStr(iResult));
    finally
      sParams.Free;
    end;
  end;
end;

procedure Tgcl_comm_sdk_test_frm.yc_send_bnClick(Sender: TObject);
var
  values : array[0..69] of gcd_address_vqt_realText15_t;
  iResult : Integer;
  i : Integer;
  iSize: Integer;
  sValue : string;
  dValue : double;
begin
  for i := 0 to Length(values)-1 do
  begin
    values[i].address := $02000000 + i;
    dValue := i * 1.23;
    sValue := Format('%8.2f', [dValue]);
    iSize := Length(sValue);
    if (iSize<Length(values[i].value)) then
    begin
      Move(sValue[1], values[i].value, iSize);
      values[i].value[iSize] := #0;
    end;
    values[i].quality := 1;
    values[i].datetime := Now;
  end;
  iSize := SizeOf(values);

  iResult := gci_realtime_data_post($0101021E, @(values[0]), iSize, Length(values));
  outInfo(IntToStr(iResult));
end;

procedure Tgcl_comm_sdk_test_frm.yx_sendBnClick(Sender: TObject);
var
  values : array[0..19] of gcd_address_vqt_intText1_t;
  iResult : Integer;
  i : Integer;
  iSize: Integer;
  sValue : string;
begin
  for i := 0 to Length(values)-1 do
  begin
    values[i].address := $01000000 + i;
    sValue := IntToStr( i mod 10 );
    iSize := Length(sValue);
    if (iSize<Length(values[i].value)) then
    begin
      Move(sValue[1], values[i].value, iSize);
      values[i].value[iSize] := #0;
    end;
    values[i].quality := 1;
    values[i].datetime := Now;
  end;
  iSize := SizeOf(values);

  iResult := gci_realtime_data_post($01010205, @(values[0]), iSize, Length(values));
  outInfo(IntToStr(iResult));
end;

end.



