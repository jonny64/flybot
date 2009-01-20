//******************************************************************************
{*
* @file settings.pas
* @author xmm
* @brief управление настройками бота
* @details содержит процедуры сохраннения настроек в файл
*}
//******************************************************************************
unit settings;

interface

uses
  Windows, SysUtils, IniFiles, Classes, Types, def;

  procedure ReadSettings;
  procedure WriteSettings;
var
  //время, на которое выдается слот, сек.
  g_SlotTimeout, g_selectedTimeout: integer;
  g_slotTimeouts: TIntegerDynArray;
  //задержка перед выдачей фразы-ответа, сек.
  g_AnswrDelay, g_selectedAnswer: integer;
  g_answrDelays: TIntegerDynArray;

  //показывать уведомления
  g_baloonInfo:boolean;

implementation

uses tray;

procedure ReadSettings;
var
  Ini: TIniFile;
  slotTimeouts, answerDelays: TStringList;
  i:Integer;
begin
  Ini := TIniFile.Create(GetModulePath + BOT_SETTINGS_FILE);
  try
    slotTimeouts := TStringList.Create;
    answerDelays := TStringList.Create;
    Ini.ReadSection('AnswerDelay', answerDelays);
    Ini.ReadSection('SlotTimeout', slotTimeouts);

    for i:=0 to answerDelays.Count - 1 do begin
      SetLength(g_answrDelays, i + 1);
      g_answrDelays[i] := Ini.ReadInteger('AnswerDelay', answerDelays[i], DEFAULT_ANSWER_DELAY);
    end;
    for i:=0 to slotTimeouts.Count - 1 do begin
      SetLength(g_slotTimeouts, i + 1);
      g_slotTimeouts[i] := Ini.ReadInteger('SlotTimeout', slotTimeouts[i], DEFAULT_SLOT_TIMEOUT);
    end;
    g_selectedTimeout:= Ini.ReadInteger('Selected', 'SlotTimeout', 1);
    g_selectedAnswer := Ini.ReadInteger('Selected', 'AnswerDelay', 1);
    g_baloonInfo:=Ini.ReadBool( 'Info', 'ShowBaloon', True );
  except
    on e: exception do  TrayForm.ShowErrMsg(e.Message);
  end;
  
  slotTimeouts.Free;
  answerDelays.Free;
end;

procedure WriteSettings;
var
  Ini: TIniFile;
  i: integer;
begin
  Ini := TIniFile.Create(GetModulePath + BOT_SETTINGS_FILE);
  try
    for i:=0 to High(g_slotTimeouts) do begin
      Ini.WriteInteger('SlotTimeout', 'delay'+ ToString(i), g_slotTimeouts[i]);
    end;
    for i:=0 to High(g_answrDelays) do begin
      Ini.WriteInteger('AnswerDelay', 'delay'+ ToString(i), g_answrDelays[i] );
    end;
    Ini.WriteInteger('Selected', 'SlotTimeout', g_selectedTimeout);
    Ini.WriteInteger('Selected', 'AnswerDelay', g_selectedAnswer);
    Ini.WriteBool( 'Info', 'ShowBaloon',  g_baloonInfo);
  except
    on e: exception do  TrayForm.ShowErrMsg(e.Message);
  end;
end;

end.
