#include <string>
#include <stack>

/*
Copyright 2011 Rick Parrish
*/

#pragma once

namespace JSON
{

struct Reader
{
	Reader(const char *text, size_t length) : cursor(text), remains(length) { };
	bool beginObject();
	bool endObject();
	bool beginArray();
	bool beginNamedArray(const char *name);
	bool endArray();
	bool decimal(double &fValue);
	bool integer(long &iValue);
	bool string(std::string &strValue);
	bool number(std::string &strValue);
	bool boolean(bool &bValue);
	bool null();
	bool namedValue(const char *name, std::string &strValue);
	bool namedValue(const char *name, std::wstring &strValue);
    bool namedValue(const char *name, bool &bValue);
    bool namedValue(const char *name, long &iValue);
    bool namedValue(const char *name, double &fValue);
	bool beginNamedObject(const char *name);
	bool comma();
private:
	const char* cursor;
	size_t remains;

	bool object();
	bool array();
	bool value();
	bool space();
	bool match(char match);
	bool quotedLiteral(const char *text);
    bool literal(const char *text);
};

};