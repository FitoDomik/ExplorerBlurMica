#pragma once
#include "Helper/DirectUIHelper.h"
#include "WindowListener.h"
namespace MBox::DTweaker
{
	void DUIRequireVerticalLine();
	void DUIRequireHorizontalLine();
	void DUIRequireOutline();
	void WINAPI My_DirectUI_Element_PaintBackground_Filter(
		DirectUI::Element* This,
		HDC hdc,
		DirectUI::Value* value,
		LPCRECT pRect,
		LPCRECT pClipRect,
		LPCRECT pExcludeRect,
		LPCRECT pTargetRect
	);
	void WINAPI My_DirectUI_Element_PaintBackground_CleanUp(
		DirectUI::Element* This,
		HDC hdc,
		DirectUI::Value* value,
		LPCRECT pRect,
		LPCRECT pClipRect,
		LPCRECT pExcludeRect,
		LPCRECT pTargetRect
	);
	class BlinkEraserListener : public IWindowListenerCallback
	{
	public:
		LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled) override;
		BlinkEraserListener() = default;
		~BlinkEraserListener() = default;
	};
	class CabinetWClassListener : public IWindowListenerCallback
	{
		using BackdropEffectUpdateCallback = std::function<void(HWND hwnd, const SIZE& size)>;
		using BackdropEffectApplyCallback = std::function<void(HWND hwnd, const SIZE& size)>;
		BackdropEffectUpdateCallback m_callback1 = nullptr;
		BackdropEffectApplyCallback m_callback2 = nullptr;
	public:
		LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled) override;
		CabinetWClassListener(BackdropEffectUpdateCallback callback1, BackdropEffectApplyCallback callback2) : m_callback1(callback1), m_callback2(callback2) {}
		~CabinetWClassListener() = default;
	};
	class BlackDesktopChildSiteBridgeListener : public IWindowListenerCallback
	{
	public:
		LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled) override;
		BlackDesktopChildSiteBridgeListener() = default;
		~BlackDesktopChildSiteBridgeListener() = default;
	};
}