;x64 UEFI Feature Test
;An Enhanced Hello World Using the Improving UEFI FASM assembly library

;Will use a custom MZ-DOS PE32+ Program Stub to be different from the standard and save a few bytes in the final output file

;-------------------;
;FASM Program Header;
;-------------------;
format PE64 EFI at 0x400000 on '../UEFI-Program-MZ-DOS-Stub.exe'
entry start		;Label name of the entry point

;-----------------;
;Inclued Libraries;
;-----------------;
include '../UEFI.inc'	;Include UEFI FASM assembly library

;-------------------;
;Numerical Constants;
;-------------------;
NULL			= 0
FALSE			= 0
TRUE			= 1
BITS_IN_HEX		= 4

;----------------;
;Symbol Constants;
;----------------;
SNL equ 10,13	;String New Line Characters

;-----------------;
;Helper Structures;
;-----------------;
struc CHAR16string [characters] {	;String Helper Struct
common								;Common label name for all characters / One-line array
	.newLine CHAR16	SNL
	. CHAR16 characters
	.termination du 0
	;.size = $ - .					;Needs db/dw or something...? Not Used Right Now
}

;--------------------------;
;Read-only Initialized Data;
;--------------------------;
section '.rdata' data readable
_newLineStr			CHAR16			SNL,0
_statusCodeStr		CHAR16string	'Status Code: '
_helloWorldStr		CHAR16string	'Hello World!'
_stackPointerStr	CHAR16string	'Stack Pointer: '
_textOutputMaxMode	CHAR16string	'Max Mode: '
_enterContinueStr	CHAR16string	'Press enter to continue...'
_keyPressedStr		CHAR16string	'Key Pressed: '

;---------------------------;
;Read/Write Initialized Data;
;---------------------------;
section '.data' data readable writeable
_hexStrBuf CHAR16 '0x0000000000000000',0	;Allocate max number of characters needed for UINT64

_waitEventIndex UINTN 0

;-----------------------------;
;Read/Write Uninitialized Data;
;-----------------------------;
section '.bss' data readable writeable
events:
_timerEvent		EFI_EVENT
_keyEvent		EFI_EVENT

;---------------;
;Relocation Data;
;---------------;
section '.reloc' fixups data discardable	;A must? for UEFI...

;----;
;Code;
;----;
;====================;
;Macros and Functions;
;====================;
section '.text' code executable readable	;read-only executable code
;Both ,acros and functions attempt to  perserve register values but do not currently peserve status register states

printStr:		;Prints String Using Pointer value in r12
	SimpleTextOutputFunction OutputString, r12
	ret

macro OutputString	string, callVersion {
if ~ callVersion eq
	if ~ string eq r12
		push	r12
		mov		r12,string
	end if
	call	printStr
	if ~ string eq r12
		pop		r12
	end if
else
	SimpleTextOutputFunction OutputString, string
end if
}

numToHexStr:	;Updates the hexadecimal string buffer in memory (_hexStrBuf) with the text version of the value in register rax. Works on CHAR16 / UTF-16 Character Set
	push	rbx
	mov		rbx,_hexStrBuf
	add		rbx,4
	push	rdx
numToHexStrLoop:
	mov		rdx,rax
	shr		rdx,cl
	and		rdx,0xF
	cmp		rdx,0xA
	jl		@f
	add		rdx,0x7
@@:
	add		rdx,0x30
	mov		word [rbx],dx
	sub		rcx,3
	add		rbx,2
	loop	numToHexStrLoop
	mov		rdx,rax
	and		rdx,0xF
	cmp		rdx,0xA
	jl		@f
	add		rdx,0x7
@@:
	add		rdx,0x30
	mov		word [rbx],dx
	add		rbx,2
	mov		word [rbx],0
	pop		rdx
	pop		rbx
	ret

macro OutputNumber register*, numBytes*, callVersion {
if ~ register eq rax
	push	rax
	mov		rax,register
end if
	push	rcx
	mov		rcx,(numBytes*8-4)
	call	numToHexStr
	pop		rcx
if ~ register eq rax
	pop		rax
end if
	OutputString _hexStrBuf, callVersion
}

macro OutputStatus {
	OutputString _statusCodeStr.newLine, 1
	OutputNumber rax,8,1
}

;=========;
;Main Code;
;=========;
start:						;Entry Point Label
	mov r10,rsp				;Move First Stack Pointer Value into r10
	InitializeUEFI			;No Error Handling Yet
	mov r11,rsp				;Move Second Stack Pointer Value into r11, to compare the offset alignment of the sp in r10
	
	;Initialize other elements of the UEFI FASM library
	BootServicesInitialize
	RuntimeServicesInitialize
	SimpleTextInputInitialize
	SimpleTextOutputInitialize
	
	;Reset Text Output
	SimpleTextOutputFunction Reset, TRUE
	SimpleTextOutputFunction SetAttribute, EFI_WHITE or EFI_BACKGROUND_BLACK
	SimpleTextOutputFunction ClearScreen
	SimpleTextOutputFunction EnableCursor, FALSE
	
	;Display the one and only Hello World String
	OutputString _helloWorldStr,1

	;Display the original stack pointer value
	mov		r12, _stackPointerStr.newLine
	OutputString r12,1
	OutputNumber r10,8,1
	;Display the new baseline stack pointer value
	OutputString r12,1
	OutputNumber r11,8,1

	OutputString _textOutputMaxMode.newLine,1
	SimpleTextOutputGetMaxMode
	OutputNumber rax,2,1
	
	BootServicesFunction SetWatchdogTimer, 0, 0x10000, 0, 0	;Should disable watchdog timer...

	BootServicesFunction CreateEvent, EVT_TIMER, 0,0,0, _timerEvent

	;Set a timer event for 2 seconds from now
	BootServicesFunction SetTimer, [_timerEvent], TimerRelative, 20000000

	BootServicesFunction WaitForEvent, 1, _timerEvent, _waitEventIndex

	BootServicesFunction CheckEvent, _timerEvent
	OutputStatus

	;Temporary Time Loop	;Not Needed anymore since timer events working
;initLoop:
	;mov	r10, 0x10000000 ;Move a value
;jmpTimeLoop:
	;dec r10
	;jnz jmpTimeLoop
	
	OutputString _enterContinueStr.newLine,1		;Display the press enter to continue string
	SimpleTextInputFunction Reset, TRUE

	SimpleTextInputWaitForKeyEvent _waitEventIndex

@@:	SimpleTextInputFunction ReadKeyStroke, inputKey
	;cmp rax, EFI_SUCCESS	;Not needed since wait for key event working
	;jne @b
	OutputString _keyPressedStr.newLine,1
	mov ax, [inputKey.ScanCode]
	OutputNumber rax,2,1
	;SimpleTextInputWaitForKeyEvent _waitEventIndex

	OutputString _newLineStr,1


	ExitSuccessfullyUEFI


