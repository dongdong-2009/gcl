program gcl_comm_sdk_test;

uses
  Forms,
  gcl_comm_sdk_test_win in 'gcl_comm_sdk_test_win.pas' {gcl_comm_sdk_test_frm},
  gcl_comm_sdk in 'gcl_comm_sdk.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(Tgcl_comm_sdk_test_frm, gcl_comm_sdk_test_frm);
  Application.Run;
end.
