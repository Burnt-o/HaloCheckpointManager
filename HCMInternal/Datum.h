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
		os << std::hex << "0x" << (uint32_t)dt;
		return os;
	}

	Datum(uint32_t in)
	{
		this->index = (uint16_t)(in);
		this->salt = (uint16_t)(in >> 16);
	}

};
static_assert(sizeof(Datum) == sizeof(int32_t));

constexpr Datum nullDatum{};