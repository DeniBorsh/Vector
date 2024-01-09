#pragma once
#include <cstdlib>
#include <new>
#include <utility>
#include <exception>
#include <memory>
#include "raw_memory.h"

template<typename T>
class Vector {
public:
	Vector() = default;
	explicit Vector(size_t size);
	Vector(const Vector<T>& other);

	size_t Size() const noexcept { return size_; };
	size_t Capacity() const noexcept { return data_.Capacity(); };
	T& operator[](size_t n) { return data_[n]; };
	const T& operator[](size_t n) const { return data_[n]; };
	void Reserve(size_t new_capacity);

	~Vector();
private:
	RawMemory<T> data_{};
	size_t size_{0};

	void CopyConstruct(T* place, const T& obj);
};

template<typename T>
Vector<T>::Vector(size_t size)
	: data_{size}
	, size_{size}
{
	std::uninitialized_value_construct_n(data_.GetAddress(), size);
}

template<typename T>
Vector<T>::Vector(const Vector<T>& other)
	: data_{other.size_}
	, size_{other.size_}
{
	std::uninitialized_copy_n(other.data_.GetAddress(), other.size_, data_.GetAddress());
}

template<typename T>
void Vector<T>::Reserve(size_t new_capacity) {
	if (new_capacity <= data_.Capacity()) return;

	RawMemory<T> new_data{new_capacity};

	if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
		std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
	}
	else {
		std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
	}
	std::destroy_n(data_.GetAddress(), size_);
	data_.Swap(new_data);
}

template<typename T>
void Vector<T>::CopyConstruct(T* place, const T& obj) {
	new(place) T{ obj };
}

template<typename T>
Vector<T>::~Vector() {
	std::destroy_n(data_.GetAddress(), size_);
}
