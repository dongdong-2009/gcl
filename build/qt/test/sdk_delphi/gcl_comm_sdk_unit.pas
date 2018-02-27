unit gcl_comm_sdk_unit;

interface

function gcl_init(argc : integer; argv : array of PAnsiChar): integer;

implementation

uses gcl_comm_sdk;

function gcl_init(argc : integer; argv : array of PAnsiChar): integer;
begin
  Result := gci_init(argc, argv);
end;

end.
