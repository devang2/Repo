// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2012 - TortoiseSVN

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#pragma once

// functions copied from atlbase.h: those functions got removed
// in VS2012...
// removed functions: AtlGetShellVersion, AtlGetDllVersion

namespace ATL
{

inline HRESULT AtlGetDllVersion(
	_In_ HINSTANCE hInstDLL,
	_Out_ DLLVERSIONINFO* pDllVersionInfo)
{
	ATLENSURE(pDllVersionInfo != NULL);

	// We must get this function explicitly because some DLLs don't implement it.
	DLLGETVERSIONPROC pfnDllGetVersion = (DLLGETVERSIONPROC)::GetProcAddress(hInstDLL, "DllGetVersion");

	if(pfnDllGetVersion == NULL)
	{
		return E_NOTIMPL;
	}

	return (*pfnDllGetVersion)(pDllVersionInfo);
}

inline HRESULT AtlGetDllVersion(
	_In_z_ LPCTSTR lpstrDllName,
	_Out_ DLLVERSIONINFO* pDllVersionInfo)
{
	HINSTANCE hInstDLL = ::LoadLibrary(lpstrDllName);
	if(hInstDLL == NULL)
	{
		return AtlHresultFromLastError();
	}
	HRESULT hRet = AtlGetDllVersion(hInstDLL, pDllVersionInfo);
	::FreeLibrary(hInstDLL);
	return hRet;
}

// Common Control Versions:
//   WinNT 4.0          maj=4 min=00
//   IE 3.x             maj=4 min=70
//   IE 4.0             maj=4 min=71
//   IE 5.0             maj=5 min=80
//   Win2000            maj=5 min=81
inline HRESULT AtlGetCommCtrlVersion(
	_Out_ LPDWORD pdwMajor,
	_Out_ LPDWORD pdwMinor)
{
	ATLENSURE(( pdwMajor != NULL ) && ( pdwMinor != NULL ));

	DLLVERSIONINFO dvi;
	memset(&dvi, 0, sizeof(dvi));
	dvi.cbSize = sizeof(dvi);

	HRESULT hRet = AtlGetDllVersion(_T("comctl32.dll"), &dvi);

	if(SUCCEEDED(hRet))
	{
		*pdwMajor = dvi.dwMajorVersion;
		*pdwMinor = dvi.dwMinorVersion;
	}

	return hRet;
}

// Shell Versions:
//   WinNT 4.0                                      maj=4 min=00
//   IE 3.x, IE 4.0 without Web Integrated Desktop  maj=4 min=00
//   IE 4.0 with Web Integrated Desktop             maj=4 min=71
//   IE 4.01 with Web Integrated Desktop            maj=4 min=72
//   Win2000                                        maj=5 min=00
inline HRESULT AtlGetShellVersion(
	_Out_ LPDWORD pdwMajor,
	_Out_ LPDWORD pdwMinor)
{
	ATLENSURE(( pdwMajor != NULL) && ( pdwMinor != NULL ));

	DLLVERSIONINFO dvi;
	memset(&dvi, 0, sizeof(dvi));
	dvi.cbSize = sizeof(dvi);
	HRESULT hRet = AtlGetDllVersion(_T("shell32.dll"), &dvi);

	if(SUCCEEDED(hRet))
	{
		*pdwMajor = dvi.dwMajorVersion;
		*pdwMinor = dvi.dwMinorVersion;
	}

	return hRet;
}

}; // end namespace ATL