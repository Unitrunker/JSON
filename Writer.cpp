#include "stdafx.h"
#include "Writer.h"

/*
Copyright 2011 Rick Parrish
*/

namespace JSON
{

Writer::Writer() : _file(NULL), _comma(false), _eol(false)
{
}

bool Writer::writeComma()
{
	if (_comma)
		fprintf(_file, ", ");
	if (_eol)
	{
		fprintf(_file, "\n");
		_eol = false;
	}
	return true;
}

bool Writer::Open(const char *path)
{
	_comma = false;
	_eol = false;
	errno_t error = fopen_s(&_file, path, "w");
	return error == 0;
}

bool Writer::writeStartObject()
{
	writeComma();
	fprintf(_file, "{");
	_comma = false;
	return true;
}

bool Writer::writeName(const char *name)
{
	writeComma();
	_comma = false;
	fprintf(_file, "\"%s\" : ", name);
	return true;
}

bool Writer::writeStartNamedObject(const char *name)
{
	return writeName(name) && writeStartObject();
}

bool Writer::writeNamedValue(const char *name, const char *value)
{
	writeName(name);
	fprintf(_file, "\"%s\"", value);
	_comma = true;
	return true;
}

bool Writer::writeNamedValue(const char *name, const wchar_t *value)
{
	std::string text;
	size_t size = wcslen(value);
	text.resize( ' ', size * 2 + 1 );
	wcstombs_s(&size, &text[0], text.size(), value, size);
	text.resize(size);
	return writeName(text.c_str());
}

bool Writer::writeNamedValue(const char *name, const bool value)
{
	writeName(name);
	fprintf(_file, "%s", value ? "true" : "false");
	_comma = true;
	return true;
}

bool Writer::writeNamedValue(const char *name, const double value, const char *format)
{
	writeName(name);
	fprintf(_file, format, value);
	_comma = true;
	return true;
}

bool Writer::writeNamedValue(const char *name, const long value, char base)
{
	writeName(name);
	if (base == 10)
		fprintf(_file, "%ld", value);
	else
		fprintf(_file, "0x%08lX", value);
	_comma = true;
	return true;
}

bool Writer::writeEndObject(bool eol)
{
	fprintf(_file, "}");
	_eol = eol;
	_comma = true;
	return true;
}

bool Writer::writeStartArray()
{
	fprintf(_file, "[\n");
	_comma = false;
	return true;
}

bool Writer::writeStartNamedArray(const char *name)
{
	return writeName(name) && writeStartArray();
}

bool Writer::writeEndArray()
{
	fprintf(_file, "]\n");
	_comma = true;
	return true;
}

bool Writer::Close()
{
	_comma = false;
	_eol = false;
	fclose(_file);
	_file = NULL;
	return true;
}

}