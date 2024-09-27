#pragma once
#include "../pubh.h"
#include "Type/BaseType.h"
#include "Factory.h"

namespace ZXEngine
{
	namespace Reflection
	{
		class Variable
		{
		public:
			enum class StorageType
			{
				Empty,
				Copy,
				Reference,
				ConstReference
			};

			struct Operation
			{
				Variable(*Copy)(const Variable&) = nullptr;
				Variable(*Move)(Variable&&) = nullptr;
				void(*Destroy)(Variable&) = nullptr;
			};

			template <typename T>
			struct OperationTraits
			{
				static Variable Copy(const Variable& var)
				{
					assert(var.mType == GetTypeInfo<T>());

					Variable result;
					result.mData = new T(*static_cast<T*>(var.mData));
					result.mType = var.mType;
					result.mStorageType = StorageType::Copy;
					result.mOperation = var.mOperation;
					return result;
				}

				static Variable Move(Variable&& var)
				{
					assert(var.mType == GetTypeInfo<T>());

					Variable result;
					result.mData = new T(std::move(*static_cast<T*>(var.mData)));
					result.mType = var.mType;
					result.mStorageType = StorageType::Copy;
					result.mOperation = var.mOperation;
					var.mData = nullptr;
					var.mType = nullptr;
					var.mStorageType = StorageType::Empty;
					return result;
				}

				static void Destroy(Variable& var)
				{
					assert(var.mType == GetTypeInfo<T>());

					delete static_cast<T*>(var.mData);
					var.mData = nullptr;
					var.mType = nullptr;
					var.mStorageType = StorageType::Empty;
				}
			};

			template <typename T>
			static Variable Copy(const T& var)
			{
				Variable result;
				result.mData = new T(var);
				result.mType = GetTypeInfo<T>();
				result.mStorageType = StorageType::Copy;

				if constexpr (std::is_copy_constructible_v<T>)
					result.mOperation.Copy = &OperationTraits<T>::Copy;
				if constexpr (std::is_move_constructible_v<T>)
					result.mOperation.Move = &OperationTraits<T>::Move;
				if constexpr (std::is_destructible_v<T>)
					result.mOperation.Destroy = &OperationTraits<T>::Destroy;

				return result;
			}

			template <typename T>
			static Variable Move(T&& var)
			{
				Variable result;
				result.mData = new T(std::move(var));
				result.mType = GetTypeInfo<T>();
				result.mStorageType = StorageType::Copy;

				if constexpr (std::is_copy_constructible_v<T>)
					result.mOperation.Copy = &OperationTraits<T>::Copy;
				if constexpr (std::is_move_constructible_v<T>)
					result.mOperation.Move = &OperationTraits<T>::Move;
				if constexpr (std::is_destructible_v<T>)
					result.mOperation.Destroy = &OperationTraits<T>::Destroy;

				return result;
			}

			template <typename T>
			static Variable Reference(T& var)
			{
				Variable result;
				result.mData = &var;
				result.mType = GetTypeInfo<T>();
				result.mStorageType = StorageType::Reference;

				if constexpr (std::is_copy_constructible_v<T>)
					result.mOperation.Copy = &OperationTraits<T>::Copy;
				if constexpr (std::is_move_constructible_v<T>)
					result.mOperation.Move = &OperationTraits<T>::Move;
				if constexpr (std::is_destructible_v<T>)
					result.mOperation.Destroy = &OperationTraits<T>::Destroy;

				return result;
			}

			template <typename T>
			static Variable ConstReference(const T& var)
			{
				Variable result;
				result.mData = &var;
				result.mType = GetTypeInfo<T>();
				result.mStorageType = StorageType::ConstReference;

				if constexpr (std::is_copy_constructible_v<T>)
					result.mOperation.Copy = &OperationTraits<T>::Copy;
				if constexpr (std::is_move_constructible_v<T>)
					result.mOperation.Move = &OperationTraits<T>::Move;
				if constexpr (std::is_destructible_v<T>)
					result.mOperation.Destroy = &OperationTraits<T>::Destroy;

				return result;
			}

			template <typename T>
			T* TryCast()
			{
				if (mType == GetTypeInfo<T>())
					return static_cast<T*>(mData);
				else
					return nullptr;
			}

			Variable() = default;

			Variable(const Variable& var) : mType(var.mType), mStorageType(var.mStorageType), mOperation(var.mOperation)
			{
				if (mOperation.Copy)
				{
					auto newVar = mOperation.Copy(var);
					mData = newVar.mData;
					newVar.mData = nullptr;
				}
				else
				{
					mType = nullptr;
					mStorageType = StorageType::Empty;
				}
			}

			Variable(Variable&& var) noexcept :
				mData(std::move(var.mData)),
				mType(var.mType), 
				mStorageType(var.mStorageType), 
				mOperation(std::move(var.mOperation))
			{}

			~Variable()
			{
				if (mOperation.Destroy && mStorageType == StorageType::Copy)
					mOperation.Destroy(*this);
			}

			Variable& operator=(const Variable& var)
			{
				if (this != &var)
				{
					if (mOperation.Destroy && mStorageType == StorageType::Copy)
						mOperation.Destroy(*this);

					mType = var.mType;
					mStorageType = var.mStorageType;
					mOperation = var.mOperation;

					if (mOperation.Copy)
					{
						auto newVar = mOperation.Copy(var);
						mData = newVar.mData;
						newVar.mData = nullptr;
					}
					else
					{
						mType = nullptr;
						mStorageType = StorageType::Empty;
					}
				}

				return *this;
			}

			Variable& operator=(Variable&& var) noexcept
			{
				if (this != &var)
				{
					if (mOperation.Destroy && mStorageType == StorageType::Copy)
						mOperation.Destroy(*this);

					mData = std::move(var.mData);
					mType = var.mType;
					mStorageType = var.mStorageType;
					mOperation = std::move(var.mOperation);
				}

				return *this;
			}

		private:
			void* mData = nullptr;
			const BaseType* mType = nullptr;
			StorageType mStorageType = StorageType::Empty;
			Operation mOperation;
		};
	}
}