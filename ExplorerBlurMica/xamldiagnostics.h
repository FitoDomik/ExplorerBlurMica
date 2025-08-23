#pragma once
#include "tapsite.h"
namespace MBox
{
	HRESULT InitializeUWPXamlDiagnostics();
	HRESULT InitializeWinUIXamlDiagnostics();
	void EnableXamlDiagnostics(bool uwp);
	void FreeXamlDiagnostics();
	void RunDiagnostics(bool uwp);
	bool ProcessIsWinUI();
}