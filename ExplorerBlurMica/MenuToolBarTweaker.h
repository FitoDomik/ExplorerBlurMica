#pragma once
#include "WindowListener.h"
namespace MBox
{
	class BlackListener : public IWindowListenerCallback
	{
	public:
		LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool& handled) override;
		BlackListener() = default;
		~BlackListener() = default;
	};
}