;x64 UEFI Feature Test
;An Enhanced Hello World Using the Improving UEFI FASM assembly library

;Will use a custom MZ-DOS PE32+ Program Stub to be different from the standard and save a few bytes in the final output file

;-------------------;
;FASM Program Header;
;-------------------;
format PE64 EFIboot; at 0x400000 on '../UEFI-Program-MZ-DOS-Stub.exe'
entry start		;Label name of the entry point

;-----------------;
;Inclued Libraries;
;-----------------;
include '../UEFI.inc'	;Include UEFI FASM assembly library

;----------------;
;Symbol Constants;
;----------------;
SNL equ 10,13	;String New Line Characters

;-----------------;
;Helper Structures;
;-----------------;
struc CHAR16string [characters] {	;String Helper Struct
common	;Common label name for all characters / One-line array
	. du characters
	.termination du 0
	.size = $ - .	;Needs db/dw or something...?
}

;---------;
;Main Code;
;---------;
section '.text' code executable readable	;read-only executable code

start:	;Entry Point Label
	InitializeUEFI					;No Error Handling Yet
	InitializeConOut
	ConOutOutputString _helloWorldStr

initLoop:
	mov	rax, 0x1000000000 ;Move a value
limitLoop:
	dec rax
	jnz limitLoop

	ExitSuccessfullyUEFI
	
;--------------------------;
;Read-only Initialized Data;
;--------------------------;
section '.rdata' data readable

_helloWorldStr	CHAR16string 'Hello World',SNL,'Extra Line'

;---------------------------;
;Read/Write Initialized Data;
;---------------------------;
section '.data' data readable writeable
;_TestStorageVariable	UINTN

;-----------------------------;
;Read/Write Uninitialized Data;
;-----------------------------;
section '.bss' data readable writeable
_TestStorageVariable2	UINTN


;End of File

