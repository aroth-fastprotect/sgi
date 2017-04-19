#include <sgi/helpers/string>
#include <iomanip>
#include <cctype>
#include <algorithm>

namespace sgi {
    namespace helpers {

void hexDumpMemory(std::basic_ostream<char>& os, const void * data, size_t size)
{
    unsigned w = 4;
    if(size > 65535)
        w = 6;
    for(unsigned offset = 0; offset < size; offset += 16)
    {
        const unsigned char * pData = (const unsigned char*)data + offset;
        size_t max_row = std::min((size_t)16u, size - (size_t)offset);
        std::stringstream ssAscii;
        os << "0x" << std::hex << std::setw(w) << std::setfill('0') << offset << ':' << ' ';
        static const char * hex_digits = "0123456789abcdef";
        for(unsigned row = 0; row < 16; row++)
        {
            if(row < max_row)
            {
                unsigned char c = (row < max_row) ? *(pData + row) : 0;
                os << hex_digits[(c >> 4) & 0xF] << hex_digits[c & 0xF] << ' ';
                if(std::isprint(c))
                    ssAscii << c;
                else
                    ssAscii << '.';
            }
            else
            {
                os << ' ' << ' ' << ' ';
                ssAscii << ' ';
            }
        }
        os << ssAscii.str();
        os << std::endl;
    }
}

std::string joinStrings(const std::vector<std::string>& input, char delim)
{
	std::stringstream buf;
	for (std::vector<std::string>::const_iterator i = input.begin(); i != input.end(); ++i)
	{
		buf << *i;
		if ((i + 1) != input.end()) buf << delim;
	}
	std::string result;
	result = buf.str();
	return result;
}

std::vector<std::string> & split(const std::string &s, char delim, std::vector<std::string> &elems) 
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> ret;
    split(s, delim, ret);
    return ret;
}


} // namespace helpers
} // namespace sgi
