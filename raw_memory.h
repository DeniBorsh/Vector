#pragma once
template<typename T>
class RawMemory {
public:
	RawMemory() = default;
	explicit RawMemory(size_t capacity);
	RawMemory(const RawMemory& other) = delete;
	RawMemory(RawMemory&& other) noexcept;
	RawMemory& operator=(const RawMemory& other) = delete;
	RawMemory<T>& operator=(RawMemory&& other) noexcept;
	T* operator+(size_t offset);
	T& operator[](size_t offset);
	const T& operator[](size_t offset) const;
	T* GetAddress() noexcept { return buffer_; }
	const T* GetAddress() const noexcept { return buffer_; }
	void Swap(RawMemory& other) noexcept;
	size_t Capacity() const { return capacity_; }
	~RawMemory();
private:
	T* buffer_{ nullptr };
	size_t capacity_{ 0 };

	static T* Allocate(size_t n);
	static void Deallocate(T* buf) noexcept;
};

template<typename T>
RawMemory<T>::RawMemory(size_t capacity) 
	: buffer_{Allocate(capacity)}
	, capacity_{capacity} {
}

template<typename T>
RawMemory<T>::RawMemory(RawMemory&& other) noexcept
	: buffer_{ other.buffer_ }
	, capacity_{ other.capacity_ } 
{
	other.buffer_ = nullptr;
	other.capacity_ - 0;
}

template<typename T>
RawMemory<T>& RawMemory<T>::operator=(RawMemory&& other) noexcept {
	capacity_ = other.capacity_;
	buffer_ = other.buffer_;
	other.buffer_ = nullptr;
	other.capacity_ = 0;
	return *this;
}

template<typename T>
T* RawMemory<T>::operator+(size_t offset) {
	return buffer_ + offset;
}

template<typename T>
T& RawMemory<T>::operator[](size_t offset) {
	return buffer_[offset];
}

template<typename T>
const T& RawMemory<T>::operator[](size_t offset) const {
	return buffer_[offset];
}

template<typename T>
void RawMemory<T>::Swap(RawMemory& other) noexcept {
	std::swap(buffer_, other.buffer_);
	std::swap(capacity_, other.capacity_);
}

template<typename T>
RawMemory<T>::~RawMemory() {
	Deallocate(buffer_);
}

template<typename T>
T* RawMemory<T>::Allocate(size_t n) {
	return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
}

template<typename T>
void RawMemory<T>::Deallocate(T* buf) noexcept {
	operator delete(buf);
}