////////////////////////////////////////////////////////////////////////////////
//
// Library.h - Библиотека на Visual C++
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "StdAfx.h"

extern KompasObjectPtr kompas;

// Вспомогательные функции
CString LoadStr(int strID);

// Новая функция для создания цилиндра
void CreateCylinder(int flag);
