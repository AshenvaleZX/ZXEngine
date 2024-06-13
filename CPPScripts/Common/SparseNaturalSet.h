#pragma once
#include "../pubh.h"

namespace ZXEngine
{
	template <typename T, size_t page_size, typename = std::enable_if<std::is_integral_v<T>>>
	class SparseNaturalSet
	{
	public:
		void Add(T value)
		{
			mDense.push_back(value);
			SetSparse(value, mDense.size() - 1);
		}

		void Remove(T value)
		{
			if (!Contain(value))
				return;

			size_t page = GetPage(value);
			size_t offset = GetOffset(value);
			size_t index = mSparse[page]->at(offset);

			SetSparse(mDense.back(), index);
			SetSparse(value, null_index);
			
			mDense[index] = mDense.back();
			mDense.pop_back();
		}

		bool Contain(T value) const
		{
			size_t page = GetPage(value);
			size_t offset = GetOffset(value);

			if (mSparse.size() <= page)
				return false;

			return mSparse[page]->at(offset) != null_index;
		}

		void Clear()
		{
			mDense.clear();
			mSparse.clear();
		}

		auto begin() const
		{
			return mDense.begin();
		}

		auto end() const
		{
			return mDense.end();
		}

	private:
		vector<T> mDense;
		vector<unique_ptr<array<size_t, page_size>>> mSparse;
		static constexpr size_t null_index = std::numeric_limits<size_t>::max();

		inline size_t GetPage(T value) const
		{
			return static_cast<size_t>(value) / page_size;
		}

		inline size_t GetOffset(T value) const
		{
			return static_cast<size_t>(value) % page_size;
		}

		void SetSparse(T value, size_t index)
		{
			size_t page = GetPage(value);
			size_t offset = GetOffset(value);

			if (mSparse.size() <= page)
			{
				for (size_t i = mSparse.size(); i <= page; i++)
				{
					mSparse.emplace_back(std::make_unique<std::array<size_t, page_size>>());
					mSparse.back()->fill(null_index);
				}
			}

			mSparse[page]->at(offset) = index;
		}
	};
}