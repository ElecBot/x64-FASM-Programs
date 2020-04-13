;=================;
;UEFI FASM Library;
;=================;
;Based on the Unified Extensible Firmware Interface (UEFI) 2.8 version specification
;	https://uefi.org/specifications
;Relies on features supported in at least Flat Assembler (FASM) 1.73.22
;	https://flatassembler.net
;Targets x64 Platforms: Supports Assembly-Based UEFI Programs
;A Tab Width of 4 is optimal, no line should contain more than 100 characters
;This text file is the entry point of the UEFI FASM library and should be included into projects.
;	Common UEFI constants, structures, and macros are defined in this file.
;	Supported included files extend the library capabilities into seperate sections.
;Updated April 13th 2020

;--------------------------;
;Common Numerical Constants;
;--------------------------;
;PE32+ Image (Executable File) Header Relevant UEFI Values
;	Subsystem Type for EFI Images
	EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION			=	10
	EFI_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER	=	11
	EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER		=	12

;	Machine Type for EFI Images
	EFI_IMAGE_MACHINE_IA32				=	0x014c
	EFI_IMAGE_MACHINE_IA64				=	0x0200
	EFI_IMAGE_MACHINE_EBC				=	0x0EBC
	EFI_IMAGE_MACHINE_x64				=	0x8664	;Value Used in x64 Executable File Header
	EFI_IMAGE_MACHINE_ARMTHUMB_MIXED	=	0x01C2
	EFI_IMAGE_MACHINE_AARCH64			=	0xAA64
	EFI_IMAGE_MACHINE_RISCV32			=	0x5032
	EFI_IMAGE_MACHINE_RISCV64			=	0x5064
	EFI_IMAGE_MACHINE_RISCV128			=	0x5128

;Logical Boolean Values
	FALSE	=	0
	TRUE	=	1

;Function Parameters
	NULL	=	0

;EFI System Table
;Little Endian Ordering...
	EFI_SYSTEM_TABLE_SIGNATURE		=	0x5453595320494249
	EFI_2_80_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 80
	EFI_2_70_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 70
	EFI_2_60_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 60
	EFI_2_50_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 50
	EFI_2_40_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 40
	EFI_2_31_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 31
	EFI_2_30_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 30
	EFI_2_20_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 20
	EFI_2_10_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 10
	EFI_2_00_SYSTEM_TABLE_REVISION	=	(2 shl 16) or 0
	EFI_1_10_SYSTEM_TABLE_REVISION	=	(1 shl 16) or 10
	EFI_1_02_SYSTEM_TABLE_REVISION	=	(1 shl 16) or 2
	EFI_SYSTEM_TABLE_REVISION		=	EFI_2_80_SYSTEM_TABLE_REVISION	;Doesn't Match Standard
	EFI_SPECIFICATION_VERSION		=	EFI_SYSTEM_TABLE_REVISION

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

;-------------------------;
;Common Symbolic Constants;
;-------------------------;
;Common UEFI Data Types
;	Structure Reservations
	BOOLEAN		equ		rb 1	;Logical Boolean: 0=FALSE, 1=TRUE, Other=Undefined
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
	INTN		equ		INT64	;Native width value valid only for x64 target
	UINTN		equ		UINT64	;Native width value valid only for x64 target
	POINTER		equ		UINTN	;Pointer
	FUNCTION	equ		POINTER	;Function, which is simply a pointer to the callable function
	VOID_PTR	equ		POINTER	;Pointer to any / undefined type
	EFI_GUID	equ		rq 2	;Unique Identifier Value, with alignment on 64-bit/8-byte boundary
	EFI_STATUS	equ		UINTN	;Status code
	EFI_HANDLE	equ		VOID_PTR;Collection of related interfaces
	EFI_EVENT	equ		VOID_PTR;Handle to an event structure
	EFI_LBA		equ		UINT64	;Logical Block Address
	EFI_TPL		equ		UINTN	;Task Priority Level

;	Networking Related Reservations
	EFI_MAC_ADDRESS		equ		rq 4	;Media Access Control Address, 32-byte buffer
	EFI_IPv4_ADDRESS	equ		rd 1	;IPv4 internet protocol address, 4-byte buffer
	EFI_IPv6_ADDRESS	equ		rq 2	;IPv6 internet protocol address, 16-byte buffer
	EFI_IP_ADDRESS		equ		rq 2	;Either internet protocol address, 16-byte buffer
										;	Aligned on 4-byte boundary

;----------;
;Structures;
;----------;
;Might Need to Add More Alignment Values In Future

struc _EFI_TABLE_HEADER {
	.Signature    	UINT64
	.Revision     	UINT32
	.HeaderSize   	UINT32
	.CRC32        	UINT32	;CCITT32 CRC algorithm - Seed Polynomial: 0x04c11db7
	.Reserved     	UINT32
}

struc _EFI_SYSTEM_TABLE {
	.Hdr					_EFI_TABLE_HEADER
	.FirmwareVendorPtr		POINTER			;CHAR16
	.FirmwareRevision		UINT32
align EIGHT_BYTE_BOUNDARY					;Necessary For x64 Structure Alignment Preferences
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

;Expand the following Information More
struc _EFI_CONFIGURATION_TABLE {
	.VendorGuid			EFI_GUID
	.VendorTablePtr		POINTER
}

struc _EFI_PROPERTIES_TABLE {
	.Version						UINT32
	.Length							UINT32
	.MemoryProtectionAttribute		UINT64
}

struc _EFI_MEMORY_ATTRIBUTES_TABLE {
	.Version			UINT32
	.NumberOfEntries	UINT32
	.DescriptorSize		UINT32
	.Reserved			UINT32
}

;----;
;Data;
;----;
;Read/Write Uninitialized Data Used With Macros (and Future Code)
section '.bss' data readable writeable

ImageHandle:			EFI_HANDLE
SystemTablePtr:			POINTER		;EFI_SYSTEM_TABLE
ReturnAddressPtr:		POINTER
TemporaryStackPointer:	UINTN		;Used when calling Functions

;-------------------------------------;
;Reference Function Calling Convention;
;-------------------------------------;
;Based on Microsoft x64 calling convention

	;Arguments
	;rcx	or xmm0
	;rdx	or xmm1
	;r8		or xmm2
	;r9		or xmm3
	;stack	(right-justified, 16-byte alignment) push 5th argument last

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
;section '.text' code executable readable	;No Code Yet (or ever...)

macro ExitSuccessfullyUEFI {
	mov rax, EFI_SUCCESS
	mov rsp, [ReturnAddressPtr]
	ret		;Return
}

macro ExitInvalidParameterUEFI {
	mov rax, EFI_INVALID_PARAMETER
	mov rsp, [ReturnAddressPtr]
	ret		;Return
}

;Virtual Structures So Offsets Can Be Used in Effective Addressing
;	Does not make copies of UEFI data where not necessary
macro VirtualStructure struct,name {
	virtual at 0
		name struct
	end virtual
}

;Virtual structures used in below macros
VirtualStructure _EFI_TABLE_HEADER, EFI_TABLE_HEADER
VirtualStructure _EFI_SYSTEM_TABLE, EFI_SYSTEM_TABLE
VirtualStructure _EFI_CONFIGURATION_TABLE, EFI_CONFIGURATION_TABLE

;Helper UEFI initialization macro that should be called at the beginning of a UEFI application
;Must be called before using any other library macros and before rcx,rdx,rsp change
;UEFI Handoff State
;rcx = EFI_HANDLE - ImageHandle
;rdx = POINTER - EFI_SYSTEM_TABLE
;rsp = return address (stack pointer register)
macro InitializeUEFI {
	;Move register values into data locations
	mov [ImageHandle], rcx
	mov [SystemTablePtr], rdx
	mov [ReturnAddressPtr], rsp		;Saving Stack Pointer For Exiting UEFI
	;and	rsp, (not 0xF) ;16-Byte stack alignment...needs double checking and understanding
	;Do some basic checking on system Table Here
}


;Helper call function macro for UEFI functions based on the specified calling convention above
;Only works with general registers / and addresses for now integer arguments for now
;rax and derivatives is always considered volatile but returns the result

;Self Reminder...push accepts arguments from memory or registers
;Compatibility Remider...only push and pop with registers...understanding
macro CallFunction function*,arg1,arg2,arg3,arg4,[argE] {
common
;Saves all argument registers except when registers are pre-setup
local rcxPush,rdxPush,r8Push,r9Push 
	rcxPush	= 1
	rdxPush	= 1
	r8Push	= 1
	r9Push	= 1
local numberOfStackArguments
	numberOfStackArguments = 0
;local saver10, saver11
	;saver10 = 0
	;saver11 = 0
	push r10	;Save r10 and r11 always
	push r11
	;Put Arguments in corresponding registers if they exist and are different
if ~ arg1 eq		;If True then there is a first argument
	if ~ arg1 in <cl,cx,ecx,rcx>		;Push / save rcx if argument isn't rcx or a derivative
		push rcx
	;Maybe rearrange if-else if order for faster FASM assembly time
	if arg1 in <dl,r8b,r9b,r10b,r11b,r12b,r13b,r14b,r15b>
		mov cl,arg1
	else if arg1 in <dx,r8w,r9w,r10w,r11w,r12w,r13w,r14w,r15w>
		mov cx,arg1
	else if arg1 in <edx,r8d,r9d,r10d,r11d,r12d,r13d,r14d,r15d>
		mov ecx,arg1
	else if arg1 in <rdx,r8,r9,r10,r11,r12,r13,r14,r15>
		mov rcx,arg1
	else if arg1 in <al,ah,ax,eax,rax,bl,bh,bx,ebx,rbx,ch,dh,\
					spl,sp,esp,rsp,bpl,bp,ebp,sil,si,esi,rsi,dil,di,edi,rdi>
		err 1
	else	;Handling addresses / immediate value / offset...will throw fasm error when issue
		mov rcx,arg1
	end if
	else
		rcxPush = 0
	end if
else
	push rcx
end if
if ~ arg2 eq
	if ~ arg2 in <dl,dx,edx,rdx>
		push rdx
	if arg2 in <r8b,r9b,r10b,r11b,r12b,r13b,r14b,r15b>
		mov dl,arg2
	else if arg2 in <r8w,r9w,r10w,r11w,r12w,r13w,r14w,r15w>
		mov dx,arg2
	else if arg2 in <r8d,r9d,r10d,r11d,r12d,r13d,r14d,r15d>
		mov edx,arg2
	else if arg2 in <r8,r9,r10,r11,r12,r13,r14,r15>
		mov rdx,arg2
	else if arg2 in <al,ah,ax,eax,rax,bl,bh,bx,ebx,rbx,cl,ch,cx,ecx,rcx,dh,\
					spl,sp,esp,rsp,bpl,bp,ebp,sil,si,esi,rsi,dil,di,edi,rdi>
		err 1
	else	;Handling addresses / immediate value / offset...will throw fasm error when issue
		mov rdx,arg2
	end if
	else
		rdxPush = 0
	end if
else
	push rdx
end if
if ~ arg3 eq
	if ~ arg3 in <r8b,r8w,r8d,r8>
		push r8
	if arg3 in <r9b,r10b,r11b,r12b,r13b,r14b,r15b>
		mov r8b,arg3
	else if arg3 in <r9w,r10w,r11w,r12w,r13w,r14w,r15w>
		mov r8w,arg3
	else if arg3 in <r9d,r10d,r11d,r12d,r13d,r14d,r15d>
		mov r8d,arg3
	else if arg3 in <r9,r10,r11,r12,r13,r14,r15>
		mov r8,arg3
	else if arg3 in <al,ah,ax,eax,rax,bl,bh,bx,ebx,rbx,cl,ch,cx,ecx,rcx,dl,dh,dx,edx,rdx,\
					spl,sp,esp,rsp,bpl,bp,ebp,sil,si,esi,rsi,dil,di,edi,rdi>
		err 1
	else	;Handling addresses / immediate value / offset...will throw fasm error when issue
		mov r8,arg3
	end if
	else
		r8Push = 0
	end if
else
	push r8
end if
if ~ arg4 eq
	if ~ arg4 in <r9b,r9w,r9d,r9>
		push r9
	if arg4 in <r10b,r11b,r12b,r13b,r14b,r15b>
		mov r9b,arg4
	else if arg4 in <r10w,r11w,r12w,r13w,r14w,r15w>
		mov r9w,arg4
	else if arg4 in <r10d,r11d,r12d,r13d,r14d,r15d>
		mov r9d,arg4
	else if arg4 in <r10,r11,r12,r13,r14,r15>
		mov r9,arg4
	else if arg4 in <al,ah,ax,eax,rax,bl,bh,bx,ebx,rbx,cl,ch,cx,ecx,rcx,dl,dh,dx,edx,rdx,\
					r8b,r8w,r8q,r8,\
					spl,sp,esp,rsp,bpl,bp,ebp,sil,si,esi,rsi,dil,di,edi,rdi>
		err 1
	else	;Handling addresses / immediate value / offset...will throw fasm error when issue
		mov r9,arg4
	end if
	else
		r9Push = 0
	end if
else
	push r9
end if
	mov [TemporaryStackPointer],rsp ;Save Stack Pointer
	and rsp,(not 0xF)	;Always Realign the stack to 16-byte boundary...for now
if ~ argE eq	;True if there are any remaining arguments
forward	;Counte Number of Additional / Stack Arguments
	numberOfStackArguments = numberOfStackArguments + 1
common
	if numberOfStackArguments and 1	;True if odd
		push rax	;Maybe sub rsp,4 would be better? to keep alignment
	end if
reverse ;Push remaing arguments (in argE list) in reverse order onto the stack
	;moving into rax before pushing for compatibility reasons
	if argE in <al,ah,ax,eax,rax,bl,bh,bx,ebx,rbx,cl,ch,cx,ecx,rcx,dl,dh,dx,edx,rdx,\
					r8b,r8w,r8q,r8,r9b,r9w,r9q,r9\
					spl,sp,esp,rsp,bpl,bp,ebp,sil,si,esi,rsi,dil,di,edi,rdi>
		err Invalid argument argE
	else if argE in <r10b,r11b,r12b,r13b,r14b,r15b>
		mov al,argE
	else if argE in <r10w,r11w,r12w,r13w,r14w,r15w>
		mov ax,argE
	else if argE in <r10d,r11d,r12d,r13d,r14d,r15d>
		mov eax,argE
	else if argE in <r10,r11,r12,r13,r14,r15>
		mov rax,argE
	else
		mov rax,argE
	end if
	push rax
common	;Ending Common value
end if
common
	sub		rsp, 8*4	;Reserve "shadow space" for parameters passed by register
	call	function	;rbx should be used for this...?
	mov		rsp,[TemporaryStackPointer]

if r9Push
	pop r9
end if
if r8Push
	pop r8
end if
if rdxPush
	pop rdx
end if
if rcxPush
	pop rcx
end if
	pop r11
	pop r10
}

macro enum [parameters] {	;C-like simple enum declaration
common
	local enumValue
	enumValue = 0
forward
	parameters	=	enumValue
	enumValue	=	enumValue + 1
}

macro ClearRegister register* {		;Contains Optimal way to Clear / Reset Register
	xor	register,register
}

;----------------------------------;
;UEFI FASM Library Support Sections;
;----------------------------------;
;Included Library Files
include 'BootServices.inc'
include 'RuntimeServices.inc'
include 'ConsoleSupport.inc'
;include 'UEFIdata.inc'

;--------------------------;
;Symbolic Constants (Cont.);
;--------------------------;
;UEFI Data Type Definition Override for use in program data declerations
	BOOLEAN		equ		db		;0=FALSE, 1=TRUE, Other=Undefined
	INT8		equ		db
	UINT8		equ		db
	INT16		equ		dw
	UINT16		equ		dw
	INT32		equ		dd
	UINT32		equ		dd
	INT64		equ		dq
	UINT64		equ		dq
	INT128		equ		dq		;Must have 2 values per data type
	UINT128		equ		dq		;Must have 2 values per data type
	CHAR8		equ		db
	CHAR16		equ		du
	INTN		equ		INT64	;Native Width Value Valid Only for 64-bit
	UINTN		equ		UINT64	;Native Width Value Valid Only for 64-bit

