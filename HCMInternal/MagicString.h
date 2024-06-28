#pragma once


// used for checking magic strings "scnr" "tags" "bipd" etc
union MagicString {
private:
	char chars[4];
	int32_t num;
public:
	explicit MagicString(int32_t num) : num(num) {}
	explicit MagicString(std::string str) { for (int i = 0; i < 4 && i < str.size(); i++) { chars[i] = str[i]; } }
	friend bool operator==(const auto& a, const auto& b) { return a.num == b.num; };

	std::string_view getString() { return std::string_view(chars, 4); }
	const int32_t& getNum() { return num; }
};