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
_spaceStr			CHAR16			' ',0
_xStr				CHAR16			'x',0
_exclamationStr		CHAR16			'!',0
_statusCodeStr		CHAR16string	'Status Code: '
_helloStr			CHAR16string	'Hello '
_helloWorldStr		CHAR16string	'Hello World!'
_stackPointerStr	CHAR16string	'Stack Pointer: '
_textOutputMaxModeStr	CHAR16string	'Max Mode: '
_textOutputModeStr	CHAR16string	'Mode: '
_modeSelectStr		CHAR16string	'Enter a mode number: '
_invalidInputStr	CHAR16string	'Invalid Input',SNL
_whatIsYourNameStr	CHAR16string	'Enter your name: '
_enterContinueStr	CHAR16string	'Press enter to continue...'
_keyPressedStr		CHAR16string	'Key Pressed: '

;---------------------------;
;Read/Write Initialized Data;
;---------------------------;
section '.data' data readable writeable
_hexStrBuf CHAR16 '0x0000000000000000',0	;Allocate max number of characters needed for UINT64
_decStrBuf CHAR16 '4294967295',0	;Allocate max number of characters needed for UINT32
_modeColumns		UINTN 0
_modeRows			UINTN 0
_inputBufferIndex	UINTN 0
_oneCharStr			CHAR16		'?',0
_waitEventIndex	 UINTN 0

;-----------------------------;
;Read/Write Uninitialized Data;
;-----------------------------;
section '.bss' data readable writeable
events:
;_inputKey		_EFI_INPUT_KEY
_inputBuffer	rw 100	;Reserve 100 characters for the input buffer
_timerEvent		EFI_EVENT


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

numToHexStr:	;Updates the hexadecimal string buffer in memory located at rbx with the text version of the value in register rax using rcx indicating number of bytes. Works on CHAR16 / UTF-16 Character Set. Expects rbx to have '0x' as starting characters
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
	ret

macro OutputHexNumber register*, buffer*, numBytes*, callVersion {
if ~ register eq rax
	push	rax
	mov		rax,register
end if
	push	rbx
	mov		rbx,buffer
	push	rcx
	mov		rcx,(numBytes*8-4)
	call	numToHexStr
	pop		rcx
	pop		rbx
if ~ register eq rax
	pop		rax
end if
	OutputString buffer, callVersion
}

macro OutputStatus {
	OutputString _statusCodeStr.newLine, 1
	OutputHexNumber rax,_hexStrBuf,8,1
}

numToDecStr:	;Updates the decimal string buffer in memory (_decStrBuf) with the text version of the value in register rax. Works with CHAR16 / UTF-16 character set
	push	rbx				;save rbx value
	mov		rbx,10			;put 10 into rbx
	push	rcx				;save rcx value
	xor		rcx,rcx			;clear rcx
	push	rdx				;save rdx value
	xor		rdx,rdx			;clear rdx
@@:
	div		ebx
	add		edx,0x30
	push	dx
	xor		edx,edx
	inc		rcx
	cmp		eax,0
	jnz		@b
	mov		rbx,r8
@@:
	pop		dx
	mov		word [rbx],dx
	add		rbx,2
	loop	@b
	mov		word [rbx],0
	pop		rdx
	pop		rcx
	pop		rbx
	ret

macro OutputDecNumber register*, buffer*, callVersion {
if ~ register eq rax
	push	rax
	mov		rax,register
end if
	push	r8
	mov		r8,buffer
	call	numToDecStr
	pop		r8
if ~ register eq rax
	pop		rax
end if
	OutputString buffer, callVersion
}

;String buffer is in rbx
decStrToNum:
	xor		rax,rax			;clear rax
	push	rcx				;save rcx value
	xor		rcx,rcx			;clear rcx
	mov		cx,word [rbx]
	cmp		cx,0x30		;Is At Least 0
	jb		invalidDecStr
	cmp		cx,0x39		;Is No More Than 9
	ja		invalidDecStr
	push	rdx				;save rdx value
	push	r8
	mov		r8,10
@@:
	sub		cx,0x30
	add		eax,ecx
	add		rbx,2
	mov		cx,word [rbx]
	cmp		cx,0x30		;Is At Least 0
	jb		@f
	cmp		cx,0x39		;Is No More Than 9
	ja		@f
	mul		r8d
	jmp		@b
@@:
	pop r8
	pop rdx
invalidDecStr:
	pop rcx
	ret

macro InputDecNumber buffer*, register*, checkInputValidity {
if ~ register eq rax
	push	rax
end if
	push	rbx
	mov		rbx,buffer
if ~ checkInputValidity eq
	
end if
	call	decStrToNum
	pop		rbx
if ~ register eq rax
	mov		register,rax
	pop		rax
end if
}

; Displays Entered Input On Console Too with printStr call
; Expects input buffer to be on r11
; Simple: does not accept moving cursor or delete key
waitForEnteredInput:
	xor		r10,r10		;Clear input buffer count
	mov		r12,_oneCharStr
readKeyStroke:
	SimpleTextInputReadKey
	cmp rax, EFI_SUCCESS
	jne waitForKeyStroke
	cmp bx, 0	;is scan key
	jne readKeyStroke
	cmp cx, 0xD	;Is Key Enter
	jne @f
	ret			;Enter occured so return...maybe insert print newLine
@@:				;Not Enter
	cmp cx, 0x8	;Is Key Backspace
	jne @f
	cmp r10,0
	je	readKeyStroke
	dec	r10
	sub r11,2
	jmp printCharacter
@@:				;Not Backspace
	inc	r10
	mov word [r11], cx		;Future work:better addressing
	add r11,2				;This is a test
printCharacter:
	mov word [r12], cx
	call printStr
	jmp readKeyStroke	;Collect all input until no input
waitForKeyStroke:
	SimpleTextInputWaitForKeyEvent _waitEventIndex
	jmp readKeyStroke


macro WaitForEnteredInput countReg*, buffer*, saveRegisters {
if ~ countReg eq r10
	push	r10
	mov		r10,countReg
end if
if ~ saveRegisters eq
	push r11
	push r12
end if
	mov		r11,buffer
	call	waitForEnteredInput
if ~ saveRegisters eq
	pop	r12
	pop r11
end if
if ~ countReg eq r10
	mov		countReg,r10
	pop		r10
end if
}

waitForEnter:
	SimpleTextInputReadKey
	cmp rax, EFI_SUCCESS
	jne waitForEnterStroke
	cmp bx, 0	;is scan key
	jne waitForEnter
	cmp cx, 0xD	;Is key Enter
	jne waitForEnter
	ret			;Enter occured so return
waitForEnterStroke:
	SimpleTextInputWaitForKeyEvent _waitEventIndex
	jmp waitForEnter

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
	SimpleTextOutputFunction EnableCursor, FALSE
	SimpleTextOutputFunction SetAttribute, EFI_WHITE or EFI_BACKGROUND_BLACK
	SimpleTextOutputFunction ClearScreen

	;Display the one and only Hello World String
	OutputString _helloWorldStr,1

	;Display the original stack pointer value
	mov	r12, _stackPointerStr.newLine
	OutputString r12,1
	OutputHexNumber r10,_hexStrBuf,8,1
	;Display the new baseline stack pointer value
	OutputString r12,1
	OutputHexNumber r11,_hexStrBuf,8,1

	;Disable watchdog timer...hopefully
	BootServicesFunction SetWatchdogTimer, 0, 0x10000, 0, 0
	OutputStatus

	;Display total number of modes (mode 1 / 80x50 may be unsupported)
	SimpleTextOutputMode MaxMode
	mov	r10,rax	;Move number of output modes to r10
	dec r10
	OutputString _textOutputMaxModeStr.newLine,1
	OutputDecNumber r10,_decStrBuf,1

	;Display Various Mode Options
	xor r11,r11		;clear r11 - used for mode looping
@@:
	SimpleTextOutputFunction QueryMode, r11, _modeColumns, _modeRows
	inc r11
	cmp rax, EFI_SUCCESS
	jne @b	; jump back if information was not returned
	dec r11
	OutputString _textOutputModeStr.newLine,1
	OutputDecNumber r11,_decStrBuf,1
	OutputString _spaceStr,1
	OutputDecNumber [_modeColumns],_decStrBuf,1
	OutputString _xStr,1
	OutputDecNumber [_modeRows],_decStrBuf,1
	inc r11
	cmp r11, r10
	jb @b

	;Loop for entered number
	SimpleTextInputFunction Reset, TRUE
	SimpleTextOutputFunction EnableCursor, TRUE
	
	mov r15, r10
selectMode:
	OutputString _modeSelectStr.newLine,1
	WaitForEnteredInput r10, _inputBuffer
	mov		word [r10*2 + _inputBuffer],0
	mov		r11w, word [_inputBuffer]
	cmp		r11w,0x30		;Is At Least 0
	jb		invalidMode
	cmp		r11w,0x39		;Is No More Than 9
	ja		invalidMode
	InputDecNumber _inputBuffer, r11
	cmp		r15, r11		;Check under max
	jb		invalidMode
	SimpleTextOutputFunction QueryMode, r11, _modeColumns, _modeRows
	cmp		rax, EFI_SUCCESS
	je		modeSelected
invalidMode:
	OutputString _invalidInputStr.newLine,1
	;OutputString _newLineStr,1
	jmp selectMode
modeSelected:
	SimpleTextOutputFunction SetMode, r11

	OutputString _whatIsYourNameStr,1
	WaitForEnteredInput r10, _inputBuffer
	mov	word [r10*2 + _inputBuffer],0
	SimpleTextOutputFunction EnableCursor, FALSE
	SimpleTextOutputFunction SetAttribute, EFI_LIGHTBLUE or EFI_BACKGROUND_LIGHTGRAY
	SimpleTextOutputFunction ClearScreen

calculateMiddlePosition:
	xor rdx,rdx
	mov rax, [_modeRows]
	mov	rbx, 2
	div ebx
	mov r11, rax
	xor rdx,rdx
	mov rax, [_modeColumns]
	sub rax,7
	sub rax,r10
	div ebx
	mov r12,rax
	;OutputDecNumber r12,_decStrBuf,1
	;OutputString _xStr,1
	;OutputDecNumber r11,_decStrBuf,1
	SimpleTextOutputFunction SetCursorPosition, r12, r11
	;OutputDecNumber r10,_decStrBuf,1
	OutputString _helloStr,1
	OutputString _inputBuffer,1
	OutputString _exclamationStr,1

	BootServicesFunction CreateEvent, EVT_TIMER, 0,0,0, _timerEvent

	;Set a timer event for 2 seconds from now
	BootServicesFunction SetTimer, [_timerEvent], TimerRelative, 20000000

	BootServicesFunction WaitForEvent, 1, _timerEvent, _waitEventIndex

	;BootServicesFunction CheckEvent, _timerEvent
	;OutputStatus
	
	OutputString _newLineStr,1
	OutputString _enterContinueStr.newLine,1		;Display the press enter to continue string
	call	waitForEnter

	OutputString _newLineStr,1
	ExitSuccessfullyUEFI

