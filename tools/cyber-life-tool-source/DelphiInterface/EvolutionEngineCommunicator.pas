{

    TEvolutionEngineCommunicator - Version 0.9b

    Copyright (C) 1999 Daniel Silverstone (Kinnison) of Creature Labs Ltd

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact Point: Daniel.Silverstone@creaturelabs.com
}

unit EvolutionEngineCommunicator;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs;

type

  PInterfaceRecord = ^TInterfaceRecord;
  TInterfaceRecord = packed record
    Magic      : array [0..3] of Char;
    ServerID   : Cardinal;
    ReturnCode : Integer;
    DataSize   : Cardinal;
    BufferSize : Cardinal;
    Pad        : Integer;
  end;

  TEvolutionEngineCommunicator = class(TComponent)
  private
    { Private declarations }
    FMutex,
    FRequestEvent,
    FResultEvent,
    FMappedFile : Cardinal;
    FInterface : PInterfaceRecord;
    FAfterInterface : Pointer;

    FConnected : boolean;
    FTransaction,
    FResults,
    FGameName  : String;

    FReturnCode : Integer;
  protected
    { Protected declarations }
    procedure DoSetConnected(aValue : boolean);
    procedure DoSetGameName(aValue : string);
    procedure DoSetTransaction(aValue : string);
    procedure DoTransactionWithServer;
    procedure DoIgnoreString(a:string);
    procedure DoIgnoreInt(a:integer);
  public

    { Public declarations }
    constructor Create(aOwner : TComponent);override;
    destructor Destroy; override;

    function Connect : boolean;
    procedure Disconnect;

  published
    { Published declarations }
    property GameName : String read FGameName write DoSetGameName;
    property Connected : boolean read FConnected write DoSetConnected;
    property Transaction : String read FTransaction write DoSetTransaction;
    property Results : String read FResults write DoIgnoreString;
    property ReturnCode : Integer read FReturnCode write DoIgnoreInt;
  end;

procedure Register;

implementation

procedure TEvolutionEngineCommunicator.DoIgnoreString(a:string);
begin
end;
procedure TEvolutionEngineCommunicator.DoIgnoreInt(a:integer);
begin;
end;

procedure TEvolutionEngineCommunicator.DoSetTransaction(aValue : string);
begin
  FTransaction := aValue;
  if (FConnected) then
  begin
    DoTransactionWithServer;
  end;
end;

procedure TEvolutionEngineCommunicator.DoTransactionWithServer;
var
 SerPID : Cardinal;
 TwoHands : array[0..1] of Cardinal;
 tempString : string;
begin
  FResults := '[***ERROR***]';
  if (FConnected = false) then
    exit;
  if (Cardinal(Length(FTransaction)) > FInterface^.BufferSize) then
    exit;
  if (FInterface^.Magic <> 'c2e@') then
    exit;
  if ( WaitForSingleObject(FMutex, 500) <> WAIT_OBJECT_0 ) then
    exit;
  // Check if the first four chars are scrp
  if (AnsiCompareText(Copy(FTransaction,1,4),'scrp') = 0) then
    tempString := FTransaction + #0
  else
    tempString := 'execute' + #10 + FTransaction + #0;
  CopyMemory(FAfterInterface,PChar(tempString),Length(tempString));
  FInterface^.DataSize := Length(tempString);
  ResetEvent(FResultEvent);
  SetEvent(FRequestEvent);
  SerPID := OpenProcess(PROCESS_ALL_ACCESS, false,FInterface^.ServerID);
  TwoHands[0] := FResultEvent;
  TwoHands[1] := SerPID;
  if ( WaitForMultipleObjects( 2, @(TwoHands[0]), false, INFINITE) <> WAIT_OBJECT_0 ) then
  begin
    CloseHandle(SerPID);
    ReleaseMutex( FMutex );
    exit;
  end;
  CloseHandle(SerPID);
  SetLength(FResults, FInterface^.DataSize - 1);
  CopyMemory(PChar(FResults),FAfterInterface,FInterface^.DataSize - 1);
  FReturnCode := FInterface^.ReturnCode;
  ReleaseMutex(FMutex);
end;

procedure TEvolutionEngineCommunicator.DoSetGameName(aValue : string);
var
 was : boolean;
begin
  was := FConnected;
  if (FConnected) then
    Disconnect;
  FGameName := aValue;
  if (was) then
    Connect;
end;

procedure TEvolutionEngineCommunicator.DoSetConnected(aValue : boolean);
begin
  if (aValue = false) then
    Disconnect
  else
    Connect;
end;

constructor TEvolutionEngineCommunicator.Create(aOwner : TComponent);
begin
  inherited Create(aOwner);
  FMutex := 0;
  FRequestEvent := 0;
  FResultEvent := 0;
  FMappedFile := 0;
  FInterface := nil;
  FConnected := false;
  FGameName := '';
  FTransaction := '';
  FResults := '[***ERROR***]';
end;

destructor TEvolutionEngineCommunicator.Destroy;
begin
  if (FConnected) then
    Disconnect;
  inherited Destroy;
end;

function TEvolutionEngineCommunicator.Connect : boolean;
var
  tempString : String;
begin
  if (FConnected) then
    Disconnect;
  Result := false;
  FConnected := true;
  // ... Do connection stuffs
  tempString := FGameName + '_mutex';
  FMutex := OpenMutex( MUTEX_ALL_ACCESS, false, PChar(tempString) );
  if (FMutex = 0) then
  begin
    Disconnect;
    exit;
  end;
  tempString := FGameName+'_request';
  FRequestEvent := OpenEvent( EVENT_ALL_ACCESS, false, PChar(tempString) );
  if (FRequestEvent = 0) then
  begin
    Disconnect;
    exit;
  end;
  tempString := FGameName+'_result';
  FResultEvent := OpenEvent( EVENT_ALL_ACCESS, false, PChar(tempString) );
  if (FResultEvent = 0) then
  begin
    Disconnect;
    exit;
  end;
  tempString := FGameName+ '_mem';
  FMappedFile := OpenFileMapping(FILE_MAP_ALL_ACCESS, false, PChar(tempString) );
  if (FMappedFile = 0) then
  begin
    Disconnect;
    exit;
  end;
  FInterface := PInterfaceRecord(MapViewOfFile(FMappedFile,FILE_MAP_ALL_ACCESS,0,0,0));
  if (FInterface = nil) then
  begin
    Disconnect;
    exit;
  end;
  FAfterInterface := Pointer(Cardinal(FInterface) + sizeof(TInterfaceRecord));
  // Sanity Check on the header
  if (FInterface^.Magic <> 'c2e@') then
  begin
    Disconnect;
    exit;
  end;
  if (FTransaction <> '') then
    DoTransactionWithServer;
  Result := true;
end;

procedure TEvolutionEngineCommunicator.Disconnect;
begin
  if (FConnected = false) then
    exit;
  // ... Do disconnect Stuffs
  if (FInterface <> nil) then
  begin
    UnmapViewOfFile(FInterface);
    FInterface := nil;
  end;
  if (FMappedFile <> 0) then
  begin
    CloseHandle(FMappedFile);
    FMappedFile := 0;
  end;
  if (FResultEvent <> 0) then
  begin
    CloseHandle(FResultEvent);
    FResultEvent := 0;
  end;
  if (FRequestEvent <> 0) then
  begin
    CloseHandle(FRequestEvent);
    FRequestEvent := 0;
  end;
  if (FMutex <> 0) then
  begin
    CloseHandle(FMutex);
    FMutex := 0;
  end;
  FConnected := false;
end;

procedure Register;
begin
  RegisterComponents('C2e', [TEvolutionEngineCommunicator]);
end;

end.
