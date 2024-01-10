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
	Vector(Vector<T>&& other) noexcept;
	Vector<T>& operator=(const Vector<T>& other);
	Vector<T>& operator=(Vector<T>&& rhs) noexcept;

	void PushBack(const T& value);
	void PushBack(T&& value);
	void PopBack() noexcept;
	size_t Size() const noexcept { return size_; };
	size_t Capacity() const noexcept { return data_.Capacity(); };
	T& operator[](size_t n) { return data_[n]; };
	const T& operator[](size_t n) const { return data_[n]; };
	void Reserve(size_t new_capacity);
	void Resize(size_t new_size);

	~Vector();
private:
	RawMemory<T> data_{};
	size_t size_{0};

	void Swap(Vector<T>& other) noexcept;
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
Vector<T>::Vector(Vector<T>&& other) noexcept
	: data_{std::move(other.data_)}
	, size_{other.size_} 
{
	other.size_ = 0;
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& rhs) {
	if (this != &rhs) {
		if (data_.Capacity() < rhs.size_) {
			Vector<T> tmp{ rhs };
			this->Swap(tmp);
		}
		else {
			if (size_ <= rhs.size_) {
				std::copy_n(rhs.data_.GetAddress(), size_, data_.GetAddress());
				std::uninitialized_copy_n(rhs.data_.GetAddress() + size_, rhs.size_ - size_, data_ + size_);
			}
			else {
				std::copy_n(rhs.data_.GetAddress(), rhs.size_, data_.GetAddress());
				std::destroy_n(data_ + rhs.size_, size_ - rhs.size_);
			}
			size_ = rhs.size_;
		}
	}
	return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& rhs) noexcept {
	if (this != &rhs) {
		data_ = std::move(rhs.data_);
		size_ = rhs.size_;
		rhs.size_ = 0;
	}
	return *this;
}

template<typename T>
void Vector<T>::PushBack(const T& value) {
	if (size_ == data_.Capacity()) {
		RawMemory<T> new_data{ size_ == 0 ? 1 : size_ * 2 };
		new (new_data + size_) T{ value };

		if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
			std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
		else
			std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());

		std::destroy_n(data_.GetAddress(), size_);
		data_.Swap(new_data);
	}
	else {
		new (data_ + size_) T{ value };
	}
	++size_;
}

template<typename T>
void Vector<T>::PushBack(T&& value) {
	if (size_ == data_.Capacity()) {
		RawMemory<T> new_data{ size_ == 0 ? 1 : size_ * 2 };
		new (new_data + size_) T{ std::move(value) };

		if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
			std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
		else
			std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());

		std::destroy_n(data_.GetAddress(), size_);
		data_.Swap(new_data);
	}
	else {
		new (data_ + size_) T{ std::move(value) };
	}
	++size_;
}

template<typename T>
void Vector<T>::PopBack() noexcept {
	if (size_ > 0) {
		data_[size_ - 1].~T();
		--size_;
	}
}

template<typename T>
void Vector<T>::Reserve(size_t new_capacity) {
	if (new_capacity <= data_.Capacity()) return;

	RawMemory<T> new_data{new_capacity};

	if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
		std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
	else
		std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
	std::destroy_n(data_.GetAddress(), size_);
	data_.Swap(new_data);
}

template<typename T>
void Vector<T>::Resize(size_t new_size) {
	if (new_size < size_)
		std::destroy_n(data_ + new_size, size_ - new_size);
	else if (data_.Capacity() < new_size)
		Reserve(std::max(new_size, size_ * 2));
	std::uninitialized_value_construct_n(data_ + size_, new_size > size_ ? new_size - size_ : 0);
	size_ = new_size;
}

template<typename T>
Vector<T>::~Vector() {
	std::destroy_n(data_.GetAddress(), size_);
}

template<typename T>
void Vector<T>::Swap(Vector<T>& other) noexcept {
	data_.Swap(other.data_);
	std::swap(size_, other.size_);
}
