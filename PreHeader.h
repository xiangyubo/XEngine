#ifndef __PREHEADER__
#define __PREHEADER__

#pragma once

#include <Windows.h>
#include <Windowsx.h>
#include <d3dcompiler.h>
#include <xnamath.h>
#include <atlstr.h>

#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx11effect.h>

#include <dsound.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "Effects11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

#pragma comment(lib, "winmm.lib")

using std::vector;
using std::string;
using std::wstring;
using std::unordered_map;
using std::shared_ptr;
using std::ifstream;
using std::stringstream;
using std::map;
using std::make_pair;
using std::move;

#endif