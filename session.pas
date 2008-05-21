unit session;

interface

uses
  SysUtils, Classes, Windows, Dictionary, settings, botdef, Forms;

type
  TUserSession = class
  public
    cid: WideString;
    params: TStringList;
    procedure handleMessage(const params, msg: WideString);
    class function getCID(const params: WideString): WideString;
    constructor Create(cid: WideString);
    destructor Done;
    function GetVariable(const v: WideString): WideString;
  private
    hShutdown: THandle;
    replies: array of WideString;
    used: TBoolArray;
    function processVariables(const msg: WideString): WideString;
    procedure SendDelayedMsg(const msg: WideString; timeout: integer);
  end;

var
  sessions: array of TUserSession;

implementation

uses bot;

destructor TUserSession.done;
begin
  SetEvent(hShutdown);
  Params.Free;
  Sleep(200);
  CloseHandle(hShutdown);
end;

constructor TUserSession.Create(cid: WideString);
begin
  hShutdown := CreateEvent(nil, true, false, nil);
  Self.cid := cid;
  replies := nil;
  SetLength(used, dict.count);
  params := TStringList.Create;
end;

function TUserSession.GetVariable(const v: WideString): WideString;
var
  id, res: WideString;
begin
  id := AnsiUpperCase(v);
  if (id = 'LAST') then res := replies[length(replies)-1]
  else if (id = 'HISTORY') then res := replies[random(length(replies))]
  else res := params.Values[id];
  result := res;
end;

class function TUserSession.getCID(const params: WideString): WideString;
var
  i: integer;
begin
  i := pos('CID=', params);
  if (i > 0) and (i+39+4 < length(params)) then
    result := copy(params, i+4, 39)
  else
    result := '';
end;

procedure TUserSession.handleMessage(const params, msg: WideString);
var
  possible: TPhrases;
  current: TPhrase;
  i, timeout: integer;

  nick: WideString;
  addActions:boolean;
begin
  Self.Params.Text := StringReplace(params, '|', #13#10, [rfReplaceAll]);

  //определим ник автора PM
  nick:= self.params.Values['NICK'];

  //сохраняем входящую реплику автора PM
  SetLength(replies, length(replies)+1);
  replies[length(replies)-1] := msg;

  //if (GetVariable('BOT') = '1') then exit;

  //определям список возможных ответов
  possible := dict.GetMatched(msg, used);
  if (length(possible) = 0) then begin
    for i := low(used) to high(used) do
      used[i] := false;
    possible := dict.GetMatched(msg, used);
  end;

  //выбираем произвольный, помечаем его использованным
  current := dict.GetRandom(possible);
  used[current.id] := true;
  //замена спец переменных в шаблоне ответа ($NICK, etc...) на фактические значения
  current.phrase := processVariables(current.phrase);
  
  // TODO переделать во что-нибудь поизящнее
  with current do begin
    addActions:=giveSlot or closeWnd or addToIngnore;
    if addActions then begin
      SendMsg(WideString(cid), WideString(current.phrase));
      if current.giveSlot then
         SendProc2( integer(USER_SLOT), pWideChar(cid), @g_slotTimeout, sizeof(g_slotTimeout) );
      if current.closeWnd then
         SendProc2( integer(USER_CLOSE), pWideChar(cid), nil, 0 );
      if current.addToIngnore then
         SendProc2( integer(USER_IGNORE), pWideChar(cid), @WIN32_TRUE, sizeof(WIN32_TRUE) );
      exit;
    end;
  end;

  //if (random(10) > 8) then exit;

  timeout := length(current.phrase)*100;
  if (random(5) = 0) then inc(timeout, g_answrDelay);
  SendDelayedMsg(current.phrase, timeout);
end;

function TUserSession.processVariables(const msg: WideString): WideString;
var
  i, ps: integer;
  varname: string;
begin
  result := '';
  i := 1;
  while (i <= length(msg)) do begin
    if (i > length(msg)-3) or (msg[i] <> '$') and (msg[i+1] <> '(') then
      result := result + msg[i]
    else begin
      ps := i+2;
      while (ps <= length(msg)) and (msg[ps] <> ')') do inc(ps);
      if (ps > length(msg)) then
        result := result + msg[i]
      else begin
        varname := copy(msg, i+2, ps-i-2);
        result := result + GetVariable(varname);
        i := ps;
      end;
    end;
    inc(i);
  end;
end;

type
  TMsgRecord = record
    msg, cid: WideString;
    timeout: integer;
    waitHandle: THandle;
  end;

procedure DelayedMsgThread(cid: pointer); stdcall;
var
  r: ^TMsgRecord;
  current:TPhrase;
begin
  r := cid;
  if (WaitForSingleObject(r.waitHandle, r.timeout) = WAIT_TIMEOUT) and (Assigned(SendProc2)) then
    SendMsg(WideString(r.cid), WideString(r.msg));
    Dispose(r);
end;

procedure TUserSession.SendDelayedMsg(const msg: WideString; timeout: integer);
var
  r: ^TMsgRecord;
  tid: DWORD;
begin
  New(r);
  r.msg := msg;
  r.cid := cid;
  r.timeout := timeout;
  r.waitHandle := hShutDown;
  CloseHandle(CreateThread(nil, 0, @DelayedMsgThread, r, 0, tid));
end;

procedure ShutDown;
var i: integer;
begin
  for i := 0 to length(sessions)-1 do
    sessions[i].Free;
  sessions := nil;
end;

initialization
  sessions := nil;
finalization
  ShutDown;
end.
