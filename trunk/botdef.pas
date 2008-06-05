//******************************************************************************
{*
* @file botdef.pas
* @author xmm
* @date 03-июн-2008
* @brief общие определения
*}
//******************************************************************************
unit botdef;

interface
uses Windows;

type
  TSendMessage = procedure(params: PWideChar; message: PWideChar); stdcall;
  TRecvMessage = procedure(params: PWideChar; message: PWideChar); stdcall;

  TSendMessage2 = function(msgid:integer; objid:PWideChar; param:pointer; paramsize: Cardinal):boolean;stdcall;
  TRecvMessage2 = procedure(msgid:integer; objid:PWideChar; param:pointer; paramsize: Cardinal);stdcall;

  TQueryInfo = function(qryid:integer; objid:PWideChar;const param:pointer; paramsize:integer):pointer;stdcall;
  TFreeInfo  = procedure(info:pointer);stdcall;
type
/// тип события
CODES =(
                SEND_PM         = 0,
                SEND_CM         = 1,
                USER_CLOSE      = 2,
                USER_IGNORE     = 3,
                USER_BAN        = 4,
                USER_SLOT       = 5,
                DL_MAGNET       = 6,

                RECV_PM_NEW     = 40,
                RECV_PM         = 41,
                RECV_CM         = 42,
                //RECV_JOIN     = 43,
                RECV_PART       = 44,
                RECV_UPDATE     = 45,
                RECV_CONNECT    = 46,
                RECV_DISCONNECT = 47,

                QUERY_USER_BY_CID       = 80,
                QUERY_HUB_BY_URL        = 81,
                QUERY_CONNECTED_HUBS    = 82,
                QUERY_HUB_USERS         = 83,
                QUERY_SELF              = 84,
                QUERY_RUNNING_UPLOADS   = 85,
                QUERY_QUEUED_UPLOADS    = 86,
                QUERY_DOWNLOADS         = 87,

                LAST
        );
/// структура, передаваемая при старте клиентом боту
  TBotInit = record
    apiVersion:         DWORD;
    appName:            PCHAR;
    appVersion:         PCHAR;
    OnSendMessage:      TSendMessage;
    OnRecvMessage:      TRecvMessage;
    botId:              PCHAR;
    botVersion:         PCHAR;
    OnSendMessage2:     TSendMessage2;
    OnRecvMessage2:     TRecvMessage2;
    QueryInfo:          TQueryInfo;
    FreeInfo:           TFreeInfo;
  end;
  const SLOT_TIMEOUT_SEC  : DWORD = 600;
        WIN32_TRUE        : BOOL  = true;
        BOT_VERSION_STRING: string='flybot v0.22';
        DICTIONARY_FILENAME:string='dictionary.ini';
        BOT_SETTINGS_FILE:string='flybot.ini';
        DEFAULT_SLOT_TIMEOUT:integer=600;
        DEFAULT_ANSWER_DELAY:integer=6;
implementation
end.
