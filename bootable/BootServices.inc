;=====================;
;Boot Services Section;
;=====================;
;Section of UEFI FASM Library
;Updated April 10th 2020

;-------------------;
;Numerical Constants;
;-------------------;
;EFI Boot Services
	EFI_BOOT_SERVICES_SIGNATURE		=	0x56524553544f4f42
	EFI_BOOT_SERVICES_REVISION		=	EFI_SPECIFICATION_VERSION

;Event Types
	EVT_TIMER							=	0x80000000
	EVT_RUNTIME							=	0x40000000
	EVT_NOTIFY_WAIT						=	0x00000100
	EVT_NOTIFY_SIGNAL					=	0x00000200
	EVT_SIGNAL_EXIT_BOOT_SERVICES		=	0x00000201
	EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE	=	0x60000202

;EFI_TIMER_DELAY
enum TimerCancel, TimerPeriodic, TimerRelative

;EFI_TPL Task Priority Levels
	TPL_APPLICATION	= 	 4
	TPL_CALLBACK	=	 8
	TPL_NOTIFY		=	16
	TPL_HIGH_LEVEL	=	31

;EFI_ALLOCATE_TYPE
	AllocateAnyPages	=	0
	AllocateMaxAddress	=	1
	AllocateAddress		=	2
	MaxAllocateType		=	3

;EFI_MEMORY_TYPE
	EfiReservedMemoryType		=	0
	EfiLoaderCode				=	1
	EfiLoaderData				=	2
	EfiBootServicesCode			=	3
	EfiBootServicesData			=	4
	EfiRuntimeServicesCode		=	5
	EfiRuntimeServicesData		=	6
	EfiConventionalMemory		=	7
	EfiUnusableMemory			=	8
	EfiACPIReclaimMemory		=	9
	EfiACPIMemoryNVS			=	10
	EfiMemoryMappedIO			=	11
	EfiMemoryMappedIOPortSpace	=	12
	EfiPalCode					=	13
	EfiPersistentMemory			=	14
	EfiMaxMemoryType			=	15

;Memory Attribute Definitions
	EFI_MEMORY_UC				=	0x0000000000000001
	EFI_MEMORY_WC				=	0x0000000000000002
	EFI_MEMORY_WT				=	0x0000000000000004
	EFI_MEMORY_WB				=	0x0000000000000008
	EFI_MEMORY_UCE				=	0x0000000000000010
	EFI_MEMORY_WP				=	0x0000000000001000
	EFI_MEMORY_RP				=	0x0000000000002000
	EFI_MEMORY_XP				=	0x0000000000004000
	EFI_MEMORY_NV				=	0x0000000000008000
	EFI_MEMORY_MORE_RELIABLE	=	0x0000000000010000
	EFI_MEMORY_RO				=	0x0000000000020000
	EFI_MEMORY_SP				=	0x0000000000040000
	EFI_MEMORY_CPU_CRYPTO		=	0x0000000000080000
	EFI_MEMORY_RUNTIME			=	0x8000000000000000

;Memory Descriptor Version Number
	EFI_MEMORY_DESCRIPTOR_VERSION	=	1

;------------------;
;Symbolic Constants;
;------------------;
	EFI_PHYSICAL_ADDRESS	equ		UINT64
	EFI_VIRTUAL_ADDRESS		equ		UINT64

;----------;
;Structures;
;----------;
struc _EFI_BOOT_SERVICES {
	.Hdr				_EFI_TABLE_HEADER
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

struc _EFI_MEMORY_DESCRIPTOR {
	.Type			UINT32
	.PhysicalStart	EFI_PHYSICAL_ADDRESS
	.VirtualStart	EFI_VIRTUAL_ADDRESS
	.NumberOfPages	UINT64
	.Attribute		UINT64
}

;----;
;Data;
;----;
;Read/Write Uninitialized Data Used With Macros
section '.bss' data readable writeable
BootServicesPtr:		POINTER		;EFI_BOOT_SERVICES
MemoryDescriptor	_EFI_MEMORY_DESCRIPTOR	;Data Structure used to describe memory

;------;
;Macros;
;------;
VirtualStructure _EFI_BOOT_SERVICES, EFI_BOOT_SERVICES

macro BootServicesInitialize {
	mov rbx,[SystemTablePtr]
	mov	rbx,[rbx + EFI_SYSTEM_TABLE.BootServicesPtr]
	mov [BootServicesPtr],rbx
}

macro BootServicesFunction function*, [data] { ;Set Function in Register
common
	mov rbx,[BootServicesPtr]
	mov rbx,[rbx + EFI_BOOT_SERVICES.#function]
	CallFunction rbx, data
}







