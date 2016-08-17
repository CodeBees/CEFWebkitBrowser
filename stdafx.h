// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
#define _CRT_SECURE_NO_DEPRECATE


#include <cef_client.h>
#include <cef_app.h>
#include <capi/cef_app_capi.h>

#include <sstream>
#include <string>

#include <base/cef_bind.h>
#include <wrapper/cef_closure_task.h>
#include <cef_app.h>
#include <cef_base.h>
#include <base/cef_lock.h>
#include <wrapper/cef_helpers.h>


// Windows 头文件:
#include <windows.h>
#include <objbase.h>
#include <Commdlg.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
//
// TODO: 在此处引用程序需要的其他头文件


// TODO: 在此处引用程序需要的其他头文件

#include "DuiLib\UIlib.h"
#include "DuiLib\Ex\ShadowWindow.h"

using namespace DuiLib;
#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "lib\\DuiLib_ud.lib")
#   else
#      pragma comment(lib, "lib\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "lib\\DuiLib_u.lib")
#   else
#       pragma comment(lib, "lib\\DuiLib.lib")
#   endif
#endif


#define  UM_CEF_WEBLOADSTART			WM_USER+100
#define  UM_CEF_WEBLOADEND				WM_USER+101
#define  UM_CEF_WEBLOADPOPUP			WM_USER+102
#define  UM_CEF_COMPLETEELEASE			WM_USER+103
#define  UM_CEF_WEBTITLECHANGE			WM_USER+104




// TODO: 在此处引用程序需要的其他头文件
