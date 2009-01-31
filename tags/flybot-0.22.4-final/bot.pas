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
  SendProc2: TSendMessage2;///var ����� ������� API � ������������ �������
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
* @date 03-���-2008 : Original Version
* @param cid WideString id ��������
* @param msg WideString ����� �����
* @result None
* @brief �������� ����� ������������
*}
//******************************************************************************
procedure SendMsg(cid, msg: WideString);
begin
  if length(msg)<>0 then
      SendProc2( integer(SEND_PM), pWideChar(cid), pWideString(msg), sizeof(msg) );
end;

//******************************************************************************
{* @author xmm
* @date 03-���-2008 : Original Version
* @param userinfo WideString ���������, ���������� ���������� � ���������� �����
* @param userMsg WideString ����� ��������� PM
* @result None
* @brief �������� �����/��������� � �����/etc
*}
//******************************************************************************
procedure SendAnswer(userinfo, userMsg: WideString);
var
  cid: WideString;
  i,sessionId: integer;
begin
  try
     //����� ������� �� ����������� (������)
    cid := TUserSession.getCID(userinfo);
    sessionId := -1;
    // ... ���� ���������, ����������� ����� sic! �������� ����� (�����!?)
    for i := 0 to length(sessions)-1 do
      if (sessions[i].cid = cid) then sessionId := i;
    //... �� �����, �.�. ����� ����� -> ������� ���������, ������������ �����
    if (sessionId < 0) then begin
      sessionId := length(Sessions);
      SetLength(Sessions, sessionId + 1);
      Sessions[sessionId] := TUserSession.Create(cid);
    end;
    //... ��������� � �������� ����� (_�������_ �� ������)
    Sessions[sessionId].handleMessage(userinfo, userMsg);
  except
    on e: exception do begin
      SendMsg(cid, WideString('�������� � ChatBot.dll: '+e.message) );
    end;
  end;
end;


//******************************************************************************
{* @author xmm
* @date 02-���-2008 : Original Version
* @param msgid integer ��� ���������
* @param objid PWideChar ������� �� ��������� (������ cid �����, ���������� �������� ���������)
* @param param pointer ������� �� ���������
* @param paramsize Cardinal ������ ���������� ���������.
* @result None
* @brief ������������ ����������� �� ������� DC
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
    RECV_PM_NEW://��������� ����� (������� ����� ���� PM)
    begin
      msg := pWideChar(param);//����� ����������� �����
      //��������� ���������� �� ������ PM � ������������ �����
      userinfo := pWideChar(QueryInfo2(integer(QUERY_USER_BY_CID), objid, nil, 0));
      SendAnswer(userinfo, msg);
    end;
    //-------------------------------------------------------------------------
    RECV_PM:
    //��������� ����� (���� PM ��� �������)
    begin
      msg := pWideChar(param);//����� ����������� �����
      //��������� ���������� �� ������ PM � ������������ �����
      userinfo := pWideChar(QueryInfo2(integer(QUERY_USER_BY_CID), objid, nil, 0));
      SendAnswer(userinfo, msg);
    end;
    //-------------------------------------------------------------------------
    RECV_CM:
    //��������� ��������� �� ������������ � ������� ����
    ;
    //-------------------------------------------------------------------------
    RECV_UPDATE:
    //������������ ������� ������(����, �����) ���
    //����� ������������ ������������� � ����;
    ;
    //-------------------------------------------------------------------------
    RECV_PART:
    //������������ ������������ �� ����
    ;
    //-------------------------------------------------------------------------
    RECV_CONNECT:
    //������ ���������� � �����
    ;
    //-------------------------------------------------------------------------
    RECV_DISCONNECT:
    //������ ������������ �� ����
    ;
    //-------------------------------------------------------------------------
end;
end;

//******************************************************************************
{* @author xmm
* @date 03-���-2008 : Original Version
* @param _init TBotInit var ���������, ���������� ������ ������� API, ���������� �����
* @result boolean
* @brief ���������� �������� ��� ������
*}
//******************************************************************************
function init(var _init: TBotInit): boolean; stdcall;
begin
  Randomize;
  _init.botId := 'flybot';
  _init.botVersion := '0.2';

  //��������� ������ botapi, �������������� ��������
  //�������� ������� ����� ��������� ��������� �����������
  //�������� ������ �������� ������� � �������� ���������
  case _init.apiVersion of
    1:begin
           // ���� botapi2 �� ����-�� ��������
           TrayForm.ShowErrMsg('botapi2 �� �������������� ��������. �������� ������.');
           exit;
      end;
    2: begin
        _init.OnRecvMessage2:= OnRecvMessage2;
        SendProc2 := _init.OnSendMessage2;
        QueryInfo2:= _init.QueryInfo;
      end;
  end;
  
  result := true;//���� ��� ���� ������
  //������� � ��������� ������� ����-�������
  try
    dict := TDict.init(GetModulePath + DICTIONARY_FILENAME);
  except
    on e: exception do
    begin
      TrayForm.ShowErrMsg('�������� �� �������� ' + #13 + e.message);
      result := false;
    end;
  end;
end;

end.