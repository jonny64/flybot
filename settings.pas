unit settings;

interface

uses
  Windows, SysUtils, IniFiles, botdef;

  procedure ReadSettings;
  procedure WriteSettings;
var
  //�����, �� ������� �������� ����, ���.
  g_slotTimeout:integer;
  //�������� ����� ������� �����-������, ���.
  g_answrDelay:integer;
  //���������� �����������
  g_baloonInfo:boolean;

implementation

uses tray;

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
    g_slotTimeout:=Ini.ReadInteger( 'Time', 'SlotTimeout', DEFAULT_SLOT_TIMEOUT );
    g_answrDelay:=Ini.ReadInteger( 'Time', 'AnswerDelay', DEFAULT_ANSWER_DELAY );
    g_baloonInfo:=Ini.ReadBool( 'Info', 'ShowBaloon', True );
  except
    on e: exception do  TrayForm.ShowErrMsg(e.Message);
  end;
end;

procedure WriteSettings;
var
  Ini: TIniFile;
begin
  Ini := TIniFile.Create(GetModulePath + BOT_SETTINGS_FILE);
  try
    Ini.WriteInteger( 'Time', 'SlotTimeout',  g_slotTimeout);
    Ini.WriteInteger( 'Time', 'AnswerDelay',  g_answrDelay);
    Ini.WriteBool( 'Info', 'ShowBaloon',  g_baloonInfo);
  except
    on e: exception do  TrayForm.ShowErrMsg(e.Message);
  end;
end;

end.
