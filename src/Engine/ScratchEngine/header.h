// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
//#include <Winuser.h>

// reference additional headers your program requires here
#include <d3d11_2.h>
#include <DirectXMath.h>

#include <DDSTextureLoader.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <fstream>
#include <functional>
#include <limits.h>
#include <malloc.h>
#include <map>
#include <mutex>
#include <queue>
#include <stack>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace DirectX;
using namespace std;
