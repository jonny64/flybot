library ChatBot;

uses
  SysUtils,
  Classes,
  Windows,
  //����������� �����������
  botdef in 'botdef.pas',

  //��������� ��������� �� �������
  bot in 'bot.pas',

  //���������� ������� �����
  session in 'session.pas',

  //������ �� �������� ����-�������
  dictionary in 'dictionary.pas',

  //������ � ��������� �����
  tray in 'tray.pas' {TrayForm},

  //��������� ������: ��������� ����������
  //��������� ��� �������
  RegExpr in 'RegExpr.pas';

{$R *.RES}

end.
