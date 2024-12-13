#include "pch.h"
#include "SubpixelID.h"


//https://stackoverflow.com/a/43482688
struct separate_thousands : std::numpunct<char> {
	char_type do_thousands_sep() const override { return ','; }  // separate with commas
	string_type do_grouping() const override { return "\3"; } // groups of 3 digit
};

static std::locale thousandsSeperatedLocale = std::locale(std::cout.getloc(), new separate_thousands);


std::ostream& operator<<(std::ostream& out, const SubpixelID& d)
{
	auto prevLocale = out.imbue(thousandsSeperatedLocale);
	out << static_cast<uint32_t>(d);
	out.imbue(prevLocale);
	return out;
}