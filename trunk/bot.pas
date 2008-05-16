unit bot;

interface

uses
  SysUtils, Classes, Windows, Session, botdef;

var
  SendProc: tSendMessage;
  SendProc2: tSendMessage2;
  QueryInfo2: tQueryInfo;

  procedure OnRecvMessage2 (msgid:integer; objid:PWideChar; param: pointer; paramsize: Cardinal); stdcall;
  function init(var _init: TBotInit): boolean; stdcall;
  procedure SendMsg(cid, msg: WideString);
  exports init;

implementation

uses
  dictionary, tray;

  
//посылает личку пользователю с заданным cid
procedure SendMsg(cid, msg: WideString);
begin
      SendProc2( integer(SEND_PM), pWideChar(cid), pWideString(msg), sizeof(msg) );
end;

//обработчик уведомлений от клиента
procedure SendAnswer(userinfo, userMsg: WideString);
var
  cid,answer: WideString;
  i,sessionId: integer;
begin
  try
     //ответ зависит от предыстории (сеанса)
    cid := TUserSession.getCID(userinfo);
    sessionId := -1;
    // ... ищем структуру, описывающую сеанс sic! линейных поиск (плохо!?)
    for i := 0 to length(sessions)-1 do
      if (sessions[i].cid = cid) then sessionId := i;
    //... не нашли, т.к. новая личка -> создаем структуру, описывающиую сеанс
    if (sessionId < 0) then begin
      sessionId := length(Sessions);
      SetLength(Sessions, sessionId + 1);
      Sessions[sessionId] := TUserSession.Create(cid);
    end;
    //... формируем и отсылаем ответ (_зависит_ от сеанса)
    Sessions[sessionId].handleMessage(userinfo, userMsg);
  except
    on e: exception do begin
      SendMsg(cid, WideString('Проблема с ChatBot.dll: '+e.message) );
      Windows.Beep(800, 50);
    end;
  end;
end;

//обрабатываем уведомления от клиента DC
procedure OnRecvMessage2 (msgid:integer; objid:PWideChar; param:pointer; paramsize: Cardinal); stdcall;
var
  cid, msg,answer,userinfo: WideString;
  i,sid: integer;
begin
     if (TrayForm <> nil) and (not TrayForm.State) then exit;

  //bot callback for receiving notifications from client
  case CODES(msgid) of
    //-------------------------------------------------------------------------
    RECV_PM_NEW://поступила личка (открыто новое окно PM)
    begin
      msg := pWideChar(param);//текст поступившей лички
      //извлекаем информацию об авторе PM и обрабатываем личку
      userinfo := pWideChar(QueryInfo2(integer(QUERY_USER_BY_CID), objid, nil, 0));
      SendAnswer(userinfo, msg);
    end;
    //-------------------------------------------------------------------------
    RECV_PM:
    //поступила личка (окно PM уже открыто)
    begin
      msg := pWideChar(param);//текст поступившей лички
      //извлекаем информацию об авторе PM и обрабатываем личку
      userinfo := pWideChar(QueryInfo2(integer(QUERY_USER_BY_CID), objid, nil, 0));
      SendAnswer(userinfo, msg);
    end;
    //-------------------------------------------------------------------------
    RECV_CM:
    //поступило сообщение от пользователя в главном чате
    ;
    //-------------------------------------------------------------------------
    RECV_UPDATE:
    //пользователь изменил данные(шару, слоты) или
    //новый пользователь присоединился к хабу;
    ;
    //-------------------------------------------------------------------------
    RECV_PART:
    //пользователь отсоединился от хаба
    ;
    //-------------------------------------------------------------------------
    RECV_CONNECT:
    //клиент соединился с хабом
    ;
    //-------------------------------------------------------------------------
    RECV_DISCONNECT:
    //клиент отсоединился от хаба
    ;
    //-------------------------------------------------------------------------
end;
end;

//инициализация _init struct
function init(var _init: TBotInit): boolean; stdcall;
var
  exe: array [0..512] of char;
begin
  Randomize;
  _init.botId := 'xBot';
  _init.botVersion := '0.1';

  //проверяем версию botapi, поддерживаемую клиентом
  //передаем клиенту адрес процедуры обработки уведомлений
  //получаем адреса процедур запроса и отправки сообщения
  case _init.apiVersion of
    1:begin
           // TODO что-то сделать если botapi2 не подд-ся клиентом
           exit;
      end;
    2: begin
        _init.OnRecvMessage2:= OnRecvMessage2;
        SendProc2 := _init.OnSendMessage2;
        QueryInfo2:= _init.QueryInfo;
      end;
  end;
  
  result := true;//пока все идет хорошо
  //создаем и заполняем словарь фраз-ответов
  try
    GetModuleFilename(0, exe, 512);
    dict := TDict.init(ExtractFilePath(string(exe)) + 'Dictionary.ini');
  except
    on e: exception do
    begin
      Windows.Beep(400, 50);
      Windows.MessageBox(0, pchar('(ChatBot.dll) Проблема со словарем '#13#10+e.message),
                         'error', MB_OK or MB_ICONERROR);
      result := false;
    end;
  end;
end;

end.