;========================;
;Console Support Section;
;========================;
;Section of UEFI FASM Library
;Updated April 10th 2020

;-------------------;
;Numerical Constants;
;-------------------;
;EFI Simple Text Output Attributes
	EFI_BLACK					=	0x00
	EFI_BLUE					=	0x01
	EFI_GREEN					=	0x02
	EFI_CYAN					=	0x03
	EFI_RED						=	0x04
	EFI_MAGENTA					=	0x05
	EFI_BROWN					=	0x06
	EFI_LIGHTGRAY				=	0x07
	EFI_BRIGHT					=	0x08
	EFI_DARKGRAY				=	EFI_BRIGHT or EFI_BLACK
	EFI_LIGHTBLUE				=	0x09
	EFI_LIGHTGREEN				=	0x0A
	EFI_LIGHTCYAN				=	0x0B
	EFI_LIGHTRED				=	0x0C
	EFI_LIGHTMAGENTA			=	0x0D
	EFI_YELLOW					=	0x0E
	EFI_WHITE					=	0x0F

	EFI_BACKGROUND_BLACK		=	0x00
	EFI_BACKGROUND_BLUE			=	0x10
	EFI_BACKGROUND_GREEN		=	0x20
	EFI_BACKGROUND_CYAN			=	0x30
	EFI_BACKGROUND_RED			=	0x40
	EFI_BACKGROUND_MAGENTA		=	0x50
	EFI_BACKGROUND_BROWN		=	0x60
	EFI_BACKGROUND_LIGHTGRAY	=	0x70

;------------------;
;Symbolic Constants;
;------------------;
	EFI_PHYSICAL_ADDRESS	equ		UINT64
	EFI_VIRTUAL_ADDRESS		equ		UINT64

;----------;
;Structures;
;----------;
struc _EFI_SIMPLE_TEXT_INPUT_PROTOCOL {
	.Reset				FUNCTION	;EFI_INPUT_RESET
	.ReadKeyStroke		FUNCTION	;EFI_INPUT_READ_KEY
	.WaitForKey			EFI_EVENT
}

struc _EFI_INPUT_KEY {
	.ScanCode			UINT16
	.UnicodeChar		CHAR16
}

struc _EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL {
	.Reset					FUNCTION	;EFI_INPUT_RESET_EX
	.ReadKeyStrokeEx		FUNCTION	;EFI_INPUT_READ_KEY_EX
	.WaitForKeyEx			EFI_EVENT
	.SetState				FUNCTION	;EFI_SET_STATE
	.RegisterKeyNotify		FUNCTION	;EFI_REGISTER_KEYSTROKE_NOTIFY
	.UnregisterKeyNotify	FUNCTION	;EFI_UNREGISTER_KEYSTROKE_NOTIFY
}

struc _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
	.Reset				FUNCTION	;EFI_TEXT_RESET
	.OutputString		FUNCTION	;EFI_TEXT_STRING
	.TestString			FUNCTION	;EFI_TEST_STRING
	.QueryMode			FUNCTION	;EFI_TEXT_QUERY_MODE
	.SetMode			FUNCTION	;EFI_TEXT_SET_MODE
	.SetAttribute		FUNCTION	;EFI_TEXT_SET_ATTRIBUTE
	.ClearScreen		FUNCTION	;EFI_TEXT_CLEAR_SCREEN
	.SetCursorPosition	FUNCTION	;EFI_TEXT_SET_CURSOR_POSITION
	.EnableCursor		FUNCTION	;EFI_TEXT_ENABLE_CURSOR
	.ModePtr			POINTER		;SIMPLE_TEXT_OUTPUT_MODE
}

struc _SIMPLE_TEXT_OUTPUT_MODE {
	.MaxMode		INT32
;Current Settings
	.Mode			INT32
	.Attribute		INT32
	.CursorColumn	INT32
	.CursorRow		INT32
	.CursorVisible	BOOLEAN
}

;----;
;Data;
;----;
;Read/Write Uninitialized Data Used With Macros
section '.bss' data readable writeable
ConInPtr:				POINTER		;EFI_SIMPLE_TEXT_INPUT_PROTOCOL
ConOutPtr:				POINTER		;EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
InputKey			_EFI_INPUT_KEY	;Used as an output data structure for getting key input

;------;
;Macros;
;------;
;Virtual Structures
VirtualStructure _EFI_SIMPLE_TEXT_INPUT_PROTOCOL, EFI_SIMPLE_TEXT_INPUT_PROTOCOL
VirtualStructure _EFI_INPUT_KEY, EFI_INPUT_KEY
	
macro SimpleTextInputInitialize {
	mov rbx,[SystemTablePtr]									
	mov	rbx,[rbx + EFI_SYSTEM_TABLE.ConInPtr]
	mov [ConInPtr],rbx
}

macro SimpleTextInputFunction function*, [data] { ;Set Function in Register
common
	mov rcx,[ConInPtr]
	mov rbx,[rcx + EFI_SIMPLE_TEXT_INPUT_PROTOCOL.#function]
	CallFunction rbx, rcx, data
}

;At exit of macro, rax is same status, bx is scan code, cx is unicodeCharReg, unless overriden
;Overriden expects 2 registers of at least 2-bytes or 1 register of at least 4-bytes...future work
macro SimpleTextInputReadKey scanCodeReg, unicodeCharReg {
	SimpleTextInputFunction ReadKeyStroke, InputKey
if scanCodeReg eq
	mov bx, [InputKey.ScanCode]
else
	mov scanCodeReg, [InputKey.ScanCode]
end if	
if unicodeCharReg eq
	mov cx, [InputKey.UnicodeChar]
else
	mov unicodeCharReg, [InputKey.UnicodeChar]
end if
}

macro SimpleTextInputWaitForKeyEvent indexPointer{
	mov rdx,[ConInPtr]
	;mov rdx,[rdx + EFI_SIMPLE_TEXT_INPUT_PROTOCOL.WaitForKey] ;Not Correct
	add rdx,EFI_SIMPLE_TEXT_INPUT_PROTOCOL.WaitForKey		
	BootServicesFunction WaitForEvent, 1, rdx, indexPointer
}

VirtualStructure _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL, EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL

macro SimpleTextOutputInitialize {
;Use effective addressing to set the ConOutPtr memory
	mov rbx,[SystemTablePtr]
	mov	rbx,[rbx + EFI_SYSTEM_TABLE.ConOutPtr]
	mov [ConOutPtr],rbx
}

macro SimpleTextOutputFunction function*, [data] { ;Set Function in Register
common
	mov rcx,[ConOutPtr]
	mov rbx,[rcx + EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.#function]
	CallFunction rbx, rcx, data
}

VirtualStructure _SIMPLE_TEXT_OUTPUT_MODE, SIMPLE_TEXT_OUTPUT_MODE

macro SimpleTextOutputMode name { ;Return the mode name in rax
	mov rbx,[ConOutPtr]
	mov	rbx,[rbx + EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.ModePtr]
	mov eax,[rbx + SIMPLE_TEXT_OUTPUT_MODE.#name]
}

