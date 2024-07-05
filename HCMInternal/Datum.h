#pragma once

struct Datum
{
	uint16_t index;
	uint16_t salt;

	//bool operator<(const Datum& rhs) const;
	//bool operator==(const Datum& rhs) const;
	auto operator<=>(const Datum&) const = default;
	constexpr Datum(uint16_t i, uint16_t s) : index(i), salt(s) {}

	constexpr Datum() : index(0xFFFF), salt(0xFFFF) {} // default initialised is nullDatum
	operator uint32_t() const { return (((uint32_t)salt << 16) + index); } // swap endianness

	friend std::ostream& operator<<(std::ostream& os, const Datum& dt)
	{
		os << std::hex << "0x" << std::uppercase << (uint32_t)dt << std::nouppercase;
		return os;
	}

	// todo: make explicit/factory. implicit conversion is bad
	Datum(uint32_t in)
	{
		this->index = (uint16_t)(in);
		this->salt = (uint16_t)(in >> 16);
	}

	bool isNull() const { return this->operator unsigned int() == 0xFFFFFFFF || this->operator unsigned int() == 0; }
	//operator bool() const { return this->operator unsigned int() == 0xFFFFFFFF || this->operator unsigned int() == 0; } // null datum check

	std::string toString() const
	{
		std::stringstream ss;
		ss << *this;
		return ss.str();
	}

};
static_assert(sizeof(Datum) == sizeof(int32_t));

