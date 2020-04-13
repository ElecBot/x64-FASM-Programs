;==========================;
;x64 UEFI Print System Info;
;==========================;
;Prints the System and UEFI Info In an Organized Manor On the Screen
;An assembly program using the ever improving UEFI FASM Library

;-------------------;
;FASM Program Header;
;-------------------;
;The following tells FASM to create an EFI executable application targeting an x64 Platform
;	The ImageBase address value in the header is "at" 0x00400000 Windows NT application default
;		https://docs.microsoft.com/en-us/windows/win32/debug/pe-format\
;		optional-header-windows-specific-fields-image-only
;FASM will use the custom MZ-DOS PE32+ Program Stub defined after "on"
;	This saves a few bytes in the final output executable file
format PE64 EFI at 0x400000 on '../UEFI-Program-MZ-DOS-Stub.exe'
entry start		;Label name of the program execution entry point

;-----------------;
;Inclued Libraries;
;-----------------;
include '../UEFI.inc'	;Include UEFI FASM Library

;-------------------;
;Numerical Constants;
;-------------------;
BITS_IN_HEX		= 4
MEMORY_MAP_BYTES	=	4096*10

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
_indexStr			CHAR16			': ',0
_statusCodeStr		CHAR16string	'Status Code: '
_enterContinueStr	CHAR16string	'Press enter to continue...'
_uefiInfoStr		CHAR16string	'UEFI Info:'
_efiRevisionStr		CHAR16string	'EFI Revision: '
_firmwareVendorStr	CHAR16string	'Firmware Vendor: '
_firmwareRevStr		CHAR16string	'Firmware Revision: '
_numTableEntriesStr	CHAR16string	'System Configuration Count: '
_bufferIndexStr		CHAR16string	'Buffer Index '
_descriptorSizeStr	CHAR16string	'Descriptor Byte Size: '
_descriptorVerStr	CHAR16string	'Descriptor Version: '
_memoryMapHeadStr	CHAR16string	'Memory Map Entry Type Pages ',\
									'Physical Start Virtual Start Attributes'
_memoryMapStr		CHAR16string	'Memory Map'
_invalidInputStr	CHAR16string	'Invalid Input',SNL

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
_waitEventIndex		UINTN 0

align EIGHT_BYTE_BOUNDARY	;For good measure
_memoryMapSize				UINTN	MEMORY_MAP_BYTES
_memoryMapKey				UINTN	0
_memoryDescriptorSize		UINTN	0
_memoryDescriptorVersion	UINT32	32

;-----------------------------;
;Read/Write Uninitialized Data;
;-----------------------------;
section '.bss' data readable writeable
align EIGHT_BYTE_BOUNDARY	;For good measure
_memoryMap		rb MEMORY_MAP_BYTES

_inputBuffer	rw 100	;Reserve 100 characters for the input buffer
_timerEvent		EFI_EVENT

;---------------;
;Relocation Data;
;---------------;
section '.reloc' fixups data discardable	;A must? for UEFI applications...

;====;
;Code;
;====;
;The rest of the file defines executable read-only code
section '.text' code executable readable

;--------------------;
;Macros and Functions;
;--------------------;
;Macros and Functions attempt to properly perserve register values
;	There is no garentee that status register states will be preserved

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

;Updates the string buffer in memory located at rbx with the text version of the value
;	Value is in register rax and rcx is used to indicate number of bytes.
;	Designed for on CHAR16 / UTF-16 Character Set which is character format of UEFI
;	Expects rbx hexadecimal string buffer to have '0x' as starting characters
numToHexStr:
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
	push rax
	OutputString _statusCodeStr.newLine, 1
	pop rax
	OutputHexNumber rax,_hexStrBuf,8,1
}

;Updates the decimal string buffer in memory (rbx) with the text version of a value
;	The value is in register rax. Designed for CHAR16 / UTF-16 character set
numToDecStr:
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

macro pressEnterToContinue {
	OutputString _newLineStr,1
	OutputString _enterContinueStr.newLine,1	;Display the press enter to continue string
	call	waitForEnter
}

;---------;
;Main Code;
;---------;
start:						;Entry Point Label
	InitializeUEFI			;No Error Handling Yet

	;Initialize other elements of the UEFI FASM library
	BootServicesInitialize
	RuntimeServicesInitialize
	SimpleTextInputInitialize
	SimpleTextOutputInitialize

	;Disables the watchdog timer
	BootServicesFunction SetWatchdogTimer, 0, 0x10000, 0, 0
	OutputStatus
	pressEnterToContinue

	;Reset Text Output
	SimpleTextOutputFunction Reset, TRUE
	SimpleTextOutputFunction EnableCursor, FALSE
	SimpleTextOutputFunction SetAttribute, EFI_WHITE or EFI_BACKGROUND_BLACK
	SimpleTextOutputFunction ClearScreen

	;Set Text Output Mode To Max Rows
	SimpleTextOutputMode MaxMode	;Get number of output modes into rax
	mov	r10,rax				;Move number of output modes to r10
	ClearRegister r11		;clear r11 - used for mode looping
	ClearRegister r12		;clear r12 - used for holding max rows found so far
	ClearRegister r13		;clear r13 - used for holding best index so far
	;sub r11,1
loopOutputModes:
	SimpleTextOutputFunction QueryMode, r11, _modeColumns, _modeRows
	inc r11
	cmp rax, EFI_SUCCESS	;Check if Valid Mode
	jne loopOutputModes		;jump back if information was not returned
	cmp r12,[_modeRows]		;Compare max rows
	jae	@f
	mov r12,[_modeRows]
	mov r13,r11
@@:
	cmp r10,r11
	ja loopOutputModes

	;Switch Mode After Double-Check
	dec r13
	SimpleTextOutputFunction QueryMode, r13, _modeColumns, _modeRows
	cmp rax, EFI_SUCCESS	;DoubleCheck if Valid Mode
	jne enterAndExit		;Exit Program If Failed
	SimpleTextOutputFunction SetMode, r13	;Change to selected console out text mode

	;Display Basic UEFI Info
	mov r10,[SystemTablePtr]
	OutputString _uefiInfoStr,1
	OutputString _efiRevisionStr.newLine,1
	mov r12,EFI_TABLE_HEADER.Revision
	mov	eax,[r10 + r12 + EFI_SYSTEM_TABLE.Hdr]
	OutputHexNumber rax,_hexStrBuf,4,1
	OutputString _firmwareVendorStr.newLine,1
	mov	r12,[r10 + EFI_SYSTEM_TABLE.FirmwareVendorPtr]
	OutputString r12, 1
	OutputString _firmwareRevStr.newLine,1
	mov	eax,[r10 + EFI_SYSTEM_TABLE.FirmwareRevision]
	OutputHexNumber rax,_hexStrBuf,4,1

	;Display Configuration Table Entries Top GUID 64 Bytes
	OutputString _numTableEntriesStr.newLine,1
	mov rax,[r10 + EFI_SYSTEM_TABLE.NumberOfTableEntries]
	mov r11,rax
	OutputDecNumber r11,_decStrBuf,1
	mov r12,[r10 + EFI_SYSTEM_TABLE.ConfigurationTablePtr]
	ClearRegister r13
	ClearRegister r14
@@:
	mov r15,[r12 + r13]
	OutputString _bufferIndexStr.newLine,1
	OutputDecNumber r14,_decStrBuf,1
	OutputString _indexStr,1
	OutputHexNumber r15,_hexStrBuf,8,1
	add r13,24
	inc r14
	cmp r11,r14
	ja	@b

	;Press Enter to continue
	pressEnterToContinue

	;Get Memory Map and Print Basic Info
	SimpleTextOutputFunction ClearScreen
	BootServicesFunction	GetMemoryMap,_memoryMapSize,_memoryMap,\
							_memoryMapKey,_memoryDescriptorSize,_memoryDescriptorVersion
	OutputStatus
	cmp rax, EFI_SUCCESS
	je @f
	OutputDecNumber [_memoryMapSize],_decStrBuf,1
	jmp enterAndExit
@@:
	OutputString _descriptorSizeStr.newLine,1
	OutputDecNumber [_memoryDescriptorSize],_decStrBuf,1
	OutputString _descriptorVerStr.newLine,1
	mov eax,[_memoryDescriptorVersion]
	OutputDecNumber rax,_decStrBuf,1

	;Calculate number of memory entries -> store into r13
	mov rax,[_memoryMapSize]
	ClearRegister rdx
	div [_memoryDescriptorSize]
	mov r13,rax

	OutputString _memoryMapHeadStr.newLine,1
	mov r10,_memoryMap
	mov r11,[_memoryDescriptorSize]
	ClearRegister r12
	mov r14,[_modeRows]
	sub r14,4
memoryMapLoop:
	OutputString _memoryMapStr.newLine,1
	OutputString _spaceStr,1
	OutputDecNumber r12,_decStrBuf,1
	OutputString _spaceStr,1
	mov eax,[r10 + EFI_MEMORY_DESCRIPTOR.Type]
	OutputDecNumber rax,_decStrBuf,1
	OutputString _spaceStr,1
	OutputDecNumber [r10 + EFI_MEMORY_DESCRIPTOR.NumberOfPages],_decStrBuf,1
	OutputString _spaceStr,1
	OutputHexNumber [r10 + EFI_MEMORY_DESCRIPTOR.PhysicalStart],_hexStrBuf,8,1
	OutputString _spaceStr,1
	OutputHexNumber [r10 + EFI_MEMORY_DESCRIPTOR.VirtualStart],_hexStrBuf,8,1
	OutputString _spaceStr,1
	OutputHexNumber [r10 + EFI_MEMORY_DESCRIPTOR.Attribute],_hexStrBuf,8,1
	
	SimpleTextOutputMode CursorRow
	cmp rax,r14
	jbe @f
	pressEnterToContinue
	SimpleTextOutputFunction ClearScreen
	OutputString _memoryMapHeadStr.newLine,1
@@:
	add r10,r11
	inc r12
	cmp r13,r12
	ja memoryMapLoop

enterAndExit:
	;Press Enter to continue
	pressEnterToContinue
	OutputString _newLineStr,1

	ExitSuccessfullyUEFI
































