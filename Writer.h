#include <string>
#include <stack>
#include <tchar.h>

/*
Copyright 2011 Rick Parrish
*/

#pragma once

namespace JSON
{

struct Writer
{
	Writer();
	bool Open(const TCHAR *path);
	bool writeStartObject();
	bool writeName(const char *name);
	bool writeStartNamedObject(const char *name);
	bool writeNamedValue(const char *name, const char *value);
	bool writeNamedValue(const char *name, const wchar_t *value);
	bool writeNamedValue(const char *name, const bool value);
	bool writeNamedValue(const char *name, const double value, const char *format = "%f");
	bool writeNamedValue(const char *name, const long value, char base = 10);
	bool writeEndObject(bool eol = false);
	bool writeStartArray();
	bool writeStartNamedArray(const char *name);
	bool writeEndArray();
	bool Close();
	std::stack<bool> _stack;
private:
	bool writeComma();
	FILE *_file;
	bool _comma;
	bool _eol;
};

};