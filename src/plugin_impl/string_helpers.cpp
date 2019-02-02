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

/**
* Trims whitespace from the ends of a string.
* by Rodrigo C F Dias
* http://www.codeproject.com/KB/stl/stdstringtrim.aspx
*/
void trim2( std::string& str )
{
    static const std::string whitespace (" \t\f\v\n\r");
    std::string::size_type pos = str.find_last_not_of( whitespace );
    if(pos != std::string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of( whitespace );
        if(pos != std::string::npos) str.erase(0, pos);
    }
    else
        str.erase(str.begin(), str.end());
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
