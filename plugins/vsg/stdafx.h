#pragma once

#ifdef _WIN32
// as precaution include the windows.h to avoid including it later
// and these damned min/max macros cause serious trouble in various
// files.
#define WIN32_LEAN_AND_MEAN
// but include winsock2 first, to avoid include of winsock.h which
// causes further trouble for someone who wants to use Winsock V2.
#include <winsock2.h>
#include <Windows.h>

// get rid of the damn WinApi macros min/max
#ifdef min
#undef min
#endif // min
#ifdef max
#undef max
#endif // max
#else // _WIN32
#include <math.h>
#include <float.h>
#endif // _WIN32

#if defined(_DEBUG)
#if defined(_MSC_VER)
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else // defined(_MSC_VER)
#define DEBUG_NEW new
#endif // defined(_MSC_VER)
#endif // defined(_DEBUG)

// commonly used headers from STL
#include <map>
#include <list>

#include <osg/Referenced>
#include <osg/ref_ptr>
