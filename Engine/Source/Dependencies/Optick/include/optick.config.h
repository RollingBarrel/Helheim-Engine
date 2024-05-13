// The MIT License(MIT)
//
// Copyright(c) 2019 Vadim Slyusarev
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL SETTINGS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [x] USE_OPTICK					- (Master Switch)
// [x] OPTICK_ENABLE_TRACING		- (Enable Kernel-level tracing)
// [x] OPTICK_ENABLE_GPU_D3D12		- (GPU D3D12)
// [x] OPTICK_ENABLE_GPU_VULKAN		- (GPU VULKAN)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MASTER SWITCH - use it for disabling profiler in final builds															  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_DEBUG)
	#if !defined(USE_OPTICK)
	#define USE_OPTICK (1)
	#endif
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enable Low-level platform-specific tracing (Switch Contexts, Autosampling, etc.)											  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(OPTICK_ENABLE_TRACING)
#define OPTICK_ENABLE_TRACING (USE_OPTICK /*&& 0*/)
#endif //OPTICK_ENABLE_TRACING
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// GPU Counters																										  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if !defined(OPTICK_ENABLE_GPU)
#define OPTICK_ENABLE_GPU (USE_OPTICK /*&& 0*/)
#endif //OPTICK_ENABLE_GPU

// D3D12
#if !defined(OPTICK_ENABLE_GPU_D3D12)
#if defined(_MSC_VER)
#define OPTICK_ENABLE_GPU_D3D12 (OPTICK_ENABLE_GPU /*&& 0*/)
#else
#define OPTICK_ENABLE_GPU_D3D12 (0)
#endif
#endif

// VULKAN
#if !defined(OPTICK_ENABLE_GPU_VULKAN)
#if defined(_MSC_VER)
#define OPTICK_ENABLE_GPU_VULKAN (OPTICK_ENABLE_GPU /*&& 0*/)
#else
#define OPTICK_ENABLE_GPU_VULKAN (0)
#endif
#endif

