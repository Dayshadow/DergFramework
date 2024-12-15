#pragma once

#include <vector>
#include "Framework/Log.hpp"
#include <algorithm>
template<class T>
class Array2D {
public:
	Array2D() { width = 0; height = 0; data = std::vector<T>(); }
	Array2D(uint32_t p_width, uint32_t p_height) {
		data.resize(p_width * p_height, T());

		width = p_width;
		height = p_height;
	}
	Array2D(const Array2D<T>& other) {
		data = other.data;
		width = other.width;
		height = other.height;
	}
	Array2D(Array2D<T>&& other) noexcept {
		data = std::move(other.data);
		width = other.width;
		height = other.height;
	}
	Array2D<T> operator=(const Array2D<T>& other) {
		data = other.data;
		width = other.width;
		height = other.height;
		return *this;
	}
	
	T& operator()(int x, int y) {
#ifdef SBBB_DEBUG
		if (!bounded(x, y)) {
			throw std::out_of_range("2D Array index out of bounds.");
		}
#endif
		return data[((y * width) + x)];
	}

	T operator()(int x, int y) const {
#ifdef SBBB_DEBUG
		if (!bounded(x, y)) {
			throw std::out_of_range("2D Array index out of bounds.");
		}
#endif
		return data[((y * width) + x)];
	}

	void fill(T p_fillValue) {
		for (size_t i = 0; i < width * height; i++) {
			data[i] = p_fillValue;
		}
	}
	std::vector<T>& getData() {
		return data;
	}

	// a somewhat unsafe operation
	// scratch that, super unsafe lol
	void setData(T* p_data) {
		data.assign(p_data, p_data + width * height);
	}

	bool bounded(int x, int y) const {
		if (x >= 0 && x < width && y >= 0 && y < height) return true;
		return false;
	}
	void resize(uint32_t p_width, uint32_t p_height) {
		data.resize(p_width * p_height, T());

		width = p_width;
		height = p_height;
	}

	void append(T* p_data, size_t p_size) {
		if (p_size == 0) return;
		if (p_size % width != 0) {
			ERROR_LOG("Tried to append data of invalid size to existing 2d array. Make sure the width of what you're appending matches the destination.");
			return;
		}
		data.reserve(width * height);
		std::copy(&p_data[0], &p_data[p_size], std::back_inserter(data));
		height += p_size / width;
	}
	void prepend(T* p_data, size_t p_size) {
		if (p_size == 0) return;
		if (p_size % width != 0) {
			ERROR_LOG("Tried to prepend data of invalid size to existing 2d array. Make sure the width of what you're prepending matches the destination.");
			return;
		}
		data.reserve(width * height);
		data.insert(data.begin(), &p_data[0], &p_data[p_size]);
		height += p_size / width;
	}

	void reverse() {
		std::reverse(data.begin(), data.end());
	}

	void reserve(size_t p_amt) {
		data.reserve(p_amt);
	}
	void reserve(size_t p_width, size_t p_height) {
		data.reserve(p_width * p_height);
		width = p_width;
		height = p_height;
	}

	void clear() {
		data.clear();
		data.shrink_to_fit();
	}
	size_t width;
	size_t height;
private:
	std::vector<T> data;
};
