/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the 
"Apache License"); you may not use this file except in compliance with the 
Apache License. You may obtain a copy of the Apache License at 
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2024 Audiokinetic Inc.
*******************************************************************************/

// AkPlatformFuncs.h

/// \file 
/// Platform-dependent functions definition.

#ifndef _AK_TOOLS_COMMON_AKPLATFORMFUNCS_H
#define _AK_TOOLS_COMMON_AKPLATFORMFUNCS_H

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/SoundEngine/Common/AkAtomic.h>

// Uncomment the following to enable built-in platform profiler markers in the sound engine
//#define AK_ENABLE_INSTRUMENT

#if defined(AK_WIN)
#include <AK/Tools/Win32/AkPlatformFuncs.h>

#elif defined (AK_XBOX)
#include <AK/Tools/XboxGC/AkPlatformFuncs.h>

#elif defined (AK_APPLE)
#include <AK/Tools/Mac/AkPlatformFuncs.h>
#include <AK/Tools/POSIX/AkPlatformFuncs.h>

#elif defined( AK_ANDROID ) || defined ( AK_LINUX_AOSP ) 
#include <AK/Tools/Android/AkPlatformFuncs.h>

#elif defined (AK_PS4)
#include <AK/Tools/PS4/AkPlatformFuncs.h>

#elif defined (AK_PS5)
#include <AK/Tools/PS5/AkPlatformFuncs.h>

#elif defined (AK_EMSCRIPTEN)
#include <AK/Tools/Emscripten/AkPlatformFuncs.h>

#elif defined (AK_LINUX)
#include <AK/Tools/Linux/AkPlatformFuncs.h>
#include <AK/Tools/POSIX/AkPlatformFuncs.h>

#elif defined (AK_QNX)
#include <AK/Tools/QNX/AkPlatformFuncs.h>
#include <AK/Tools/POSIX/AkPlatformFuncs.h>

#elif defined (AK_NX)
#include <AK/Tools/NX/AkPlatformFuncs.h>

#else
#error AkPlatformFuncs.h: Undefined platform
#endif

#ifndef AkPrefetchZero
#define AkPrefetchZero(___Dest, ___Size)
#endif

#ifndef AkPrefetchZeroAligned
#define AkPrefetchZeroAligned(___Dest, ___Size)
#endif

#ifndef AkZeroMemAligned
#define AkZeroMemAligned(___Dest, ___Size) AKPLATFORM::AkMemSet(___Dest, 0, ___Size);
#endif
#ifndef AkZeroMemLarge
#define AkZeroMemLarge(___Dest, ___Size) AKPLATFORM::AkMemSet(___Dest, 0, ___Size);
#endif
#ifndef AkZeroMemSmall
#define AkZeroMemSmall(___Dest, ___Size) AKPLATFORM::AkMemSet(___Dest, 0, ___Size);
#endif

#ifndef AkAllocaSIMD
#ifdef __clang__
#if __has_builtin( __builtin_alloca_with_align )
#define AkAllocaSIMD( _size_ ) __builtin_alloca_with_align( _size_, 128 )
#else
// work around alloca alignment issues in versions of clang before 4.0
#define AkAllocaSIMD( _size_ ) (void*)( ( ( uintptr_t )AkAlloca( _size_ + 16 ) + 0xF ) & ~0xF )
#endif
#else
#define AkAllocaSIMD( _size_ ) AkAlloca( _size_ )
#endif
#endif

#ifndef AK_THREAD_INIT_CODE
#define AK_THREAD_INIT_CODE(_threadProperties)
#endif

/// Platform-dependent helpers
namespace AKPLATFORM
{
	inline void AkGetDefaultHighPriorityThreadProperties(AkThreadProperties& out_threadProperties)
	{
		AkGetDefaultThreadProperties(out_threadProperties);
		out_threadProperties.nPriority = AK_THREAD_PRIORITY_ABOVE_NORMAL;
	}

	// fallback implementations for when platform don't have their own implementation
#if !defined(AK_BIT_SCAN_INSTRUCTIONS)

	// AkPopCount returns how many set bits there are in the provided value
#if __clang__ || defined __GNUG__
	AkForceInline AkUInt32 AkPopCount(AkUInt32 in_bits)
	{
		return __builtin_popcount(in_bits);
	}
#else
	AkForceInline AkUInt32 AkPopCount(AkUInt32 in_bits)
	{
		AkUInt32 num = 0;
		while (in_bits) { ++num; in_bits &= in_bits - 1; }
		return num;
	}
#endif


	// AkBitScanForward returns how many 0s there are until the least-significant-bit is set, or the length of the param if the value is zero
#if defined _MSC_VER && (defined AK_CPU_X86_64 || defined AK_CPU_ARM_64)
	AkForceInline AkUInt32 AkBitScanForward64(AkUInt64 in_bits)
	{
		unsigned long ret = 0;
		_BitScanForward64(&ret, in_bits);
		return in_bits ? ret : 64;
	}
#elif __clang__ || defined __GNUG__
	AkForceInline AkUInt32 AkBitScanForward64(AkUInt64 in_bits)
	{
		return __builtin_ctzll(in_bits);
	}
#else
	AkForceInline AkUInt32 AkBitScanForward64(AkUInt64 in_bits)
	{
		if (in_bits == 0) return 64;
		AkUInt32 ret = 0;
		if ((in_bits & 0x00000000FFFFFFFFULL) == 0) { ret += 32; in_bits >>= 32; }
		if ((in_bits & 0x000000000000FFFFULL) == 0) { ret += 16; in_bits >>= 16; }
		if ((in_bits & 0x00000000000000FFULL) == 0) { ret += 8;  in_bits >>= 8; }
		if ((in_bits & 0x000000000000000FULL) == 0) { ret += 4;  in_bits >>= 4; }
		if ((in_bits & 0x0000000000000003ULL) == 0) { ret += 2;  in_bits >>= 2; }
		if ((in_bits & 0x0000000000000001ULL) == 0) { ret += 1;  in_bits >>= 1; }
		return ret;
	}
#endif

#if defined _MSC_VER
	AkForceInline AkUInt32 AkBitScanForward(AkUInt32 in_bits)
	{
		unsigned long ret = 0;
		_BitScanForward(&ret, in_bits);
		return in_bits ? ret : 32;
	}

#elif __clang__ || defined __GNUG__
	AkForceInline AkUInt32 AkBitScanForward(AkUInt32 in_bits)
	{
		return __builtin_ctzl(in_bits);
	}
#else
	AkForceInline AkUInt32 AkBitScanForward(AkUInt32 in_bits)
	{
		if (in_bits == 0) return 32;
		AkUInt32 ret = 0;
		if ((in_bits & 0x0000FFFFULL) == 0) { ret += 16; in_bits >>= 16; }
		if ((in_bits & 0x000000FFULL) == 0) { ret += 8;  in_bits >>= 8; }
		if ((in_bits & 0x0000000FULL) == 0) { ret += 4;  in_bits >>= 4; }
		if ((in_bits & 0x00000003ULL) == 0) { ret += 2;  in_bits >>= 2; }
		if ((in_bits & 0x00000001ULL) == 0) { ret += 1;  in_bits >>= 1; }
		return ret;
	}
#endif

	// AkBitScanReverse returns how many 0s there are after the most-significant-bit is set, or the length of the param if the value is zero
#if defined _MSC_VER && (defined AK_CPU_X86_64 || defined AK_CPU_ARM_64)
	AkForceInline AkUInt32 AkBitScanReverse64(AkUInt64 in_bits)
	{
		unsigned long ret = 0;
		_BitScanReverse64(&ret, in_bits);
		return in_bits ? 63 - ret : 64;
	}
#elif __clang__ || defined __GNUG__
	AkForceInline AkUInt32 AkBitScanReverse64(AkUInt64 in_bits)
	{
		return __builtin_clzll(in_bits);
	}
#else
	AkForceInline AkUInt32 AkBitScanReverse64(AkUInt64 in_bits)
	{
		if (in_bits == 0) return 64;
		int ret = 0;
		if ((in_bits & 0xFFFFFFFF00000000ULL) == 0) { ret += 32; in_bits <<= 32; }
		if ((in_bits & 0xFFFF000000000000ULL) == 0) { ret += 16; in_bits <<= 16; }
		if ((in_bits & 0xFF00000000000000ULL) == 0) { ret += 8;  in_bits <<= 8; }
		if ((in_bits & 0xF000000000000000ULL) == 0) { ret += 4;  in_bits <<= 4; }
		if ((in_bits & 0xC000000000000000ULL) == 0) { ret += 2;  in_bits <<= 2; }
		if ((in_bits & 0x8000000000000000ULL) == 0) { ret += 1;  in_bits <<= 1; }
		return ret;
	}
#endif

#if defined _MSC_VER
	AkForceInline AkUInt32 AkBitScanReverse(AkUInt32 in_bits)
	{
		unsigned long ret = 0;
		_BitScanReverse(&ret, in_bits);
		return in_bits ? 31 - ret : 32;
	}
#elif __clang__ || defined __GNUG__
	AkForceInline AkUInt32 AkBitScanReverse(AkUInt32 in_bits)
	{
		return __builtin_clzl(in_bits);
	}
#else
	AkForceInline AkUInt32 AkBitScanReverse(AkUInt32 in_bits)
	{
		if (in_bits == 0) return 32;
		int ret = 0;
		if ((in_bits & 0xFFFF0000ULL) == 0) { ret += 16; in_bits <<= 16; }
		if ((in_bits & 0xFF000000ULL) == 0) { ret += 8;  in_bits <<= 8; }
		if ((in_bits & 0xF0000000ULL) == 0) { ret += 4;  in_bits <<= 4; }
		if ((in_bits & 0xC0000000ULL) == 0) { ret += 2;  in_bits <<= 2; }
		if ((in_bits & 0x80000000ULL) == 0) { ret += 1;  in_bits <<= 1; }
		return ret;
	}
#endif

#endif // !defined(AK_BIT_SCAN_INSTRUCTIONS)

	// fallback implementations for when platform don't have their own implementation
#if !defined(AK_LIMITEDSPINFORZERO)
	// Waits for a limited amount of time for in_pVal to hit zero (without yielding the thread)
	inline void AkLimitedSpinForZero(AkAtomic32* in_pVal)
	{
		AkInt64 endSpinTime = 0;
		AkInt64 currentTime = 0;
		PerformanceCounter(&endSpinTime);
		endSpinTime += AkInt64(AK::g_fFreqRatio * 0.01); // only spin for about 10us
		while (true)
		{
			// if pval is zero, skip out
			if (AkAtomicLoad32(in_pVal) == 0)
			{
				break;
			}
			AkSpinHint();

			// Check if we've hit the deadline for the timeout
			PerformanceCounter(&currentTime);
			if (currentTime > endSpinTime)
			{
				break;
			}
		}
	}

	// Waits for a limited amount of time for in_pVal to get atomically shift from the expected value to the proposed one
	// returns true if acquisition succeeded
	inline bool AkLimitedSpinToAcquire(AkAtomic32* in_pVal, AkInt32 in_proposed, AkInt32 in_expected)
	{
		if (AkAtomicCas32(in_pVal, in_proposed, in_expected))
		{
			return true;
		}

		// Cas failed, start the slower evaluation
		AkInt64 endSpinTime = 0;
		AkInt64 currentTime = 0;
		PerformanceCounter(&endSpinTime);
		endSpinTime += AkInt64(AK::g_fFreqRatio * 0.01); // only spin for about 10us
		while (true)
		{
			// attempt cas to acquire and if successful, skip out
			if (AkAtomicCas32(in_pVal, in_proposed, in_expected))
			{
				return true;
			}
			AkSpinHint();

			// Check if we've hit the deadline for the timeout
			PerformanceCounter(&currentTime);
			if (currentTime > endSpinTime)
			{
				return false;
			}
		}
	}
#endif // !defined(AK_LIMITEDSPINFORZERO)

	inline void AkSpinWaitForZero(AkAtomic32* in_pVal)
	{
		if (AkAtomicLoad32(in_pVal) == 0)
		{
			return;
		}

		// do a limited spin on-the-spot until in_pVal hits zero
		AkLimitedSpinForZero(in_pVal);

		// if in_pVal is still non-zero, then the other thread is either blocked or waiting for us.  Yield for real.
		while (AkAtomicLoad32(in_pVal))
			AkThreadYield();
	}

	// Waits for a limited amount of time for in_pVal to get atomically shift from 0 to 1
	inline void AkSpinToAcquire(AkAtomic32* in_pVal, AkInt32 in_proposed, AkInt32 in_expected)
	{
		// do a limited spin on-the-spot until in_pVal can successfully hit 1
		// or if it fails, then the other thread is either blocked or waiting for us.  Yield for real.
		while (!AkLimitedSpinToAcquire(in_pVal, in_proposed, in_expected))
		{
			AkThreadYield();
		}
	}
}


/// Utility functions
namespace AK
{
	/// Count non-zero bits, e.g. to count # of channels in a channelmask
	/// \return Number of channels.
	AkForceInline AkUInt32 GetNumNonZeroBits(AkUInt32 in_uWord)
	{
		return AKPLATFORM::AkPopCount(in_uWord);
	}

	/// Computes the next power of two given a value.
	/// \return next power of two.
	AkForceInline AkUInt32 GetNextPowerOfTwo(AkUInt32 in_uValue)
	{
		in_uValue--;
		in_uValue |= in_uValue >> 1;
		in_uValue |= in_uValue >> 2;
		in_uValue |= in_uValue >> 4;
		in_uValue |= in_uValue >> 8;
		in_uValue |= in_uValue >> 16;
		in_uValue++;
		return in_uValue;
	}

	AkForceInline AkUInt32 ROTL32(AkUInt32 x, AkUInt32 r)
	{
		return (x << r) | (x >> (32 - r));
	}

	AkForceInline AkUInt64 ROTL64(AkUInt64 x, AkUInt64 r)
	{
		return (x << r) | (x >> (64 - r));
	}
}


#ifndef AK_PERF_RECORDING_RESET
#define AK_PERF_RECORDING_RESET()
#endif
#ifndef AK_PERF_RECORDING_START
#define AK_PERF_RECORDING_START( __StorageName__, __uExecutionCountStart__, __uExecutionCountStop__ )
#endif
#ifndef AK_PERF_RECORDING_STOP
#define AK_PERF_RECORDING_STOP( __StorageName__, __uExecutionCountStart__, __uExecutionCountStop__ )	
#endif

#endif // _AK_TOOLS_COMMON_AKPLATFORMFUNCS_H
