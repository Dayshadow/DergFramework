#pragma once
#pragma once

#include <vector>
#include "Framework/Log.hpp"
#include <algorithm>
template<class T>
// an Array2D that keeps its data in a raw pointer, handles its own memory
// Allocated data must be past in externally at the moment, serves as a wrapper.
// WARNING: DOES NOT BOUNDS CHECK
// IF YOU READ OUT OF BOUNDS IT'S YOUR OWN FAULT, LOL
class StaticArray3D {
public:
	StaticArray3D() : width(0), height(0), data(nullptr), invertDepth(false) {}
	StaticArray3D(size_t p_width, size_t p_height, size_t p_depth) : width(p_width), height(p_height), depth(p_depth), data(nullptr), invertDepth(false) {}
	StaticArray3D(const StaticArray3D<T>& other) = delete;

	StaticArray3D(StaticArray3D<T>&& other) noexcept {
		data = other.data;
		other.data = nullptr;
		initialized = other.initialized;
		other.initialized = false;
		width = other.width;
		height = other.height;
		depth = other.depth;
	}

	~StaticArray3D() {
		if (data) free(data);
	}
	StaticArray3D<T> operator=(const StaticArray3D<T>& other) = delete;

	T& operator()(size_t x, size_t y, size_t z) {
#ifdef SBBB_DEBUG
		if (!bounded((int)x, (int)y, (int)z)) {
			throw std::out_of_range("3D Array index out of bounds.");
		}
#endif
		if (!invertDepth) {
			return data[(z * width * height) + ((y * width) + x)];
		}
		else {
			return data[((depth - 1 - z) * width * height) + ((y * width) + x)];
		}

	}
	// Can only be ran if the array does not have data, otherwise, it is fixed and cannot change
	void resize(size_t p_width, size_t p_height, size_t p_depth) {
		if (initialized) {
			ERROR_LOG("Tried to resize an initialized static array.");
			throw std::exception();
			return;
		}
		width = p_width;
		height = p_height;
		depth = p_depth;
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
		CONDITIONAL_LOG(!initialized, "RETRIEVED NULLPTR FROM STATIC 3D ARRAY. NOT GREAT.");
		return data;
	}

	bool bounded(int x, int y, int z) {
		if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth) return true;
		return false;
	}

	void reverse() {
		std::reverse(data, &data[width * height * depth]);
	}

	void clear() {
		memset(data, 0, width * height * depth * sizeof(T));
	}

	void reset() {
		if (data) free(data);
		width = 0;
		height = 0;
		initialized = false;
	}
	size_t width;
	size_t height;
	size_t depth;
	bool initialized = false;
	bool invertDepth;
private:
	T* data;
};
