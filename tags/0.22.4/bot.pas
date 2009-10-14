{
    flybot - simple extension for flylink dc client
    Copyright (C) 2008 xmm

    This library is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 2.1 of the License, or (at your option)
    any later version.

    This library is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details:
    http://www.opensource.org/licenses/lgpl-2.1.php
}
unit bot;

interface

uses
  SysUtils, Classes, Windows, Session, settings, def;

var
  SendProc2: TSendMessage2;///var адрес функции API в пространстве клиента
  QueryInfo2: TQueryInfo;

  procedure OnRecvMessage2 (msgid:integer; objid:PWideChar; param: pointer; paramsize: Cardinal); stdcall;
  function init(var _init: TBotInit): boolean; stdcall;
  procedure SendMsg(cid, msg: WideString);
  
  exports init;

implementation

uses
  dictionary, tray;

//******************************************************************************
{* @author xmm
* @date 03-июн-2008 : Original Version
* @param cid WideString id адресата
* @param msg WideString текст лички
* @result None
* @brief посылает личку пользователю
*}
//******************************************************************************
procedure SendMsg(cid, msg: WideString);
begin
  if length(msg)<>0 then
      SendProc2( integer(SEND_PM), pWideChar(cid), pWideString(msg), sizeof(msg) );
end;

//******************************************************************************
{* @author xmm
* @date 03-июн-2008 : Original Version
* @param userinfo WideString структура, содержаща€ информацию о приславшем личку
* @param userMsg WideString текст пришедшей PM
* @result None
* @brief посылает ответ/добавл€ет в игнор/etc
*}
//******************************************************************************
procedure SendAnswer(userinfo, userMsg: WideString);
var
  cid: WideString;
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
  msg,userinfo: WideString;
begin
  if (TrayForm = nil) then
    exit;

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
* @date 03-июн-2008 : Original Version
* @param _init TBotInit var структура, содержаща€ адреса функций API, вызываемых ботом
* @result boolean
* @brief вызываетс€ клиентом при старте
*}
//******************************************************************************
function init(var _init: TBotInit): boolean; stdcall;
begin
  Randomize;
  _init.botId := 'flybot';
  _init.botVersion := '0.2';

  //провер€ем версию botapi, поддерживаемую клиентом
  //передаем клиенту адрес процедуры обработки уведомлений
  //получаем адреса процедур запроса и отправки сообщени€
  case _init.apiVersion of
    1:begin
           // если botapi2 не подд-с€ клиентом
           TrayForm.ShowErrMsg('botapi2 не поддерживаетс€ клиентом. ќбновите клиент.');
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
  except
    on e: exception do
    begin
      TrayForm.ShowErrMsg('ѕроблема со словарем ' + #13 + e.message);
      result := false;
    end;
  end;
end;

end.