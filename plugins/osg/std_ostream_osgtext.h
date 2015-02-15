#pragma once
namespace std {

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::TextBase::AxisAlignment & t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::TextBase::CharacterSizeMode & t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::TextBase::Layout & t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::KerningType & t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::Text::BackdropType & t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::Text::BackdropImplementation & t);
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgText::Text::ColorGradientMode & t);

}; // namespace std
