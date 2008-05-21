unit dictionary;

interface

uses
  SysUtils, Classes, Windows, RegExpr, botdef, tray;

type
  TPhrase = record
    id, basePrio: integer;
    match: string;
    phrase: string;
    closeWnd,giveSlot,addToIngnore: boolean;
  end;

  TPhrases = array of TPhrase;
  TBoolArray = array of boolean;

type
  TDict = class
  private
    dicFilename:string;
    function getPhraseCount: integer;
  public
    allphrases: TPhrases;
    property count: integer read getPhraseCount;
    
    function getMatched(const msg: WideString; const used: TBoolArray): TPhrases;
    function Reload: boolean;

    class function GetRandom(const  from: TPhrases): TPhrase;
    constructor init(const ini: string);
  end;

   
var
  dict: TDict;
  
implementation

function TDict.Reload: boolean;
var
  exe: array [0..512] of char;
begin
  result:=true;
  try
    GetModuleFilename(0, exe, 512);
    dict := TDict.init(ExtractFilePath(string(exe)) + DICTIONARY_FILENAME);
  except
    on e: exception do
    begin
      TrayForm.ShowErrMsg('Проблема со словарем ' + #13 + e.message);
      result := false;
    end;
  end;
end;

function TDict.getPhraseCount: integer;
begin
  result := length(allphrases);
end;

//сверяем входящую фразу со списком неиспользованных шаблонов
function TDict.getMatched(const msg: WideString; const used: TBoolArray): TPhrases;
var
  i,dst, dstOther: integer;
  t: WideString;
  r : TRegExpr;
  resMatched, resOther:TPhrases;
begin
  // TODO разобраться удобно ли учитывать регистр
  t := AnsiLowerCase(msg);
  resMatched := nil; resMatched:=nil;
  dst := 0; dstOther:=0;
  for i := 0 to count-1 do begin
    if (used[i]) then continue;//шаблон уже использовался
    
    r := TRegExpr.Create;
    r.Expression := allphrases[i].match;

    if (allphrases[i].match = '')then begin
    //добавляем фразу в список фраз с пустым шаблоном
      SetLength(resOther, dstOther+1);
      resOther[dstOther] := allphrases[i];
      inc(dstOther);
    end
    else
      try
        if (r.Exec(t)) then begin
        //добавляем фразу в список фраз с непустым шаблоном
          SetLength(resMatched, dst+1);
          resMatched[dst] := allphrases[i];
          inc(dst);
        end;
      except
        on e: exception do  begin
          SetLength(resMatched, 1);
          resMatched[dst]:= allphrases[i];
          resMatched[dst].phrase:=e.message;
          inc(dst);
          break;
        end;
      end;
  end;
  //если нет подходящих фраз с непустым шаблоном
  if dst=0 then
    result:=resOther
  //используем фразу одну из фраз-отмазок (с пустым шаблоном)
  else result:=resMatched;
end;

class function TDict.GetRandom(const from: TPhrases): TPhrase;
var
  sumPrio, msgId, i: integer;
begin
  sumPrio := 0;
  for i := low(from) to high(from) do
    sumPrio := sumPrio + round(1000/from[i].basePrio);
  msgId := random(sumPrio);
  //if (msgId >= sumPrio) then msgId := sumPrio-1;
  i := 0; sumPrio := 0;
  while (true) do begin
    sumPrio := sumPrio + round(1000/from[i].basePrio);
    if (msgId < sumPrio) then break
    else inc(i);
  end;
  result := from[i];
end;

//инициализация словаря содержимым файла
constructor TDict.init(const ini: string);
var
  f: TextFile;
  s, matchtxt,answer, params: string;
  dst, ps, prio: integer;
begin
  allphrases := nil;
  AssignFile(f, ini);
  Reset(f);
  try
    dicFilename:=ini;
    dst := 0;
    while not EOF(f) do begin
      Readln(f, s);
      //пропуск комментариев и слишком коротких строк
      if (length(s) < 3) or (s[1] = ';') then continue;
      // TODO пользовать RegExpr для сплиттинга строки
      ps := pos('/', s);
      if (ps <= 0) then continue;
      prio := StrToInt(copy(s, 1, ps-1));
      delete(s, 1, ps);
      ps := pos('/', s);
      if (ps <= 0) then continue;
      matchtxt := copy(s, 1, ps-1);
      delete(s, 1, ps);
      ps := pos('/', s);
      if (ps <= 0) then answer := s
      else begin
          answer := copy(s, 1, ps-1);
          delete(s, 1, ps);
          params:= s;
      end;
      params := copy(s, 1, ps-1);
      delete(s, 1, ps);

      SetLength(allphrases, dst+1);
      with allphrases[dst] do begin
        id                 := dst;
        basePrio           := prio;
        match              := matchtxt;
        closeWnd           := (pos('c',params) > 0);
        giveSlot           := (pos('s',params) > 0);
        addToIngnore       := (pos('i',params) > 0);
        phrase             := answer;
      end;
      inc(dst);
    end;
  finally
    CloseFile(f);
  end;
end;

end.
