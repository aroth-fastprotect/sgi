#include <sgi/helpers/string>
#include <sgi/helpers/html>
#include <iomanip>
#include <cctype>
#include <algorithm>

#include <QtCore/QDebug>

QDebug & operator<<(QDebug & d, const std::string & s)
{
    return d << s.c_str();
}

namespace sgi {
    namespace helpers {

void hexDumpMemory(std::basic_ostream<char>& os, const void * data, size_t size, size_t maximumSize)
{
    unsigned w = 4;
    if(size > 65535)
        w = 6;
    size_t offset_max = std::min(size, maximumSize);
    bool truncated = size > maximumSize;
    for(unsigned offset = 0; offset < offset_max; offset += 16)
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
    if (truncated)
        os << "truncated after " << maximumSize << " of " << size << " bytes" << endl;
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


/** Replaces all the instances of "sub" with "other" in "s". */
std::string& replaceIn( std::string& s, const std::string& sub, const std::string& other)
{
    if ( sub.empty() ) return s;
    size_t b=0;
    for( ; ; )
    {
        b = s.find( sub, b );
        if ( b == s.npos ) break;
        s.replace( b, sub.size(), other );
        b += other.size();
    }
    return s;
}

std::string& ciReplaceIn( std::string& s, const std::string& pattern, const std::string& replacement )
{
    if ( pattern.empty() ) return s;

    std::string upperSource = s;
    std::transform( upperSource.begin(), upperSource.end(), upperSource.begin(), (int(*)(int))std::toupper );

    std::string upperPattern = pattern;
    std::transform( upperPattern.begin(), upperPattern.end(), upperPattern.begin(), (int(*)(int))std::toupper );

    for( size_t b = 0; ; )
    {
        b = upperSource.find( upperPattern, b );
        if ( b == s.npos ) break;
        s.replace( b, pattern.size(), replacement );
        upperSource.replace( b, upperPattern.size(), replacement );
        b += replacement.size();
    }

    return s;
}

// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

// trim from both ends (in place)
void trim2(std::string& s) {
	ltrim(s);
	rtrim(s);
}

/**
* Trims whitespace from the ends of a string, returning a
* copy of the string with whitespace removed.
*/
std::string trim( const std::string& in )
{
    std::string str = in;
    trim2( str );
    return str;
}


std::string html_encode(const std::string & str)
{
    std::string ret = str;
    replaceIn(ret, "<", "&lt;");
    replaceIn(ret, ">", "&gt;");
    replaceIn(ret, "\"", "&quot;");
    return ret;
}

bool string_to_bool(const std::string & s, bool * ok, bool defaultValue)
{
    if(s.compare("1") == 0 || s.compare("on") == 0 || s.compare("true") == 0 || s.compare("yes") == 0)
    {
        if(ok)
            *ok = true;
        return true;
    }
    else if(s.compare("0") == 0 || s.compare("off") == 0 || s.compare("false") == 0 || s.compare("no") == 0)
    {
        if(ok)
            *ok = true;
        return false;
    }
    else
    {
        if(ok)
            *ok = false;
        return defaultValue;
    }
}

a_href::a_href(const std::string & url)
    : _url(url)
{
}
void a_href::write(std::basic_ostream<char>& os) const
{
    if (!_url.empty())
        os << "<a href=\"" << _url << "\">" << _url << "</a>";
}


std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const a_href & link)
{
    link.write(os);
    return os;
}


} // namespace helpers
} // namespace sgi
