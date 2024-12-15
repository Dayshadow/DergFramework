#pragma once

#pragma once

#include <vector>
#include "Framework/Log.hpp"
#include <algorithm>
template<class T>
// fake baby 3d array
class Array3D {
public:
	Array3D() { width = 0; height = 0; depth = 0; data = std::vector<T>(); }
	Array3D(uint32_t p_width, uint32_t p_height, uint32_t p_depth) {
		data.resize(p_width * p_height * p_depth, T());
		width = p_width;
		height = p_height;
		depth = p_depth;
	}
	Array3D(const Array3D<T>& other) {
		data = other.data;
		width = other.width;
		height = other.height;
		depth = other.depth;
	}
	Array3D(Array3D<T>&& other) noexcept {
		data = std::move(other.data);
		width = other.width;
		height = other.height;
		depth = other.depth;
	}
	Array3D<T> operator=(const Array3D<T>& other) {
		data = other.data;
		width = other.width;
		height = other.height;
		depth = other.depth;
		return *this;
	}

	T& operator()(size_t x, size_t y, size_t z) {
#ifdef SBBB_DEBUG
		if (!bounded((int)x, (int)y, (int)z)) {
			throw std::out_of_range("3D Array index out of bounds.");
		}
#endif
		if (!invertDepth) {
			return data[(z * width * height) + ((y * width) + x)];
		} else {
			return data[((depth - 1 - z) * width * height) + ((y * width) + x)];
		}

	}
	T& operator()(int x, int y, int z) {
#ifdef SBBB_DEBUG
		if (!bounded(x, y, z)) {
			throw std::out_of_range("3D Array index out of bounds.");
		}
#endif
		if (!invertDepth) {
			return data[((size_t)z * width * height) + (((size_t)y * width) + (size_t)x)];
		}
		else {
			return data[((depth - 1 - (size_t)z) * width * height) + (((size_t)y * width) + (size_t)x)];
		}

	}
	void fill(T p_fillValue) {
		for (size_t i = 0; i < width * height * depth; i++) {
			data[i] = p_fillValue;
		}
	}
	std::vector<T>& getData() {
		return data;
	}

	bool bounded(int x, int y, int z) {
		if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth) return true;
		return false;
	}
	void resize(uint32_t p_width, uint32_t p_height, uint32_t p_depth) {
		data.resize(p_width * p_height * p_depth, T());
		width = p_width;
		height = p_height;
		depth = p_depth;
	}

	void reverse() {
		std::reverse(data.begin(), data.end());
	}

	void reserve(uint32_t p_amt) {
		data.reserve(p_amt);
	}
	void reserve(uint32_t p_width, uint32_t p_height, uint32_t p_depth) {
		data.reserve(p_width * p_height * p_depth);
		width = p_width;
		height = p_height;
		depth = p_depth;
	}

	void clear() {
		data.clear();
		data.shrink_to_fit();
	}
	size_t width;
	size_t height;
	size_t depth;
	bool invertDepth = false;
private:
	std::vector<T> data;
};
