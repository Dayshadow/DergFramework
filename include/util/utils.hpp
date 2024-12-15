#pragma once


#include <SDL.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <vector>
#include <util/ext/glm/glm.hpp>
#include <numeric>
#include "Framework/Log.hpp"

constexpr auto SIXTY_TIMES_PER_SECOND = 0.01666666666f;
constexpr auto ONE_TIME_PER_SECOND = 0.001f;
// A collection of single functions that are useful in a variety of situations
// breaks if not inline for some reason
namespace utils {
	inline float hireTimeInSeconds() {
		float t = (float)SDL_GetTicks();
		t *= 0.001f;
		return t;
	}
	inline std::string readFile(const char* filePath) {
		std::string content;
		std::ifstream fileStream(filePath, std::ios::in);

		if (!fileStream.is_open()) {
			std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
			return "";
		}

		std::string line = "";
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			content.append(line + "\n");
		}

		fileStream.close();
		return content;
	}
	inline float lerp(float f1, float f2, float t) {
		float offset = f2 - f1;
		return f1 + offset * t;
	}
	inline float rangeMap(float t, float s1, float s2, float e1, float e2) {
		float r = (t - s1) / (s2 - s1);
		return (e2 - e1) * r + e1;
	}
	inline glm::vec2 lerp(glm::vec2 v1, glm::vec2 v2, float t) {
		glm::vec2 offset = v2 - v1;
		return v1 + offset * t;
	}
	template <typename T>
	inline T averageVector(std::vector<T>& p_vec) {
		T total = (T)0;
		for (int i = 0; i < p_vec.size(); i++) {
			total += p_vec[i];
		}
		return total / p_vec.size();
	}
	template <typename T>
	void logVector(T p_vec) { // careful what you toss in here, must be cout-able lol
		for (int i = 0; i < p_vec.size(); i++) {
			std::cout << p_vec[i] << ", ";
		}
		std::cout << "\n";
	}
	inline glm::vec4 vec4Max(glm::vec4 v1, glm::vec4 v2) {
		return {
			std::max(v1.r, v2.r),
			std::max(v1.g, v2.g),
			std::max(v1.b, v2.b),
			std::max(v1.a, v2.a)
		};
	}
	template <typename T>
	T clamp(T v, T p_min, T p_max) {  // make sure whatever you're passing in has a > operator
		return std::min(std::max(v, p_min), p_max);
	}
	inline bool pointCollidingRect(glm::vec2 p_pointPos, glm::vec2 p_rectPos, glm::vec2 p_rectDim) {
		glm::vec2 relativePos = p_rectPos - p_pointPos;
		if ((relativePos.x <= p_rectDim.x) && (relativePos.y <= p_rectDim.y) && (relativePos.x > 0.0f) && (relativePos.y > 0.0f)) {
			return true;
		};
		return false;
	}
	// wraps float values to the range [0, r] 
	inline float fwrapUnsigned(float x, float r) {
		float aR = fabs(r); // The wrapping point should always be positive or else weird things happen
		// Piecewise fmod transformation function
		return x < 0 ? (fmodf(x, aR) + r) : fmodf(x, aR);
	}
	inline unsigned int modUnsigned(int x, unsigned int d) {
		if (x >= 0) {
			return x % d;
		}
		else {
			return d - 1 - abs((x + 1) % (int)d);
		}
	}
	inline int gridFloor(int x, unsigned int d) {
		return (x < 0) ? (x + 1) / (int)d - 1 : x / (int)d;
	}

	inline uint8_t* toRGBAUnsignedCharArray(float* floats, size_t p_outByteCount) {
		uint8_t* out = (uint8_t*)malloc(p_outByteCount);
		if (!out) {
			ERROR_LOG("Could not allocate enough memory to convert to unsigned char array!");
			throw std::bad_alloc();
			return nullptr;
		}
		if (p_outByteCount < 8 || p_outByteCount % 4 != 0) {
			ERROR_LOG("Specified unsigned char output size is invalid.");
			throw std::invalid_argument("Please specify a size in bytes greater or equal to 4 for conversion, and/or divisible by four.");
			return nullptr;
		}
		for (int i = 0; i < p_outByteCount; i += 4) {
			out[i + 0] = (uint8_t)(floats[i + 0] * 255.f);
			out[i + 1] = (uint8_t)(floats[i + 1] * 255.f);
			out[i + 2] = (uint8_t)(floats[i + 2] * 255.f);
			out[i + 3] = (uint8_t)(floats[i + 3] * 255.f);
		}
		return out;
	}
	inline uint8_t* divideRes(uint32_t n, uint32_t p_width, uint32_t p_height, const uint8_t* p_data) {
		if (n == 0 || p_width / n == 0 || p_height / n == 0 || p_height == 0 || p_width == 0 || (p_width * p_height * 4) / n < 4 || n > p_width) {
			ERROR_LOG("Invalid parameters passed into divide resolution function.");
			throw std::invalid_argument("Bad Params to divideRes");
			return nullptr;
		}
		uint8_t* out = (uint8_t*)malloc(size_t((p_width * p_height * 4) / n));
		if (!out) {
			ERROR_LOG("Could not allocate enough memory to divide input resolution!");
			throw std::bad_alloc();
			return nullptr;
		}
		for (uint32_t y = 0; y < p_height / n; y++) {
			for (uint32_t x = 0; x < p_width * 4; x += n * 4) {
				uint32_t total_red = 0;
				uint32_t total_green = 0;
				uint32_t total_blue = 0;
				uint32_t total_alpha = 0;

				for (uint32_t j = y * n; j < (y + 1) * n; j++) {
					for (uint32_t i = x; i < x + n * 4; i += 4) {
						total_red += p_data[j * p_width * 4 + i];
						total_green += p_data[j * p_width * 4 + i + 1];
						total_blue += p_data[j * p_width * 4 + i + 2];
						total_alpha += p_data[j * p_width * 4 + i + 3];
					}
				}

				uint32_t num_pixels = n * n;
				uint8_t avg_red = total_red / num_pixels;
				uint8_t avg_green = total_green / num_pixels;
				uint8_t avg_blue = total_blue / num_pixels;
				uint8_t avg_alpha = total_alpha / num_pixels;

				out[y * ((p_width * 4) / n) + x / n + 0] = avg_red;
				out[y * ((p_width * 4) / n) + x / n + 1] = avg_green;
				out[y * ((p_width * 4) / n) + x / n + 2] = avg_blue;
				out[y * ((p_width * 4) / n) + x / n + 3] = avg_alpha;
			}
		}
		return out;
	}
	inline glm::vec3 lerp(const glm::vec3& v1, const glm::vec3& v2, float t) {
		return (1.0f - t) * v1 + t * v2;
	}
}
// A utility class that can provide fps readouts, and also just measure times.
struct fpsGauge {
	fpsGauge() { }
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	std::chrono::microseconds elapsed = std::chrono::microseconds(0);
	//std::vector<double> frametimeBuffer;
	double lastFrametimeAverage = 0.0f;

	void startStopwatch() {
		start = std::chrono::system_clock::now();
	}
	void stopStopwatch() {
		elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start);
	}
	double getSecondsElapsed() {
		return (double)elapsed.count() / 1000000.0;
	}
	/** Adds a new frametime to the gauge's buffer, removes any old ones.
	* 
	* @param persistence 0 to 1, higher values means it takes longer for a change in fps to matter.
	*/
	void update(float persistence) {
		stopStopwatch();
		startStopwatch();
		lastFrametimeAverage = lastFrametimeAverage * persistence + getSecondsElapsed() * (1.f - persistence);
	}
	float getFrametimeAverage() {
		return (float)lastFrametimeAverage;
	}

};

// just a really basic timer, nothing fancy
struct Timer {
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	Timer() {
		start = std::chrono::high_resolution_clock::now();
	}
	~Timer() {
		std::chrono::duration<double, std::milli> duration = std::chrono::high_resolution_clock::now() - start;
		std::cout << "The simple timer took " << duration.count() << "ms!" << std::endl;
	}
};
