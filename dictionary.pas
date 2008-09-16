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
  i,nonEmptyCandidateCount, emptyCandidateCount: integer;
  receivedMsg: WideString;
  regExpParser: TRegExpr;
  nonEmptyCandidates, emptyCandidates: TPhrases;
begin
  receivedMsg := AnsiLowerCase(msg);

  nonEmptyCandidates := nil;
  nonEmptyCandidateCount := 0;
  emptyCandidateCount := 0;
  //составляем список фраз-кандидатов на ответ
  for i := 0 to count-1 do begin
    //использовавшийся шаблон пропускаем
    if (used[i]) then continue;

    regExpParser := TRegExpr.Create;
    regExpParser.Expression := allphrases[i].match;

    if (allphrases[i].match = '')then begin
      //... добавляем фразу в список фраз с пустым шаблоном
      SetLength(emptyCandidates, emptyCandidateCount+1);
      emptyCandidates[emptyCandidateCount] := allphrases[i];
      inc(emptyCandidateCount);
    end
    else
      try
        if (regExpParser.Exec(receivedMsg)) then begin
        //... добавляем фразу в список фраз с непустым шаблоном
          SetLength(nonEmptyCandidates, nonEmptyCandidateCount+1);
          nonEmptyCandidates[nonEmptyCandidateCount] := allphrases[i];
          inc(nonEmptyCandidateCount);
        end;
      except
        on e: exception do  begin
          //... текст возникшей ошибки помещаем во фразу-ответ
          SetLength(nonEmptyCandidates, 1);
          nonEmptyCandidates[nonEmptyCandidateCount]:= allphrases[i];
          nonEmptyCandidates[nonEmptyCandidateCount].phrase:=e.message;
          inc(nonEmptyCandidateCount);
          break;
        end;
      end;
  end;

  //если нет подходящих фраз с непустым шаблоном
  if nonEmptyCandidateCount=0 then
    result:=emptyCandidates
  //используем фразу одну из фраз-отмазок (с пустым шаблоном)
  else result:=nonEmptyCandidates;
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
