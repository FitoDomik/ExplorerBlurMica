#pragma once
#include "../framework.h"
#include <winternl.h>
namespace MBox
{
	class DllNotifyRoutine
	{
	public:
		static DllNotifyRoutine& GetInstance();
		DllNotifyRoutine();
		~DllNotifyRoutine() noexcept;
		DllNotifyRoutine(const DllNotifyRoutine&) = delete;
		DllNotifyRoutine& operator=(const DllNotifyRoutine&) = delete;
		struct DllInfo
		{
			PCUNICODE_STRING FullDllName;   
			PCUNICODE_STRING BaseDllName;   
			PVOID DllBase;                  
			ULONG SizeOfImage;              
		};
		using Callback = std::function<void(bool load, DllInfo info)>;
		void AddCallback(Callback callback);
		void DeleteCallback(Callback callback);
	private:
		typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA
		{
			ULONG Flags;                    
			PCUNICODE_STRING FullDllName;   
			PCUNICODE_STRING BaseDllName;   
			PVOID DllBase;                  
			ULONG SizeOfImage;              
		} LDR_DLL_LOADED_NOTIFICATION_DATA, * PLDR_DLL_LOADED_NOTIFICATION_DATA;
		typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA
		{
			ULONG Flags;                    
			PCUNICODE_STRING FullDllName;   
			PCUNICODE_STRING BaseDllName;   
			PVOID DllBase;                  
			ULONG SizeOfImage;              
		} LDR_DLL_UNLOADED_NOTIFICATION_DATA, * PLDR_DLL_UNLOADED_NOTIFICATION_DATA;
		typedef union _LDR_DLL_NOTIFICATION_DATA
		{
			LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
			LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
		} LDR_DLL_NOTIFICATION_DATA, * PLDR_DLL_NOTIFICATION_DATA;
		typedef LDR_DLL_NOTIFICATION_DATA* PCLDR_DLL_NOTIFICATION_DATA;
		typedef VOID(CALLBACK* PLDR_DLL_NOTIFICATION_FUNCTION)(
			ULONG NotificationReason,
			PCLDR_DLL_NOTIFICATION_DATA NotificationData,
			PVOID Context
			);
		static VOID CALLBACK LdrDllNotification(
			ULONG NotificationReason,
			PCLDR_DLL_NOTIFICATION_DATA NotificationData,
			PVOID Context
		);
		static constexpr UINT LDR_DLL_NOTIFICATION_REASON_LOADED{ 1 };
		static constexpr UINT LDR_DLL_NOTIFICATION_REASON_UNLOADED{ 2 };
		bool m_internalError{ false };
		PVOID m_cookie{ nullptr };
		NTSTATUS(NTAPI* m_actualLdrRegisterDllNotification)(ULONG, PLDR_DLL_NOTIFICATION_FUNCTION, PVOID, PVOID*);
		NTSTATUS(NTAPI* m_actualLdrUnregisterDllNotification)(PVOID);
		std::vector<Callback> m_callbackList{};
	};
}