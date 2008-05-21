unit settings;

interface

uses
  Windows, SysUtils, IniFiles, botdef;

  procedure ReadSettings;
  procedure WriteSettings;
var
  //время, на которое выдается слот, сек.
  g_slotTimeout:integer;
  //задержка перед выдачей фразы-ответа, сек.
  g_answrDelay:integer;

implementation

function GetModulePath:string;
var exe:array[0..512]of char;
begin
  GetModuleFilename(0, exe, 512);
  result:=ExtractFilePath(string(exe))
end;

procedure ReadSettings;
var
  Ini: TIniFile;
begin
  Ini := TIniFile.Create(GetModulePath + BOT_SETTINGS_FILE);
  try
    g_slotTimeout:=Ini.ReadInteger( 'Timeout', 'Slot', DEFAULT_SLOT_TIMEOUT );
    g_answrDelay:=Ini.ReadInteger( 'Delay', 'Answer', DEFAULT_ANSWER_DELAY );
  finally
    Ini.Free;
  end;
end;

procedure WriteSettings;
var
  Ini: TIniFile;
begin
  Ini := TIniFile.Create(GetModulePath + BOT_SETTINGS_FILE);
  try
    Ini.WriteInteger( 'Timeout', 'Slot',  g_slotTimeout);
    Ini.WriteInteger( 'Delay', 'Answer',  g_answrDelay);
  finally
    Ini.Free;
  end;
end;

end.
