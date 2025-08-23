#pragma once
#include "HookDef.h"
#include "visualtreewatcher.h"
namespace MBox::MiscTweaker
{
	void Startup();
	void Shutdown();
	extern LRESULT WINAPI MyDefWindowProcW(
		HWND hWnd,
		UINT Msg,
		WPARAM wParam,
		LPARAM lParam
	);
	extern HRESULT WINAPI MyDrawThemeBackground(
		HTHEME hTheme,
		HDC hdc,
		int iPartId,
		int iStateId,
		LPCRECT pRect,
		LPCRECT pClipRect
	);
	extern HRESULT WINAPI MyDrawThemeBackground_Address(
		HTHEME hTheme,
		HDC hdc,
		int iPartId,
		int iStateId,
		LPCRECT pRect,
		LPCRECT pClipRect
	);
	extern HRESULT __stdcall MyCoCreateInstance(
		REFCLSID rclsid,
		LPUNKNOWN pUnkOuter,
		DWORD dwClsContext,
		REFIID riid,
		LPVOID* ppv
	);
	extern HRESULT WINAPI MyGetThemeColor_WinUI(
		HTHEME hTheme,
		int iPartId,
		int iStateId,
		int iPropId,
		COLORREF* pColor
	);
	extern HRESULT WINAPI MyGetThemeColor_WASDK(
		HTHEME hTheme,
		int iPartId,
		int iStateId,
		int iPropId,
		COLORREF* pColor
	);
	void OnVisualTreeChange(
		com_ptr<VisualTreeWatcher> visualTreeWatcher,
		const ParentChildRelation& parentChildRelation,
		const VisualElement& element,
		VisualMutationType mutationType
	);
}