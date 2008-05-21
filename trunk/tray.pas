unit tray;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, CoolTrayIcon, Menus, settings, botdef;

type
  TTrayForm = class(TForm)
    ImageOn: TImage;
    ImageOff: TImage;
    CoolTrayIconMain: TCoolTrayIcon;
    PopupMenuMain: TPopupMenu;
    popSwitch: TMenuItem;
    popReloadDict: TMenuItem;
    popPM: TMenuItem;
    popSettings: TMenuItem;
    popSlot: TMenuItem;
    popSlot10m: TMenuItem;
    popSlot1h: TMenuItem;
    popSlot1d: TMenuItem;
    procedure popSwitchClick(Sender: TObject);
    procedure popReloadDictClick(Sender: TObject);
    procedure CoolTrayIconMainMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure popSlot10mClick(Sender: TObject);
    procedure popSlot1hClick(Sender: TObject);
    procedure popSlot1dClick(Sender: TObject);
  private
    { Private declarations }
    procedure SwitchIcon;
  public
    { Public declarations }
    botEnabled: boolean;
    procedure ShowErrMsg(msg:string);
    procedure ShowInfoMsg(msg:string);
    procedure OnWmMouseTray(var message: TMessage); message WM_USER;
    procedure AddTrayIcon(const tip: string);
    procedure BeforeDestruction; override;
    procedure Check(i:integer);
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
    msg, bitError,12);
end;

//информационное сообщение
procedure TTrayForm.ShowInfoMsg(msg:string);
begin
    CoolTrayIconMain.ShowBalloonHint('Операция прошла успешно',
    msg, bitInfo,12)
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
  PopupMenuMain.Items[3].Caption:=stateStr[integer(botEnabled)+1];
  SwitchIcon;
  {n.cbSize := sizeof(n);
  n.Wnd := handle;
  n.uID := 0;
  n.hIcon := Image1.Picture.Icon.Handle;
  n.uCallbackMessage := WM_USER;
  n.uFlags := NIF_ICON or NIF_TIP or NIF_MESSAGE;
  CopyMemory(@n.szTip, pchar(tip), sizeof(n.szTip));
  Shell_NotifyIcon(NIM_ADD, @n);
  state := true;   }
end;

procedure TTrayForm.BeforeDestruction;
begin
  {n.cbSize := sizeof(n);
  n.Wnd := handle;
  n.uID := 0;
  n.uFlags := 0;
  Shell_NotifyIcon(NIM_DELETE, @n);}
end;

procedure TTrayForm.OnWmMouseTray(var message: TMessage);
begin
  {if (message.LParam <> WM_LBUTTONDOWN) and (message.LParam <> WM_RBUTTONDOWN) then exit;
  state := not state;
  SwitchIcon;}
end;

procedure TTrayForm.popSwitchClick(Sender: TObject);
begin
  botEnabled := not botEnabled;
  //переключаем текст на пункте меню
  PopupMenuMain.Items[3].Caption:=stateStr[integer(botEnabled)+1];
  SwitchIcon;
end;

//перезагрузка словаря с диска
procedure TTrayForm.popReloadDictClick(Sender: TObject);
begin
  if dict.Reload    then
    ShowInfoMsg('Словарь перезагружен')
end;

procedure TTrayForm.CoolTrayIconMainMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
if (Button=mbLeft) then popSwitchClick(nil);
end;

procedure TTrayForm.Check(i:integer);
var j:integer;
begin
  for j := 0 to PopupMenuMain.Items[1].Count - 1 do
    PopupMenuMain.Items[1].Items[j].Checked:=false;
  PopupMenuMain.Items[1].Items[i].Checked:=true;
end;

procedure TTrayForm.popSlot10mClick(Sender: TObject);
begin
  g_slotTimeout:=600;
  Check(0);
  WriteSettings;
end;

procedure TTrayForm.popSlot1hClick(Sender: TObject);
begin
  g_slotTimeout:=3600;
  Check(1);
  WriteSettings;
end;

procedure TTrayForm.popSlot1dClick(Sender: TObject);
begin
  g_slotTimeout:=86400;
  Check(2);
  WriteSettings;
end;

initialization
  TrayForm := TTrayForm.Create(nil);
  TrayForm.botEnabled:=true;
  TrayForm.AddTrayIcon(BOT_VERSION_STRING);
finalization
  TrayForm.Free;
  TrayForm := nil;
end.
