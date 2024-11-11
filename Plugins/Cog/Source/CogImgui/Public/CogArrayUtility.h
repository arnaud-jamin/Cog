#pragma once

#include <array>
#include <iterator>
#include <type_traits>

// Utilities to work with one-dimensional, statically bound arrays. Code relying on these utilities should work
// without modifications with fixed-size arrays (currently used in ImGui) and with standard arrays

namespace Utilities
{
	//===============================================================
	// Range
	//===============================================================

	template<typename T>
	class TRange
	{
	public:
		TRange() {}
		TRange(const T& RangeBegin, const T& RangeEnd) { SetRange(RangeBegin, RangeEnd); }

		const T& GetBegin() const { return Begin; }
		const T& GetEnd() const { return End; }

		bool IsEmpty() const { return Begin == End; }

		void SetEmpty() { Begin = End = T(); }

		void SetRange(const T& RangeBegin, const T& RangeEnd)
		{
			checkf(RangeBegin <= RangeEnd, TEXT("Invalid arguments: RangeBegin > RangeEnd"));
			Begin = RangeBegin;
			End = RangeEnd;
		}

		void AddPosition(const T& Position)
		{
			AddRangeUnchecked(Position, Position + 1);
		}

		void AddRange(const T& RangeBegin, const T& RangeEnd)
		{
			checkf(RangeBegin <= RangeEnd, TEXT("Invalid arguments: RangeBegin > RangeEnd"));
			AddRangeUnchecked(RangeBegin, RangeEnd);
		}

	private:

		void AddRangeUnchecked(const T& RangeBegin, const T& RangeEnd)
		{
			if (IsEmpty())
			{
				Begin = RangeBegin;
				End = RangeEnd;
			}
			else
			{
				if (Begin > RangeBegin)
				{
					Begin = RangeBegin;
				}

				if (RangeEnd > End)
				{
					End = RangeEnd;
				}
			}
		}
		
		T Begin = T();
		T End = T();
	};

	// Enable range-based loops

	template<typename T>
	const T& begin(const TRange<T>& Range)
	{
		return Range.GetBegin();
	}

	template<typename T>
	const T& end(const TRange<T>& Range)
	{
		return Range.GetEnd();
	}

	//===============================================================
	// Bounded Range
	//===============================================================

	template<typename T, T BeginBound, T EndBound>
	class TBoundedRange
	{
	public:

		constexpr const T& GetLowerBound() const { return BeginBound; }
		constexpr const T& GetUpperBound() const { return EndBound; }

	public:
		
		const T& GetBegin() const { return Begin; }
		const T& GetEnd() const { return End; }

		bool IsEmpty() const { return Begin == End; }

		void SetEmpty() { Begin = End = T(); }

		void SetFull()
		{
			Begin = BeginBound;
			End = EndBound;
		}

		void AddPosition(const T& Position)
		{
			checkf(Position >= BeginBound && Position < EndBound, TEXT("Position out of range."));
			
			AddRangeUnchecked(Position, Position + 1);
		}

		void AddRange(const T& RangeBegin, const T& RangeEnd)
		{
			checkf(RangeBegin <= RangeEnd, TEXT("Invalid arguments: RangeBegin > RangeEnd"));
			checkf(RangeBegin >= BeginBound, TEXT("RangeBegin out of range"));
			checkf(RangeBegin <= EndBound, TEXT("RangeEnd out of range"));
			
			AddRangeUnchecked(RangeBegin, RangeEnd);
		}

	private:

		void AddRangeUnchecked(const T& RangeBegin, const T& RangeEnd)
		{
			if (IsEmpty())
			{
				Begin = RangeBegin;
				End = RangeEnd;
			}
			else
			{
				if (Begin > RangeBegin)
				{
					Begin = RangeBegin;
				}

				if (RangeEnd > End)
				{
					End = RangeEnd;
				}
			}
		}
		
		T Begin = T();
		T End = T();
	};

	// Enable range-based loops

	template<typename T, T BeginBound, T EndBound>
	const T& begin(const TBoundedRange<T, BeginBound, EndBound>& Range)
	{
		return Range.GetBegin();
	}

	template<typename T, T BeginBound, T EndBound>
	const T& end(const TBoundedRange<T, BeginBound, EndBound>& Range)
	{
		return Range.GetEnd();
	}
	
	//===============================================================
	// Bounded Range
	//===============================================================

	// Function to determine number of elements in fixed size array.
	template<class T, std::size_t N>
	constexpr std::size_t GetArraySize(const T(&)[N])
	{
		return N;
	}

	// Function to determine number of elements in std array.
	template<class T, std::size_t N>
	constexpr std::size_t GetArraySize(const std::array<T, N>&)
	{
		return N;
	}
	
	//===============================================================
	// Traits
	//===============================================================

	template<typename TArray>
	struct ArraySize;

	// Struct to determine number of elements in fixed size array
	template<typename T, std::size_t N>
	struct ArraySize<T[N]> : std::extent<T[N]>
	{};

	// Struct to determine number of elements in std array
	template<typename T, std::size_t N>
	struct ArraySize<std::array<T,N>> : std::tuple_size<std::array<T,N>>
	{};

	//===============================================================
	// Ranges
	//===============================================================

	// Array indices range. Limited by 0 and array size.
	template<typename TArray, typename SizeType>
	using TArrayIndexRange = TBoundedRange<SizeType, 0, ArraySize<TArray>::value>;
}