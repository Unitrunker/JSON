#include <string>
#include <stack>
#include "../Stream/Parser.h"

/*
Copyright 2011 Rick Parrish
*/

#pragma once

namespace JSON
{

struct Reader
{
	Reader();
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

	bool open(IInputStream *stream);
	void close();
private:
	Parser _parser;
	bool _quote;

	bool object();
	bool array();
	bool value();
	bool space();
	bool match(char match);
	bool quotedLiteral(const char *text);
    bool literal(const char *text);
};

};