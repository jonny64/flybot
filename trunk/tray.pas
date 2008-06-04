unit tray;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, ShellAPI, CoolTrayIcon, Menus, settings, botdef;

type
  TTrayForm = class(TForm)
    ImageOn: TImage;
    ImageOff: TImage;
    CoolTrayIconMain: TCoolTrayIcon;
    PopupMenuMain: TPopupMenu;
    mniSwitch: TMenuItem;
    mniReloadDict: TMenuItem;
    mniPM: TMenuItem;
    mniSettings: TMenuItem;
    mniSlot: TMenuItem;
    popSlot10m: TMenuItem;
    popSlot1h: TMenuItem;
    popSlot1d: TMenuItem;
    mniOpenDict: TMenuItem;
    procedure mniSwitchClick(Sender: TObject);
    procedure mniReloadDictClick(Sender: TObject);
    procedure CoolTrayIconMainMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure popSlot10mClick(Sender: TObject);
    procedure popSlot1hClick(Sender: TObject);
    procedure popSlot1dClick(Sender: TObject);
    procedure PopupMenuMainPopup(Sender: TObject);
    procedure mniOpenDictClick(Sender: TObject);
  private
    { Private declarations }
    procedure SwitchIcon;
  public
    { Public declarations }
    botEnabled: boolean;
    procedure ShowErrMsg(msg:string);
    procedure ShowInfoMsg(msg:string);
    procedure AddTrayIcon(const tip: string);
  end;
       
var
  TrayForm: TTrayForm;

implementation

{$R *.DFM}

uses
   dictionary;
var stateStr:array[1..2]of string=('&Включить', '&Отключить');

//сообщение об ошибке
procedure TTrayForm.ShowErrMsg(msg:string);
begin
    CoolTrayIconMain.ShowBalloonHint('Проблема',
    msg, bitError,10);
end;

//информационное сообщение
procedure TTrayForm.ShowInfoMsg(msg:string);
begin
    CoolTrayIconMain.ShowBalloonHint('Операция прошла успешно',
    msg, bitInfo,10)
end;

//установка иконки, отражающей состоягние бота
procedure TTrayForm.SwitchIcon;
begin
  if (botEnabled) then
     CoolTrayIconMain.Icon := ImageOn.Picture.Icon
  else
      CoolTrayIconMain.Icon := ImageOff.Picture.Icon;
end;

//инициализация иконки в трее
procedure TTrayForm.AddTrayIcon(const tip: string);
begin
  CoolTrayIconMain.Hint:=tip;
  PopupMenuMain.Items[4].Caption:=stateStr[integer(botEnabled)+1];
  SwitchIcon;
end;

//переключаем текст на пункте меню
procedure TTrayForm.mniSwitchClick(Sender: TObject);
begin
  botEnabled := not botEnabled;

  PopupMenuMain.Items[5].Caption:=stateStr[integer(botEnabled)+1];
  SwitchIcon;
end;

//перезагрузка словаря с диска
procedure TTrayForm.mniReloadDictClick(Sender: TObject);
begin
  if dict.Reload    then
    ShowInfoMsg('Словарь перезагружен')
end;

procedure TTrayForm.CoolTrayIconMainMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if (Button=mbLeft) then mniSwitchClick(nil);
end;

procedure TTrayForm.popSlot10mClick(Sender: TObject);
begin
  g_slotTimeout:=600;
  WriteSettings;
end;

procedure TTrayForm.popSlot1hClick(Sender: TObject);
begin
  g_slotTimeout:=3600;
  WriteSettings;
end;

procedure TTrayForm.popSlot1dClick(Sender: TObject);
begin
  g_slotTimeout:=86400;
  WriteSettings;
end;

procedure TTrayForm.PopupMenuMainPopup(Sender: TObject);
begin
  with PopupMenuMain.Items[2] do
    case g_slotTimeout of
         600:  Items[0].Checked:=true;
         3600: Items[1].Checked:=true;
         86400: Items[2].Checked:=true;
    end;
end;

procedure TTrayForm.mniOpenDictClick(Sender: TObject);
begin
  ShellExecute(0, 'open', PChar(DICTIONARY_FILENAME), '','', SW_SHOW);
end;

initialization
  TrayForm := TTrayForm.Create(nil);
  TrayForm.botEnabled:=true;
  TrayForm.AddTrayIcon(BOT_VERSION_STRING);
finalization
  TrayForm.Free;
  TrayForm := nil;
end.
