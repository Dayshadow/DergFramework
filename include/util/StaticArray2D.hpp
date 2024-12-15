#pragma once

#include <vector>
#include "Framework/Log.hpp"
#include <algorithm>
template<class T>
// an Array2D that keeps its data in a raw pointer, handles its own memory
// Allocated data must be past in externally at the moment, serves as a wrapper.
// WARNING: DOES NOT BOUNDS CHECK
// IF YOU READ OUT OF BOUNDS IT'S YOUR OWN FAULT, LOL
class StaticArray2D {
public:
	StaticArray2D() { width = 0; height = 0; data = nullptr; }
	StaticArray2D(uint32_t p_width, uint32_t p_height) {
		width = p_width;
		height = p_height;
	}
	StaticArray2D(const StaticArray2D<T>& other) = delete;

	StaticArray2D(StaticArray2D<T>&& other) noexcept {
		data = other.data;
		other.data = nullptr;
		initialized = other.initialized;
		other.initialized = false;
		width = other.width;
		height = other.height;
	}

	~StaticArray2D() {
		if (data) free(data);
	}
	StaticArray2D<T> operator=(const StaticArray2D<T>& other) = delete;

	T& operator()(int x, int y) {
#ifdef SBBB_DEBUG
		if (!bounded(x, y)) {
			ERROR_LOG("Invalid Index (" << x << ", " << y << ")");
			throw std::exception("2d array out of bounds");
		}
#endif
		if (!initialized) throw std::exception("uninitialized 2d array access");
		return data[((y * width) + x)];
	}
	// Can only be ran if the array does not have data, otherwise, it is fixed and cannot change
	void resize(uint32_t p_width, uint32_t p_height) {
		if (initialized) {
			ERROR_LOG("Tried to resize an initialized static array.");
			throw std::exception();
			return;
		}
		width = p_width;
		height = p_height;
	}

	void fill(T p_fillValue) {
		if (!initialized) throw std::exception();
		for (size_t i = 0; i < width * height; i++) {
			data[i] = p_fillValue;
		}
	}
	void setData(T* p_data) {
		data = p_data;
		initialized = true;
	}
	T* getData() {
		CONDITIONAL_LOG(!initialized, "RETRIEVED NULLPTR FROM STATIC 2D ARRAY. NOT GREAT.");
		return data;
	}

	bool bounded(int x, int y) {
		if (x >= 0 && x < width && y >= 0 && y < height) return true;
		return false;
	}

	void reverse() {
		std::reverse(data, &data[width * height]);
	}

	void clear() {
		memset(data, 0, width * height * sizeof(T));
	}

	void reset() {
		if (data) free(data);
		width = 0;
		height = 0;
		initialized = false;
	}
	size_t width;
	size_t height;
	bool initialized = false;
private:
	T* data;
};
