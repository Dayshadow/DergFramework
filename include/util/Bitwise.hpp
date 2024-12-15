#pragma once
#include <stdint.h>
#include "Framework/Log.hpp"
#include <exception>

struct Bitwise32 {
	Bitwise32() {
		packed = 0;
	}
	operator uint32_t() const {
		return packed;
	}
	Bitwise32& operator|=(uint32_t bits) {
		packed |= bits;
		return *this;
	}
	// end inclusive
	void setRange(uint8_t p_startOffset, uint8_t p_endOffset, uint32_t p_value) {
#ifdef SBBB_DEBUG
		if (p_endOffset > 31) {
			ERROR_LOG("Bitwise setRange invalid! Endpoint is too high.");
			throw std::exception("Bitwise overflow");
		}
		if (p_startOffset > p_endOffset) {
			ERROR_LOG("Bitwise setRange invalid! Starting point is after ending point. Unsigned underflow?");
			throw std::exception("Bitwise overflow");
		}
#endif
		uint32_t bitmask = 0;
		for (uint8_t i = p_startOffset; i <= p_endOffset; i++) {
			bitmask |= 1 << i;
		}
		packed &= ~bitmask;
		packed |= (p_value << p_startOffset) & bitmask;
	}
	uint32_t unpackRange(uint8_t p_startOffset, uint8_t p_endOffset) {
#ifdef SBBB_DEBUG
		if (p_endOffset > 31) {
			ERROR_LOG("Bitwise unpackRange overflow!");
			throw std::exception("Bitwise overflow");
		}
#endif
		uint32_t out = 0;
		for (uint8_t i = p_startOffset; i <= p_endOffset; i++) {
			out |= (packed & (1 << i)) >> p_startOffset;
		}
		return out;
	}
	uint32_t packed;
};