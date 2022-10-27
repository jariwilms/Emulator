#include <stdafx.hpp>

#include "File.hpp"

std::vector<byte> read_file(const std::filesystem::path& path)
{
	std::ifstream file{ path, std::ios::binary };
	std::stringstream ss{};
	std::string s{};
	
	if (!file) throw std::runtime_error("File could not be opened");
	ss << file.rdbuf();
	s = ss.str();
	
	std::vector<byte> res{};
	res.reserve(s.length());
	std::copy(s.begin(), s.end(), std::back_inserter(res));

	return res;
}
