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
    mniAddDelay: TMenuItem;
    mni0: TMenuItem;
    mni6: TMenuItem;
    mni60: TMenuItem;
    procedure mniSwitchClick(Sender: TObject);
    procedure mniReloadDictClick(Sender: TObject);
    procedure CoolTrayIconMainMouseDown(Sender: TObject;
      Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
    procedure popSlot10mClick(Sender: TObject);
    procedure popSlot1hClick(Sender: TObject);
    procedure popSlot1dClick(Sender: TObject);
    procedure PopupMenuMainPopup(Sender: TObject);
    procedure mniOpenDictClick(Sender: TObject);
    procedure mni0Click(Sender: TObject);
    procedure mni6Click(Sender: TObject);
    procedure mni60Click(Sender: TObject);
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

implementation

{$R *.DFM}

uses
   dictionary;
var stateStr:array[1..2]of string=('&��������', '&���������');

//��������� �� ������
procedure TTrayForm.ShowErrMsg(msg:string);
begin
    CoolTrayIconMain.ShowBalloonHint('��������',
    msg, bitError,10);
end;

//�������������� ���������
procedure TTrayForm.ShowInfoMsg(msg:string);
begin
    CoolTrayIconMain.ShowBalloonHint('�������� ������ �������',
    msg, bitInfo,10)
end;

//�������������� ���������
procedure TTrayForm.ShowInfoMsg(msg:string; header: string);
begin
    CoolTrayIconMain.ShowBalloonHint(header, msg, bitInfo, 10);
end;

//��������� ������, ���������� ���������� ����
procedure TTrayForm.SwitchIcon;
begin
  if (botEnabled) then
     CoolTrayIconMain.Icon := ImageOn.Picture.Icon
  else
      CoolTrayIconMain.Icon := ImageOff.Picture.Icon;
end;

//������������� ������ � ����
procedure TTrayForm.AddTrayIcon(const tip: string);
begin
  CoolTrayIconMain.Hint:=tip;
  PopupMenuMain.Items[6].Caption:=stateStr[integer(botEnabled)+1];
  SwitchIcon;
end;

//����������� ����� �� ������ ����
procedure TTrayForm.mniSwitchClick(Sender: TObject);
begin
  botEnabled := not botEnabled;

  PopupMenuMain.Items[6].Caption:=stateStr[integer(botEnabled)+1];
  SwitchIcon;
end;

//������������ ������� � �����
procedure TTrayForm.mniReloadDictClick(Sender: TObject);
begin
  if dict.Reload    then
    ShowInfoMsg('������� ������������')
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
  with PopupMenuMain.Items[3] do
    case g_answrDelay of
         0:  Items[0].Checked:=true;
         6: Items[1].Checked:=true;
         60: Items[2].Checked:=true;
    end;
end;

procedure TTrayForm.mniOpenDictClick(Sender: TObject);
begin
  ShellExecute(0, 'open', PChar(DICTIONARY_FILENAME), '','', SW_SHOW);
end;

procedure TTrayForm.mni0Click(Sender: TObject);
begin
  g_answrDelay:=0;
  WriteSettings;
end;

procedure TTrayForm.mni6Click(Sender: TObject);
begin
  g_answrDelay:=6;
  WriteSettings;
end;

procedure TTrayForm.mni60Click(Sender: TObject);
begin
  g_answrDelay:=60;
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
