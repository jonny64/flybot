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

  
//�������� ����� ������������ � �������� cid
procedure SendMsg(cid, msg: WideString);
begin
      SendProc2( integer(SEND_PM), pWideChar(cid), pWideString(msg), sizeof(msg) );
end;

//���������� ����������� �� �������
procedure SendAnswer(userinfo, userMsg: WideString);
var
  cid,answer: WideString;
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
      Windows.Beep(800, 50);
    end;
  end;
end;

//������������ ����������� �� ������� DC
procedure OnRecvMessage2 (msgid:integer; objid:PWideChar; param:pointer; paramsize: Cardinal); stdcall;
var
  cid, msg,answer,userinfo: WideString;
  i,sid: integer;
begin
     if (TrayForm <> nil) and (not TrayForm.State) then exit;

  //bot callback for receiving notifications from client
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

//������������� _init struct
function init(var _init: TBotInit): boolean; stdcall;
var
  exe: array [0..512] of char;
begin
  Randomize;
  _init.botId := 'xBot';
  _init.botVersion := '0.1';

  //��������� ������ botapi, �������������� ��������
  //�������� ������� ����� ��������� ��������� �����������
  //�������� ������ �������� ������� � �������� ���������
  case _init.apiVersion of
    1:begin
           // TODO ���-�� ������� ���� botapi2 �� ����-�� ��������
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
    GetModuleFilename(0, exe, 512);
    dict := TDict.init(ExtractFilePath(string(exe)) + 'Dictionary.ini');
  except
    on e: exception do
    begin
      Windows.Beep(400, 50);
      Windows.MessageBox(0, pchar('(ChatBot.dll) �������� �� �������� '#13#10+e.message),
                         'error', MB_OK or MB_ICONERROR);
      result := false;
    end;
  end;
end;

end.