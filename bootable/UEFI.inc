;UEFI FASM Library based on the 2.8 version specification: https://uefi.org/specifications
;Version 1
;Updated April 3nd 2020

;------------------------;
;Pure Numerical Constants;
;------------------------;
;PE32+ Image Header Relevant UEFI Values
	;Subsystem Type for EFI Images
	EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION			=	10
	EFI_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER	=	11
	EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER		=	12

	;Machine Type for EFI Images
	EFI_IMAGE_MACHINE_IA32	=	0x014c ;Value Not Intended for Use Currently (Shown for Reference)
	EFI_IMAGE_MACHINE_x64	=	0x8664

;EFI System Table
	EFI_SYSTEM_TABLE_SIGNATURE		=	0x5453595320494249
	EFI_2_80_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 80	;NEEDs Checking
	;ADD More Revisions Here
	;EFI_SYSTEM_TABLE_REVISION		=	EFI_2_8_SYSTEM_TABLE_REVISION	;Not sure about this yet
	;EFI_SPECIFICATION_VERSION		=	EFI_SYSTEM_TABLE_REVISION		;Works in FASM???

;EFI Boot Services
	EFI_BOOT_SERVICES_SIGNATURE		=	0x56524553544f4f42
	;EFI_BOOT_SERVICES_REVISION		=	EFI_SPECIFICATION_VERSION

;EFI Runtime Services
	EFI_RUNTIME_SERVICES_SIGNATURE	=	0x56524553544e5552
	;EFI_RUNTIME_SERVICES_REVISION	=	EFI_SPECIFICATION_VERSION

;EFI Status Codes
	EFI_SUCCESS				= 0

	EFIERR					= 0x8000000000000000	;Valid for 64-bit (only...?)
	EFI_LOAD_ERROR			= EFIERR or 1
	EFI_INVALID_PARAMETER	= EFIERR or 2
	EFI_UNSUPPORTED 		= EFIERR or 3
	EFI_BAD_BUFFER_SIZE		= EFIERR or 4
	EFI_BUFFER_TOO_SMALL	= EFIERR or 5
	EFI_NOT_READY			= EFIERR or 6
	EFI_DEVICE_ERROR		= EFIERR or 7
	EFI_WRITE_PROTECTED		= EFIERR or 8
	EFI_OUT_OF_RESOURCES	= EFIERR or 9
	EFI_VOLUME_CORRUPTED	= EFIERR or 10
	EFI_VOLUME_FULL 		= EFIERR or 11
	EFI_NO_MEDIA			= EFIERR or 12
	EFI_MEDIA_CHANGED		= EFIERR or 13
	EFI_NOT_FOUND			= EFIERR or 14
	EFI_ACCESS_DENIED		= EFIERR or 15
	EFI_NO_RESPONSE 		= EFIERR or 16
	EFI_NO_MAPPING			= EFIERR or 17
	EFI_TIMEOUT				= EFIERR or 18
	EFI_NOT_STARTED 		= EFIERR or 19
	EFI_ALREADY_STARTED		= EFIERR or 20
	EFI_ABORTED				= EFIERR or 21
	EFI_ICMP_ERROR			= EFIERR or 22
	EFI_TFTP_ERROR			= EFIERR or 23
	EFI_PROTOCOL_ERROR		= EFIERR or 24
	EFI_INCOMPATIBLE_VERSION= EFIERR or 25
	EFI_SECURITY_VIOLATION	= EFIERR or 26
	EFI_CRC_ERROR			= EFIERR or 27
	EFI_END_OF_MEDIA		= EFIERR or 28
	EFI_END_OF_FILE			= EFIERR or 31
	EFI_INVALID_LANGUAGE	= EFIERR or 32
	EFI_COMPROMISED_DATA	= EFIERR or 33
	EFI_IP_ADDRESS_CONFLICT	= EFIERR or 34
	EFI_HTTP_ERROR			= EFIERR or 35
	
	EFI_WARN_UNKNOWN_GLYPH		= 1
	EFI_WARN_DELETE_FAILURE		= 2
	EFI_WARN_WRITE_FAILURE		= 3
	EFI_WARN_BUFFER_TOO_SMALL	= 4
	EFI_WARN_STALE_DATA			= 5
	EFI_WARN_FILE_SYSTEM		= 6
	EFI_WARN_RESET_REQUIRED		= 7

;General Boundary Alignments
	TWO_BYTE_BOUNDARY		=	2
	FOUR_BYTE_BOUNDARY		=	4
	EIGHT_BYTE_BOUNDARY		= 	8
	SIXTEEN_BYTE_BOUNDARY	= 	16	;For Stack

;Native Byte Count
	NATIVE_BYTE_COUNT	=	8

;------------------;
;Symbolic Constants;
;------------------;
;UEFI Data Type Space Reservations
	BOOLEAN		equ		rb 1	;0=FALSE, 1=TRUE, Other=Undefined
	INT8		equ		rb 1
	UINT8		equ		rb 1
	INT16		equ		rw 1
	UINT16		equ		rw 1
	INT32		equ		rd 1
	UINT32		equ		rd 1
	INT64		equ		rq 1
	UINT64		equ		rq 1
	INT128		equ		rq 2
	UINT128		equ		rq 2
	CHAR8		equ		rb 1
	CHAR16		equ		rw 1
	INTN		equ		INT64	;Native Width Value Valid Only for 64-bit
	UINTN		equ		UINT64	;Native Width Value Valid Only for 64-bit
	;VOID						;Undeclared Type
	POINTER		equ		UINTN	;Pointer, valid when UINTN is valid
	FUNCTION	equ		POINTER	;Function,
	EFI_GUID	equ		rq 2	;Unique Identifier Value, needs alignment on 64-bit/8-byte boundary
	EFI_STATUS	equ		UINTN	;Status Code
	EFI_HANDLE	equ		POINTER	;Collection of related interfaces
	EFI_EVENT	equ		POINTER	;Handle to an event structure
	EFI_LBA		equ		UINT64	;Logical Block Address
	EFI_TPL		equ		UINTN	;Task Priority Level

	EFI_MAC_ADDRESS		equ		rq 4	;Media Access Controll Address, 32-byte buffer
	EFI_IPv4_ADDRESS	equ		rd 1	;IPv4 internet protocol address, 4-byte buffer
	EFI_IPv6_ADDRESS	equ		rq 2	;IPv6 internet protocol address, 16-byte buffer
	EFI_IP_ADDRESS		equ		rq 2	;Either internet protocol address, 16-byte buffer aligned on 4-byte boundary
	;Little Endian Ordering...
	;Possibly More to be added later

;----------;
;Structures;
;----------;
;Might Need to Add Alignment Values In Future

struc EFI_TABLE_HEADER {
	.Signature    	UINT64
	.Revision     	UINT32
	.HeaderSize   	UINT32
	.CRC32        	UINT32
	.Reserved     	UINT32
}

struc EFI_SYSTEM_TABLE {
	.Hdr					EFI_TABLE_HEADER
	.FirmwareVendorPtr		POINTER			;CHAR16
	.FirmwareRevision		UINT32
align	EIGHT_BYTE_BOUNDARY		;Necessary For Some Undetermined Reason
	.ConsoleInHandle		EFI_HANDLE
	.ConInPtr				POINTER			;EFI_SIMPLE_TEXT_INPUT_PROTOCOL
	.ConsoleOutHandle		EFI_HANDLE
	.ConOutPtr				POINTER			;EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
	.StandardErrorHandle	EFI_HANDLE
	.StdErrPtr				POINTER			;EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
	.RuntimeServicesPtr		POINTER			;EFI_RUNTIME_SERVICES
	.BootServicesPtr		POINTER			;EFI_BOOT_SERVICES
	.NumberOfTableEntries	UINTN
	.ConfigurationTablePtr	POINTER			;EFI_CONFIGURATION_TABLE
}

struc EFI_BOOT_SERVICES {
	.Hdr				EFI_TABLE_HEADER
;Task Priority Services
	.RaiseTPL			FUNCTION		;EFI_RAISE_TPL
	.RestoreTPL			FUNCTION		;EFI_RESTORE_TPL
;Memory Services
	.AllocatePages		FUNCTION		;EFI_ALLOCATE_PAGES
	.FreePages			FUNCTION		;EFI_FREE_PAGES
	.GetMemoryMap		FUNCTION		;EFI_GET_MEMORY_MAP
	.AllocatePool		FUNCTION		;EFI_ALLOCATE_POOL
	.FreePool			FUNCTION		;EFI_FREE_POOL
;Event & Timer Services
	.CreateEvent		FUNCTION		;EFI_CREATE_EVENT
	.SetTimer			FUNCTION		;EFI_SET_TIMER
	.WaitForEvent		FUNCTION		;EFI_WAIT_FOR_EVENT
	.SignalEvent		FUNCTION		;EFI_SIGNAL_EVENT
	.CloseEvent			FUNCTION		;EFI_CLOSE_EVENT
	.CheckEvent			FUNCTION		;EFI_CHECK_EVENT
;Protocol Handler Services
	.InstallProtocolInterface		FUNCTION	;EFI_INSTALL_PROTOCOL_INTERFACE
	.ReinstallProtocolInterface		FUNCTION	;EFI_REINSTALL_PROTOCOL_INTERFACE
	.UninstallProtocolInterface		FUNCTION	;EFI_UNINSTALL_PROTOCOL_INTERFACE
	.HandleProtocol					FUNCTION	;EFI_HANDLE_PROTOCOL
	.VoidPtr						POINTER		;Reserved
	.RegisterProtocolNotify			FUNCTION	;EFI_REGISTER_PROTOCOL_NOTIFY
	.LocateHandle					FUNCTION	;EFI_LOCATE_HANDLE
	.LocateDevicePath				FUNCTION	;EFI_LOCATE_DEVICE_PATH
	.InstallConfigurationTable		FUNCTION	;EFI_INSTALL_CONFIGURATION_TABLE
;Image Services
	.LoadImage			FUNCTION		;EFI_IMAGE_LOAD
	.StartImage			FUNCTION		;EFI_IMAGE_START
	.Exit				FUNCTION		;EFI_EXIT
	.UnloadImage		FUNCTION		;EFI_IMAGE_UNLOAD
	.ExitBootServices	FUNCTION		;EFI_EXIT_BOOT_SERVICES
;Miscellaneous Services
	.GetNextMonotonicCount		FUNCTION		;EFI_GET_NEXT_MONOTONIC_COUNT
	.Stall						FUNCTION		;EFI_STALL
	.SetWatchdogTimer			FUNCTION		;EFI_SET_WATCHDOG_TIMER
;Driver Support Services
	.ConnectController			FUNCTION		;EFI_CONNECT_CONTROLLER
	.DisconnectController		FUNCTION		;EFI_DISCONNECT_CONTROLLER
;Open and Close Protocol Services
	.OpenProtocol				FUNCTION		;EFI_OPEN_PROTOCOL
	.CloseProtocol				FUNCTION		;EFI_CLOSE_PROTOCOL
	.OpenProtocolInformation	FUNCTION		;EFI_OPEN_PROTOCOL_INFORMATION
;Library Services
	.ProtocolsPerHandle						FUNCTION	;EFI_PROTOCOLS_PER_HANDLE
	.LocateHandleBuffer						FUNCTION	;EFI_LOCATE_HANDLE_BUFFER
	.LocateProtocol							FUNCTION	;EFI_LOCATE_PROTOCOL
	.InstallMultipleProtocolInterfaces		FUNCTION	;EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES
	.UninstallMultipleProtocolInterfaces	FUNCTION	;EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES
;32-bit CRC Services
	.CalculateCrc32		FUNCTION		;EFI_CALCULATE_CRC32
;Miscellaneous Services (Cont.)
	.CopyMem			FUNCTION		;EFI_COPY_MEM
	.SetMem				FUNCTION		;EFI_SET_MEM
	.CreateEventEx		FUNCTION		;EFI_CREATE_EVENT_EX
}


struc EFI_RUNTIME_SERVICES {
	.Hdr					EFI_TABLE_HEADER
;Time Services
	.GetTime				FUNCTION		;EFI_GET_TIME
	.SetTime				FUNCTION		;EFI_SET_TIME
	.GetWakeupTime			FUNCTION		;EFI_GET_WAKEUP_TIME
	.SetWakeupTime			FUNCTION		;EFI_SET_WAKEUP_TIME
;Virtual Memory Services
	.SetVirtualAddressMap	FUNCTION		;EFI_SET_VIRTUAL_ADDRESS_MAP
	.ConverterPointer		POINTER			;EFI_CONVERT_POINTER (Function or Pointer?)
;Variable Services
	.GetVariable			FUNCTION		;EFI_GET_VARIABLE
	.GetNextVariableName	FUNCTION		;EFI_GET_NEXT_VARIABLE_NAME
	.SetVariable			FUNCTION		;EFI_SET_VARIABLE
;Miscellaneous Services
	.GetNextHighMonotonicCount		FUNCTION	;EFI_GET_NEXT_HIGH_MONO_COUNT
	.ResetSystem					FUNCTION	;EFI_RESET_SYSTEM
;UEFI 2.0 Capsule Services
	.UpdateCapsule					FUNCTION	;EFI_UPDATE_CAPSULE
	.QueryCapsuleCapabilities		FUNCTION	;EFI_QUERY_CAPSULE_CAPABILITIES
;Miscellaneous UEFI 2.0 Service
	.QueryVariableInfo		FUNCTION		;EFI_QUERY_VARIABLE_INFO
}

;Expand the following Information More
struc EFI_CONFIGURATION_TABLE {
	.VendorGuid			EFI_GUID
	.VendorTablePtr		POINTER		;Void
}

struc EFI_PROPERTIES_TABLE {
	.Version						UINT32
	.Length							UINT32
	.MemoryProtectionAttribute		UINT64
}

struc EFI_MEMORY_ATTRIBUTES_TABLE {
	.Version			UINT32
	.NumberOfEntries	UINT32
	.DescriptorSize		UINT32
	.Reserved			UINT32
}

struc EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
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

struc SIMPLE_TEXT_OUTPUT_MODE {
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
;Read/Write Uninitialized Data Used With Macros (and Future Code)
section '.bss' data readable writeable

_ImageHandle:		EFI_HANDLE
_SystemTablePtr:	POINTER		;EFI_SYSTEM_TABLE
_ConOutPtr:			POINTER		;EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
_rspTemp:			UINTN		;Used for calling Functions

;-------------------------------------;
;Reference Function Calling Convention;
;-------------------------------------;
;Based on Microsoft x64 calling convention

	;Arguments
	;rcx	or xmm0
	;rdx	or xmm1
	;r8		or xmm2
	;r9		or xmm3
	;stack	(right-justified, 16-byte alignment) push 5th argument first

	;Return
	;rax	as value only when data fits (typically status code)
	;rcx	used when returning on a pointer

	;Volatile Registers - Caller Must Save Elsewhere Before Calling
	;rax,rcx,rdx,r8,r9,xmm0-xmm3
	;r10,r11,xmm4,xmm5
	;Preserved otherwise

;More Details: UEFI specification
	;https://en.wikipedia.org/wiki/X86_calling_conventions#Microsoft_x64_calling_convention
	;https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention

;------;
;Macros;
;------;
;section '.text' code executable readable	;No Code Yet

macro ExitSuccessfullyUEFI {
	mov rax, EFI_SUCCESS
	ret		;Return
}

macro ExitInvalidParameterUEFI {
	mov rax, EFI_INVALID_PARAMETER
	ret		;Return
}

;Virtual Structures So Offsets Can Be Used in Effective Addressing without making copies of UEFI data when not necessary
macro VirtualStructure struct,name {
	virtual at 0
		name struct
	end virtual
}

VirtualStructure EFI_SYSTEM_TABLE, SystemTable

;Helper UEFI initialization macro that should be called at the beginning of a UEFI application
;Must be called before using any other library macros and function code and before rcx,rdx,rsp change
;UEFI Handoff State
;rcx = EFI_HANDLE - ImageHandle
;rdx = POINTER - EFI_SYSTEM_TABLE
;rsp = return address (stack pointer register)
macro InitializeUEFI {
	;Move register values into data locations
	mov [_ImageHandle], rcx
	mov [_SystemTablePtr], rdx
	;Need to save other important UEFI variables here including stack pointer...?
	;Do some basic checking on system Table Here
	;Start initializing other UEFI Services here potentially
}



;Helper call function macro based on the specified UEFI calling convention
;Runs on General Registers Only for right now
;Does not currently account for return values possibly needed in arguments, arg5+
;Reminder, push accepts arguments from memory or registers
macro CallFunction function*,arg1,arg2,arg3,arg4,argE { ;[argE] {
common
	;Save Volatile Register State (Might be able to simplfy later (instruction or macro or understanding))
	;Need to Work on logic too
;	push	rax
;	push	rcx
;	push	rdx
;	push	r8
;	push	r9
	push	r10
	push	r11
	;Put Arguments in corresponding registers if they exist
if ~ arg1 eq
	mov		rcx,arg1
end if
if ~ arg2 eq
	mov		rdx,arg2
end if
if ~ arg3 eq
	mov		r8,arg3
end if
if ~ arg4 eq
	mov		r9,arg4
end if
	;Save Stack Pointer
	mov	[_rspTemp],rsp
	and	esp, 0FFFFFFF0h ;Stack alignment for some reason
if ~ argE eq
reverse		;Push arguments in argE list in reverse order onto the stack
	push	argE
end if
common
	sub	rsp, NATIVE_BYTE_COUNT*4	;Reserve "shadow space" for parameters passed by register
    call function	;rbx should be used for this...?
	mov rsp,[_rspTemp]
	pop		r11
	pop		r10
}

VirtualStructure EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL, ConOut

macro InitializeConOut {
;Use effective addressing to set the ConOutPtr memory
	mov rbx,[_SystemTablePtr]
	mov	rbx,[rbx + SystemTable.ConOutPtr]
	mov [_ConOutPtr],rbx
}

macro ConOutOutputString string {
	mov rbx,[_ConOutPtr]
	mov rbx,[rbx + ConOut.OutputString]		;Set Function in Register
	CallFunction rbx, [_ConOutPtr], string
}

;End of File

