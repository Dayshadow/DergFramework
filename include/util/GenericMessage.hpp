#pragma once

#include <iostream>
#include <vector>
#include <stdint.h>

// for future internal/external networking
namespace net {

	// T should be a custom enum class that the message uses to differentiate.
	template <typename T>
	struct MessageHeader
	{
		T ID{};
		uint32_t size = 0;
	};

	template <typename T>
	struct Message
	{
		MessageHeader<T> header{};
		std::vector<uint8_t> body;

		size_t size() const {
			// it's ok to use the raw size of the vector because it is byte data
			return sizeof(MessageHeader<T>) + body.size();
		}

		friend std::ostream& operator << (std::ostream& p_os, const Message<T>& p_msg) {
			p_os << "Enum ID: " << int(p_msg.header.ID) << " Size: " << p_msg.header.size << " bytes";
			return p_os;
		}

		template <typename t_InputType>
		friend Message<T>& operator << (Message<T>& p_msg, const t_InputType& p_data) {
			static_assert(std::is_standard_layout<t_InputType>::value, "Cannot serialize message input. Too complex.");

			size_t i = p_msg.body.size();
			
			// ensure there is enough room for the new data
			p_msg.body.resize(i + sizeof(t_InputType));

			std::memcpy(p_msg.body.data() + i, &p_data, sizeof(t_InputType));

			p_msg.header.size = (uint32_t)p_msg.size();

			return p_msg;
		}

		template <typename t_InputType>
		friend Message<T>& operator >> (Message<T>& p_msg, t_InputType& p_data) {
			static_assert(std::is_standard_layout<t_InputType>::value, "Cannot serialize message output. Too complex.");

			size_t i = p_msg.body.size() - sizeof(t_InputType);

			std::memcpy(&p_data, p_msg.body.data() + i, sizeof(t_InputType));

			p_msg.body.resize(i);

			p_msg.header.size = (uint32_t)p_msg.size();

			return p_msg;
		}
	};


}
