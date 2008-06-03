//******************************************************************************
{*
* @file bot.pas
* @author xmm
* @date 02-июн-2008
* @brief главный модуль
* @details содержит процедуру инициализации
*}
//******************************************************************************
unit bot;

interface

uses
  SysUtils, Classes, Windows, Session, settings, botdef;

var
  SendProc: tSendMessage;
  SendProc2: tSendMessage2;
  QueryInfo2: tQueryInfo;

  procedure OnRecvMessage2 (msgid:integer; objid:PWideChar; param: pointer; paramsize: Cardinal); stdcall;
  function init(var _init: TBotInit): boolean; stdcall;
  procedure SendMsg(cid, msg: WideString);
  function GetModulePath:string;
  
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
    //... не нашли, т.к. нова€ личка -> создаем структуру, описывающиую сеанс
    if (sessionId < 0) then begin
      sessionId := length(Sessions);
      SetLength(Sessions, sessionId + 1);
      Sessions[sessionId] := TUserSession.Create(cid);
    end;
    //... формируем и отсылаем ответ (_зависит_ от сеанса)
    Sessions[sessionId].handleMessage(userinfo, userMsg);
  except
    on e: exception do begin
      SendMsg(cid, WideString('ѕроблема с ChatBot.dll: '+e.message) );
      Windows.Beep(800, 50);
    end;
  end;
end;


//******************************************************************************
{* @author xmm
* @date 02-июн-2008 : Original Version
* @param msgid integer тип сообщени€
* @param objid PWideChar «ависит от сообщени€ (обычно cid юзера, €вившегос€ причиной сообщени€)
* @param param pointer «ависит от сообщени€
* @param paramsize Cardinal размер переданных праметров.
* @result None
* @brief обрабатывает уведомлени€ от клиента DC
*}
//******************************************************************************
procedure OnRecvMessage2 (msgid:integer; objid:PWideChar; param:pointer; paramsize: Cardinal); stdcall;
var
  cid, msg,answer,userinfo: WideString;
  i,sid: integer;
begin
     if (TrayForm <> nil) and (not TrayForm.botEnabled) then exit;

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
    //поступило сообщение от пользовател€ в главном чате
    ;
    //-------------------------------------------------------------------------
    RECV_UPDATE:
    //пользователь изменил данные(шару, слоты) или
    //новый пользователь присоединилс€ к хабу;
    ;
    //-------------------------------------------------------------------------
    RECV_PART:
    //пользователь отсоединилс€ от хаба
    ;
    //-------------------------------------------------------------------------
    RECV_CONNECT:
    //клиент соединилс€ с хабом
    ;
    //-------------------------------------------------------------------------
    RECV_DISCONNECT:
    //клиент отсоединилс€ от хаба
    ;
    //-------------------------------------------------------------------------
end;
end;

//******************************************************************************
{* @author xmm
* @date 02-июн-2008 : Original Version
* @param _init TBotInit var структура, содержаща€ адреса функций API, вызываемых ботом
* @result boolean
* @brief функци€ вызываетс€ клиентом при старте
*}
//******************************************************************************
function init(var _init: TBotInit): boolean; stdcall;
begin
  Randomize;
  _init.botId := 'xBot';
  _init.botVersion := '0.1';

  //провер€ем версию botapi, поддерживаемую клиентом
  //передаем клиенту адрес процедуры обработки уведомлений
  //получаем адреса процедур запроса и отправки сообщени€
  case _init.apiVersion of
    1:begin
           // TODO что-то сделать если botapi2 не подд-с€ клиентом
           exit;
      end;
    2: begin
        _init.OnRecvMessage2:= OnRecvMessage2;
        SendProc2 := _init.OnSendMessage2;
        QueryInfo2:= _init.QueryInfo;
      end;
  end;
  
  result := true;//пока все идет хорошо
  //создаем и заполн€ем словарь фраз-ответов
  try
    dict := TDict.init(GetModulePath + DICTIONARY_FILENAME);
    ReadSettings;
  except
    on e: exception do
    begin
      TrayForm.ShowErrMsg('ѕроблема со словарем ' + #13 + e.message);
      result := false;
    end;
  end;
end;


//******************************************************************************
{* @author xmm
* @date 02-июн-2008 : Original Version
* @result string
* @brief возвращает путь к исполн€емому модулю клиента
*}
//******************************************************************************
function GetModulePath:string;
var exe:array[0..512]of char;
begin
  GetModuleFilename(0, exe, 512);
  result:=ExtractFilePath(string(exe))
end;

end.