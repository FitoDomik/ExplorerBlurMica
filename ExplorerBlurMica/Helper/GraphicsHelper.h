#pragma once
#include "../framework.h"
#ifdef GDIPVER
#undef GDIPVER
#define GDIPVER 0x0110
#else
#define GDIPVER 0x0110
#endif
#include <comdef.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
namespace MBox
{
	namespace Utils
	{
		extern constexpr HRESULT hresult_from_gpstatus(Gdiplus::GpStatus const status);
	}
	class LazyGdip
	{
		ULONG_PTR						m_gdiplusToken{ 0 };
		Gdiplus::GdiplusStartupOutput	m_gdiplusOutput{};
	public:
		static HRESULT EnsureInitialized();
		static LazyGdip& Instance();
		~LazyGdip() noexcept;
		LazyGdip(const LazyGdip&) = delete;
		LazyGdip& operator=(const LazyGdip&) = delete;
		Gdiplus::Bitmap* CreateBitmapFromFile(std::wstring_view fileName) const;
		Gdiplus::Bitmap* CreateBitmapFromMemory(const BYTE* pInit, UINT cbInit) const;
	private:
		LazyGdip() = default;
		HRESULT Startup();
	};
}