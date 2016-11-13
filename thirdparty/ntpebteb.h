#pragma once

#ifndef _NTPEBTEB_H
#define _NTPEBTEB_H

typedef LONG32 NTSTATUS;

typedef struct _RTL_USER_PROCESS_PARAMETERS *PRTL_USER_PROCESS_PARAMETERS;
typedef struct _RTL_CRITICAL_SECTION *PRTL_CRITICAL_SECTION;

#define GDI_HANDLE_BUFFER_SIZE32 34
#define GDI_HANDLE_BUFFER_SIZE64 60

#ifndef WIN64
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE32
#else
#define GDI_HANDLE_BUFFER_SIZE GDI_HANDLE_BUFFER_SIZE64
#endif

typedef ULONG GDI_HANDLE_BUFFER[GDI_HANDLE_BUFFER_SIZE];

typedef ULONG GDI_HANDLE_BUFFER32[GDI_HANDLE_BUFFER_SIZE32];
typedef ULONG GDI_HANDLE_BUFFER64[GDI_HANDLE_BUFFER_SIZE64];

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

typedef struct _PEB_LDR_DATA {
	ULONG Length;
	BOOLEAN Initialized;
	HANDLE SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
	BOOLEAN ShutdownInProgress;
	HANDLE ShutdownThreadId;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	_Field_size_bytes_part_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// symbols
typedef struct _PEB {
	BOOLEAN InheritedAddressSpace;
	BOOLEAN ReadImageFileExecOptions;
	BOOLEAN BeingDebugged;
	union {
		BOOLEAN BitField;
		struct {
			BOOLEAN ImageUsesLargePages : 1;
			BOOLEAN IsProtectedProcess : 1;
			BOOLEAN IsImageDynamicallyRelocated : 1;
			BOOLEAN SkipPatchingUser32Forwarders : 1;
			BOOLEAN IsPackagedProcess : 1;
			BOOLEAN IsAppContainer : 1;
			BOOLEAN IsProtectedProcessLight : 1;
			BOOLEAN IsLongPathAwareProcess : 1;
		};
	};

	HANDLE Mutant;

	PVOID ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	PVOID SubSystemData;
	PVOID ProcessHeap;
	PRTL_CRITICAL_SECTION FastPebLock;
	PVOID AtlThunkSListPtr;
	PVOID IFEOKey;
	union {
		ULONG CrossProcessFlags;
		struct {
			ULONG ProcessInJob : 1;
			ULONG ProcessInitializing : 1;
			ULONG ProcessUsingVEH : 1;
			ULONG ProcessUsingVCH : 1;
			ULONG ProcessUsingFTH : 1;
			ULONG ReservedBits0 : 27;
		};
	};
	union {
		PVOID KernelCallbackTable;
		PVOID UserSharedInfoPtr;
	};
	ULONG SystemReserved[1];
	ULONG AtlThunkSListPtr32;
	PVOID ApiSetMap;
	ULONG TlsExpansionCounter;
	PVOID TlsBitmap;
	ULONG TlsBitmapBits[2];
	PVOID ReadOnlySharedMemoryBase;
	PVOID HotpatchInformation;
	PVOID *ReadOnlyStaticServerData;
	PVOID AnsiCodePageData;
	PVOID OemCodePageData;
	PVOID UnicodeCaseTableData;

	ULONG NumberOfProcessors;
	ULONG NtGlobalFlag;

	LARGE_INTEGER CriticalSectionTimeout;
	SIZE_T HeapSegmentReserve;
	SIZE_T HeapSegmentCommit;
	SIZE_T HeapDeCommitTotalFreeThreshold;
	SIZE_T HeapDeCommitFreeBlockThreshold;

	ULONG NumberOfHeaps;
	ULONG MaximumNumberOfHeaps;
	PVOID *ProcessHeaps;

	PVOID GdiSharedHandleTable;
	PVOID ProcessStarterHelper;
	ULONG GdiDCAttributeList;

	PRTL_CRITICAL_SECTION LoaderLock;

	ULONG OSMajorVersion;
	ULONG OSMinorVersion;
	USHORT OSBuildNumber;
	USHORT OSCSDVersion;
	ULONG OSPlatformId;
	ULONG ImageSubsystem;
	ULONG ImageSubsystemMajorVersion;
	ULONG ImageSubsystemMinorVersion;
	ULONG_PTR ActiveProcessAffinityMask;
	GDI_HANDLE_BUFFER GdiHandleBuffer;
	PVOID PostProcessInitRoutine;

	PVOID TlsExpansionBitmap;
	ULONG TlsExpansionBitmapBits[32];

	ULONG SessionId;

	ULARGE_INTEGER AppCompatFlags;
	ULARGE_INTEGER AppCompatFlagsUser;
	PVOID pShimData;
	PVOID AppCompatInfo;

	UNICODE_STRING CSDVersion;

	PVOID ActivationContextData;
	PVOID ProcessAssemblyStorageMap;
	PVOID SystemDefaultActivationContextData;
	PVOID SystemAssemblyStorageMap;

	SIZE_T MinimumStackCommit;

	PVOID *FlsCallback;
	LIST_ENTRY FlsListHead;
	PVOID FlsBitmap;
	ULONG FlsBitmapBits[FLS_MAXIMUM_AVAILABLE / (sizeof(ULONG) * 8)];
	ULONG FlsHighIndex;

	PVOID WerRegistrationData;
	PVOID WerShipAssertPtr;
	PVOID pContextData;
	PVOID pImageHeaderHash;
	union {
		ULONG TracingFlags;
		struct {
			ULONG HeapTracingEnabled : 1;
			ULONG CritSecTracingEnabled : 1;
			ULONG LibLoaderTracingEnabled : 1;
			ULONG SpareTracingBits : 29;
		};
	};
	ULONGLONG CsrServerReadOnlySharedMemoryBase;
	PVOID TppWorkerpListLock;
	LIST_ENTRY TppWorkerpList;
	PVOID WaitOnAddressHashTable[128];
} PEB, *PPEB;

#define GDI_BATCH_BUFFER_SIZE 310

typedef struct _GDI_TEB_BATCH {
	ULONG Offset;
	ULONG_PTR HDC;
	ULONG Buffer[GDI_BATCH_BUFFER_SIZE];
} GDI_TEB_BATCH, *PGDI_TEB_BATCH;

typedef struct _TEB_ACTIVE_FRAME_CONTEXT {
	ULONG Flags;
	PSTR FrameName;
} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT;

typedef struct _TEB_ACTIVE_FRAME {
	ULONG Flags;
	struct _TEB_ACTIVE_FRAME *Previous;
	PTEB_ACTIVE_FRAME_CONTEXT Context;
} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;

typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME {
	struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME *Previous;
	struct _ACTIVATION_CONTEXT                 *ActivationContext;
	ULONG                                       Flags;
} RTL_ACTIVATION_CONTEXT_STACK_FRAME, *PRTL_ACTIVATION_CONTEXT_STACK_FRAME;

typedef struct _ACTIVATION_CONTEXT_STACK {
	ULONG                               Flags;
	ULONG                               NextCookieSequenceNumber;
	RTL_ACTIVATION_CONTEXT_STACK_FRAME *ActiveFrame;
	LIST_ENTRY                          FrameListCache;
} ACTIVATION_CONTEXT_STACK, *PACTIVATION_CONTEXT_STACK;

typedef struct _TEB {
	NT_TIB64 NtTib;                                             // +0x000

	PVOID EnvironmentPointer;                                   // +0x038
	CLIENT_ID ClientId;                                         // +0x040
	PVOID ActiveRpcHandle;                                      // +0x050
	PVOID ThreadLocalStoragePointer;                            // +0x058
	PPEB ProcessEnvironmentBlock;                               // +0x060

	ULONG LastErrorValue;                                       // +0x068
	ULONG CountOfOwnedCriticalSections;                         // +0x06c
	PVOID CsrClientThread;                                      // +0x070
	PVOID Win32ThreadInfo;                                      // +0x078
	ULONG User32Reserved[26];                                   // +0x080
	ULONG UserReserved[5];                                      // +0x0e8
	//UINT8 _PADDING0_[0x4];                                    // +0x0FC
	PVOID WOW32Reserved;                                        // +0x100
	LCID CurrentLocale;                                         // +0x108
	ULONG FpSoftwareStatusRegister;                             // +0x10c
	PVOID ReservedForDebuggerInstrumentation[16];               // +0x110
	PVOID SystemReserved1[37];                                  // +0x190
	UCHAR WorkingOnBehalfTicket[8];                             // +0x2b8
	NTSTATUS ExceptionCode;                                     // +0x2c0
	//UINT8 _PADDING1_[0x4];                                    // +0x2C4

	PACTIVATION_CONTEXT_STACK ActivationContextStackPointer;    // +0x2c8
	ULONG_PTR InstrumentationCallbackSp;                        // +0x2d0
	ULONG_PTR InstrumentationCallbackPreviousPc;                // +0x2d8
	ULONG_PTR InstrumentationCallbackPreviousSp;                // +0x2e0
	ULONG TxFsContext;                                          // +0x2e8

	BOOLEAN InstrumentationCallbackDisabled[4];                 // +0x2ec
	GDI_TEB_BATCH GdiTebBatch;                                  // +0x2f0
	CLIENT_ID RealClientId;                                     // +0x7d8
	HANDLE GdiCachedProcessHandle;                              // +0x7e8
	ULONG GdiClientPID;                                         // +0x7f0
	ULONG GdiClientTID;                                         // +0x7f4
	PVOID GdiThreadLocalInfo;                                   // +0x7f8
	ULONG_PTR Win32ClientInfo[62];                              // +0x800
	PVOID glDispatchTable[233];                                 // +0x9f0
	ULONG_PTR glReserved1[29];                                  // +0x1138
	PVOID glReserved2;                                          // +0x1220
	PVOID glSectionInfo;                                        // +0x1228
	PVOID glSection;                                            // +0x1230
	PVOID glTable;                                              // +0x1238
	PVOID glCurrentRC;                                          // +0x1240
	PVOID glContext;                                            // +0x1248

	NTSTATUS LastStatusValue;                                   // +0x1250
	//UINT8 _PADDING2_[0x4];                                    // +0x1254
	UNICODE_STRING StaticUnicodeString;                         // +0x1258
	WCHAR StaticUnicodeBuffer[261];                             // +0x1268
	//UINT8 _PADDING3_[0x6];                                    // +0x1472

	PVOID DeallocationStack;                                    // +0x1478
	PVOID TlsSlots[64];                                         // +0x1480
	LIST_ENTRY TlsLinks;                                        // +0x1680

	PVOID Vdm;                                                  // +0x1690
	PVOID ReservedForNtRpc;                                     // +0x1698
	PVOID DbgSsReserved[2];                                     // +0x16a0

	ULONG HardErrorMode;                                        // +0x16b0
	//UINT8 _PADDING4_[0x4];                                    // +0x16B4
#ifdef _WIN64
	PVOID Instrumentation[11];                                  // +0x16b8
#else
	PVOID Instrumentation[9];
#endif
	GUID ActivityId;                                            // +0x1710

	PVOID SubProcessTag;                                        // +0x1720
	PVOID PerflibData;                                          // +0x1728
	PVOID EtwTraceData;                                         // +0x1730
	PVOID WinSockData;                                          // +0x1738
	ULONG GdiBatchCount;                                        // +0x1740

	union {
		PROCESSOR_NUMBER CurrentIdealProcessor;                 // +0x1744
		ULONG IdealProcessorValue;                              // +0x1744
		struct {
			UCHAR ReservedPad0;                                 // +0x1744
			UCHAR ReservedPad1;                                 // +0x1745
			UCHAR ReservedPad2;                                 // +0x1746
			UCHAR IdealProcessor;                               // +0x1747
		};
	};

	ULONG GuaranteedStackBytes;                                 // +0x1748
	//UINT8 _PADDING5_[0x4];                                    // +0x174C
	PVOID ReservedForPerf;                                      // +0x1750
	PVOID ReservedForOle;                                       // +0x1758
	ULONG WaitingOnLoaderLock;                                  // +0x1760
	//UINT8 _PADDING6_[0x4];                                    // +0x1764
	PVOID SavedPriorityState;                                   // +0x1768
	ULONG_PTR ReservedForCodeCoverage;                          // +0x1770
	PVOID ThreadPoolData;                                       // +0x1778
	PVOID *TlsExpansionSlots;                                   // +0x1780
#ifdef _WIN64
	PVOID DeallocationBStore;                                   // +0x1788
	PVOID BStoreLimit;                                          // +0x1790
#endif
	ULONG MuiGeneration;                                        // +0x1798
	ULONG IsImpersonating;                                      // +0x179c
	PVOID NlsCache;                                             // +0x17a0
	PVOID pShimData;                                            // +0x17a8
	USHORT HeapVirtualAffinity;                                 // +0x17b0
	USHORT LowFragHeapDataSlot;                                 // +0x17b2
	//UINT8  _PADDING7_[0x4];                                   // +0x17b4
	HANDLE CurrentTransactionHandle;                            // +0x17b8
	PTEB_ACTIVE_FRAME ActiveFrame;                              // +0x17c0
	PVOID FlsData;                                              // +0x17c8

	PVOID PreferredLanguages;                                   // +0x17d0
	PVOID UserPrefLanguages;                                    // +0x17d8
	PVOID MergedPrefLanguages;                                  // +0x17e0
	ULONG MuiImpersonation;                                     // +0x17e8

	union {
		USHORT CrossTebFlags;                                   // +0x17ec
		USHORT SpareCrossTebBits : 16;                          // +0x17ec
	};

	union {
		USHORT SameTebFlags;                                    // +0x17ee
		struct {
			USHORT SafeThunkCall : 1;                           // +0x17ee
			USHORT InDebugPrint : 1;                            // +0x17ee
			USHORT HasFiberData : 1;                            // +0x17ee
			USHORT SkipThreadAttach : 1;                        // +0x17ee
			USHORT WerInShipAssertCode : 1;                     // +0x17ee
			USHORT RanProcessInit : 1;                          // +0x17ee
			USHORT ClonedThread : 1;                            // +0x17ee
			USHORT SuppressDebugMsg : 1;                        // +0x17ee
			USHORT DisableUserStackWalk : 1;                    // +0x17ee
			USHORT RtlExceptionAttached : 1;                    // +0x17ee
			USHORT InitialThread : 1;                           // +0x17ee
			USHORT SessionAware : 1;                            // +0x17ee
			USHORT LoadOwner : 1;                               // +0x17ee
			USHORT LoaderWorker : 1;                            // +0x17ee
			USHORT SpareSameTebBits : 2;                        // +0x17ee
		};
	};

	PVOID TxnScopeEnterCallback;                                // +0x17f0
	PVOID TxnScopeExitCallback;                                 // +0x17f8
	PVOID TxnScopeContext;                                      // +0x1800
	ULONG LockCount;                                            // +0x1808
	LONG WowTebOffset;                                          // +0x180c
	PVOID ResourceRetValue;                                     // +0x1810
	PVOID ReservedForWdf;                                       // +0x1818
	ULONGLONG ReservedForCrt;
	GUID EffectiveContainerId;
} TEB, *PTEB, *_PTEB;

#endif