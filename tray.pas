//******************************************************************************
{*
* @file bot.pas
* @author xmm
* @brief форма контекстного меню
* @details контекстное меню иконки в трее
*}
//******************************************************************************
unit tray;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, ShellAPI, Menus, settings, def, CoolTrayIcon;

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
    mniOpenDict: TMenuItem;
    mniAddDelay: TMenuItem;
    mniShowInfo: TMenuItem;
    mniYes: TMenuItem;
    mniNo: TMenuItem;
    procedure mniSwitchClick(Sender: TObject);
    procedure mniReloadDictClick(Sender: TObject);
    procedure CoolTrayIconMainMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure PopupMenuMainPopup(Sender: TObject);
    procedure mniOpenDictClick(Sender: TObject);
    procedure mniYesClick(Sender: TObject);
    procedure mniNoClick(Sender: TObject);
    procedure DelayItemClick(Sender: TObject);
  private
    { Private declarations }
    procedure SwitchIcon;
  public
    { Public declarations }
    botEnabled: boolean;
    procedure ShowErrMsg(msg:string);
    procedure ShowInfoMsg(msg:string); overload;
    procedure ShowInfoMsg(msg:string; header: string); overload;
    procedure AddTrayIcon(const tip: string);
  end;

var
  TrayForm: TTrayForm;
  item: TMenuItem;
  i:integer;
implementation

{$R *.DFM}

uses
   dictionary, bot;
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

//информационное сообщение
procedure TTrayForm.ShowInfoMsg(msg:string; header: string);
begin
    CoolTrayIconMain.ShowBalloonHint(header, msg, bitInfo, 10);
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
  PopupMenuMain.Items[7].Caption:=stateStr[integer(botEnabled)+1];
  SwitchIcon;
end;

//переключаем текст на пункте меню
procedure TTrayForm.mniSwitchClick(Sender: TObject);
begin
  botEnabled := not botEnabled;

  PopupMenuMain.Items[7].Caption:=stateStr[integer(botEnabled)+1];
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

procedure TTrayForm.PopupMenuMainPopup(Sender: TObject);
begin
    with PopupMenuMain.Items[4] do
    case g_baloonInfo of
         True:  Items[0].Checked:=true;
         False: Items[1].Checked:=true;
    end;
end;

procedure TTrayForm.mniOpenDictClick(Sender: TObject);
begin
  ShellExecute(0, 'open', PChar(GetModulePath + DICTIONARY_FILENAME), '','', SW_SHOW);
end;

procedure TTrayForm.mniYesClick(Sender: TObject);
begin
  g_baloonInfo:=True;
  WriteSettings;
end;

procedure TTrayForm.mniNoClick(Sender: TObject);
begin
  g_baloonInfo:=False;
  WriteSettings;
end;

procedure TTrayForm.DelayItemClick(Sender: TObject);
begin
  (Sender as TMenuItem).Checked:= True;
  WriteSettings;
end;

initialization
  TrayForm := TTrayForm.Create(nil);
  ReadSettings;
  for i:=0 to High(g_slotTimeouts) do begin
    item := TMenuItem.Create(nil);
    item.RadioItem := true;
    item.Tag := g_slotTimeouts[i];
    item.Caption := ToString(item.Tag div 60) + ' мин.';
    item.OnClick := TrayForm.DelayItemClick;
    TrayForm.PopupMenuMain.Items[2].Add(item);
  end;
  for i:=0 to High(g_answrDelays) do begin
    item := TMenuItem.Create(nil);
    item.RadioItem := true;
    item.Tag := g_answrDelays[i];
    item.Caption := ToString(item.Tag) + ' сек.';
    item.OnClick := TrayForm.DelayItemClick;
    TrayForm.PopupMenuMain.Items[3].Add(item);
  end;
  TrayForm.PopupMenuMain.Items[2].Items[g_selectedTimeout].Click();
  TrayForm.PopupMenuMain.Items[3].Items[g_selectedAnswer].Click();

  TrayForm.botEnabled:=true;
  TrayForm.AddTrayIcon(BOT_VERSION_STRING);
finalization
  TrayForm.Free;
  TrayForm := nil;
end.
