#pragma once
#include "framework.h"
#include "Helper/comdef.h"
#include "Helper/WinRTHelper.h"
#include "visualtreewatcher.h"
namespace MBox
{
	constexpr CLSID CLSID_TAPSite =
	{ 0x233a5395, 0x98b8, 0x40c9, { 0x8d, 0xee, 0x1b, 0xd9, 0x15, 0x8b, 0x34, 0xca } };
	struct TAPSite : implements<TAPSite, IObjectWithSite, winrt::non_agile>
	{
	public:
		HRESULT STDMETHODCALLTYPE SetSite(IUnknown* pUnkSite) override;
		HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppvSite) noexcept override;
		static HRESULT ShutdownVisualTreeWatcher();
	private:
		static com_ptr<VisualTreeWatcher> s_VisualTreeWatcher;
		com_ptr<IUnknown> m_site = nullptr;
	};
}