//******************************************************************************
{*
* @file session.pas
* @author xmm
* @date 03-июн-2008
* @brief сессии лички
* @details описан класс предстал€ющий один сеанс лички
*}
//******************************************************************************
unit session;

interface

uses
  SysUtils, Classes, Windows, Dictionary, settings, botdef, Forms;

type
  /// сеанс лички
  {*
    предстал€ет уникальный сеанс лички
  *}
  TUserSession = class
  public
    /// cid того, с кем общаемс€
    cid: WideString;
    /// вс€ информаци€ о нем
    params: TStringList;
    /// обработать сообщение
    procedure handleMessage(const params, msg: WideString);
    /// получить cid по структуре userinfo
    class function getCID(const params: WideString): WideString;
    ///конструктор
    constructor Create(cid: WideString);
    //деструктор
    destructor Done;
    ///получить параметр из структуры userinfo
    function GetVariable(const v: WideString): WideString;
  private
    hShutdown: THandle;
    ///контекст (ответы адресата в рамках сессии)
    replies: array of WideString;
    ///маркер использованных фраз-ответов
    used: TBoolArray;
    ///подстановка фактических значений во фразу, таких, как $NICK
    function processVariables(const msg: WideString): WideString;
    ///отсылка сообщени€ через заданный промежуток времени
    procedure SendDelayedMsg(const msg: WideString; timeout: integer);
  end;

var
  sessions: array of TUserSession;

implementation

uses bot, tray;

destructor TUserSession.done;
begin
  SetEvent(hShutdown);
  Params.Free;
  Sleep(200);
  CloseHandle(hShutdown);
end;

//******************************************************************************
{* @author xmm
* @date 03-июн-2008 : Original Version 
* @param cid WideString cid адресата
* @result None
* @brief конструктор
*}
//******************************************************************************
constructor TUserSession.Create(cid: WideString);
begin
  hShutdown := CreateEvent(nil, true, false, nil);
  Self.cid := cid;
  replies := nil;
  SetLength(used, dict.count);
  params := TStringList.Create;
end;

//******************************************************************************
{* @author xmm
* @date 03-июн-2008 : Original Version 
* @param v WideString const  
* @result WideString запршенный параметр
* @brief получить параметр из структуры userinfo
*}
//******************************************************************************
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

//******************************************************************************
{* @author xmm
* @date 03-июн-2008 : Original Version 
* @param params WideString const  
* @result WideString запрошенный cid
* @brief получить cid по структуре userinfo
*}
//******************************************************************************
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

//******************************************************************************
{* @author xmm
* @date 03-июн-2008 : Original Version 
* @param params WideString const структура userinfo
* @param msg WideString const текст вход€щей лички
* @result None
* @brief обработать сообщение
*}
//******************************************************************************
procedure TUserSession.handleMessage(const params, msg: WideString);
var
  possible: TPhrases;
  current: TPhrase;
  i, timeout: integer;

  nick, match: WideString;
  favouriteUser, addActions:boolean;
begin
  Self.Params.Text := StringReplace(params, '|', #13#10, [rfReplaceAll]);

  //определим ник автора PM
  nick:= self.params.Values['NICK'];
  //если друг, то бот молчит
  favouriteUser:= self.params.Values['ISFAV']='1';
  if favouriteUser then exit;

  //сохран€ем в истории вход€щую реплику автора PM
  SetLength(replies, length(replies)+1);
  replies[length(replies)-1] := msg;


  //определ€м список возможных ответов
  possible := dict.GetMatched(msg, used);
  if (length(possible) = 0) then begin
    for i := low(used) to high(used) do
      used[i] := false;
    possible := dict.GetMatched(msg, used);
  end;

  //если поход€щих ответов нет
  //(нет ответов с пустым шаблоном, а ни одна с непустым не подоошла)
  if length(possible)=0 then exit;

  //выбираем произвольный, помечаем его использованным
  current := dict.GetRandom(possible);
  used[current.id] := true;
  //замена спец переменных в шаблоне ответа ($NICK, etc...) на фактические значени€
  current.phrase := processVariables(current.phrase);
  
  // TODO переделать во что-нибудь поиз€щнее
  //обработка дополнительных флагов (в игнор, закрыть окно PM, etc...)
  with current do begin
    addActions:=giveSlot or closeWnd or addToIngnore;
    if addActions then begin
      SendMsg(WideString(cid), WideString(current.phrase));
      match := '—овпадение по шаблону: '+current.match;
      if current.closeWnd then begin
         SendProc2( integer(USER_CLOSE), pWideChar(cid), nil, 0 );
         if g_baloonInfo then TrayForm.ShowInfoMsg('«акрыта личка ' + nick, match);
      end;
      if current.giveSlot then begin
         SendProc2( integer(USER_SLOT), pWideChar(cid), @g_slotTimeout, sizeof(g_slotTimeout) );
         if g_baloonInfo then TrayForm.ShowInfoMsg('¬ыдан слот ' + nick, match);
      end;
      if current.addToIngnore then begin
         SendProc2( integer(USER_IGNORE), pWideChar(cid), @WIN32_TRUE, sizeof(WIN32_TRUE) );
         if g_baloonInfo then TrayForm.ShowInfoMsg(nick + ' добавлен в игнор.', match);
      end;
      exit;
    end;
  end;

  //if (random(10) > 8) then exit;

  timeout := length(current.phrase)*100;
  inc(timeout, g_answrDelay*1000);
  //ответ с задержкой
  SendDelayedMsg(current.phrase, timeout);
end;

//******************************************************************************
{* @author xmm
* @date 03-июн-2008 : Original Version 
* @param msg WideString const фраза
* @result WideString фраза с подставленными значени€ми
* @brief подстановка фактических значений во фразу, таких, как $NICK
*}
//******************************************************************************
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

/// отложенна€ личка
type
  TMsgRecord = record
    ///текст, cid адресата
    msg, cid: WideString;
    ///задержка
    timeout: integer;
    ///дескриптор таймера
    waitHandle: THandle;
  end;

/// поток, отсылающий отложенную личку
procedure DelayedMsgThread(cid: pointer); stdcall;
var
  r: ^TMsgRecord;
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
