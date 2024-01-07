#pragma once
#include <cstdlib>
#include <new>
#include <utility>

template<typename T>
class Vector {
public:
	Vector() = default;
	explicit Vector(size_t size);
	Vector(const Vector<T>& other);

	size_t Size() const noexcept { return size_; };
	size_t Capacity() const noexcept { return capacity_; };
	T& operator[](size_t n) { return data_[n]; };
	const T& operator[](size_t n) const { return data_[n]; };
	void Reserve(size_t new_capacity);

	~Vector();
private:
	T* data_{ nullptr };
	size_t size_{ 0 };
	size_t capacity_{ 0 };

	static T* Allocate(size_t n);
	static void Deallocate(T* buf) noexcept;
	void CopyConstruct(T* place, const T& obj);
	static void DestroyN(T* buf, size_t n) noexcept;
	static void Destroy(T* buf) noexcept;
};

template<typename T>
Vector<T>::Vector(size_t size)
	: data_{Allocate(size)}
	, size_{size}
	, capacity_{size}
{
	for (int i = 0; i != size; ++i)
		new(data_ + i) T{};
}

template<typename T>
Vector<T>::Vector(const Vector<T>& other)
	: data_{Allocate(other.size_)}
	, size_{other.size_}
	, capacity_{other.size_}
{
	for (int i = 0; i != other.size_; ++i)
		CopyConstruct(data_ + i, other.data_[i]);
}

template<typename T>
void Vector<T>::Reserve(size_t new_capacity) {
	if (new_capacity <= capacity_) return;

	T* new_data = Allocate(new_capacity);
	for (int i = 0; i != size_; ++i)
		CopyConstruct(new_data + i, data_[i]);

	DestroyN(data_, size_);
	Deallocate(data_);
	data_ = new_data;
	capacity_ = new_capacity;
}

template<typename T>
T* Vector<T>::Allocate(size_t n) {
	return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
}

template<typename T>
void Vector<T>::Deallocate(T* buf) noexcept {
	operator delete(buf);
}

template<typename T>
void Vector<T>::CopyConstruct(T* place, const T& obj) {
	new(place) T{ obj };
}

template<typename T>
void Vector<T>::DestroyN(T* buf, size_t n) noexcept {
	for (size_t i = 0; i != n; ++i) {
		Destroy(buf + i);
	}
}

template<typename T>
void Vector<T>::Destroy(T* buf) noexcept {
	buf->~T();
}

template<typename T>
Vector<T>::~Vector() {
	DestroyN(data_, size_);
	Deallocate(data_);
}
