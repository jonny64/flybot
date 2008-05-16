library ChatBot;

uses
  SysUtils,
  Classes,
  Windows,
  //необходимые определения
  botdef in 'botdef.pas',

  //обработка сообщений от клиента
  bot in 'bot.pas',

  //управление сенсами лички
  session in 'session.pas',

  //работа со словарем фраз-ответов
  dictionary in 'dictionary.pas',

  //иконка в системном лотке
  tray in 'tray.pas' {TrayForm},

  //сторонний модуль: поддержка регулярных
  //выражений для словаря
  RegExpr in 'RegExpr.pas';

{$R *.RES}

end.
