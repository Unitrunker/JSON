#include "stdafx.h"
#include "Reader.h"

/*
Copyright 2011 Rick Parrish
*/

namespace JSON
{

bool Reader::space()
{
	while (remains)
	{
		if (*cursor == ' ' || *cursor == '\t' || *cursor == '\r' || *cursor == '\n')
		{
			cursor++;
			remains--;
		}
		else break;
	}
	return true;
}

bool Reader::match(char match)
{
	if (remains && *cursor == match)
	{
		cursor++;
		remains--;
		return true;
	}
	return false;
}

bool Reader::literal(const char *match)
{
	const char *scan = cursor;
	size_t leaves = remains;
	while (leaves && *match && *scan == *match)
	{
		match++;
		scan++;
		leaves--;
	}
	// complete match?
	if (*match == 0)
	{
		cursor = scan;
		remains = leaves;
		return true;
	}
	return false;
}

bool Reader::number(std::string &strValue)
{
	strValue.clear();
	if ( remains && *cursor >= '0' && *cursor <= '9' )
	{
		strValue.push_back(*cursor);
		cursor++;
		remains--;
		while ( remains && *cursor >= '0' && *cursor <= '9' )
		{
			strValue.push_back(*cursor);
			cursor++;
			remains--;
		}
		return true;
	}
	return false;
}

static bool isdigit(char digit, char base = 10)
{
	switch(base)
	{
		case 2:
			return digit >= '0' && digit <= '1';

		case 8:
			return digit >= '0' && digit <= '7';

		case 16:
			return digit >= '0' && digit <= '9' ||
				digit >= 'A' && digit <= 'Z' ||
				digit >= 'a' && digit <= 'z';

		case 10:
		default:
			break;
	}
	return digit >= '0' && digit <= '9';
}

bool Reader::integer(long &iValue)
{
	char base = 10;
	std::string strValue;
	if (remains > 2 && strncmp(cursor, "0x", 2) == 0)
	{
		base = 16;
		cursor += 2;
		remains -= 2;
	}
	if ( remains && isdigit(*cursor, base) )
	{
		strValue.push_back(*cursor);
		cursor++;
		remains--;
		while ( remains && isdigit(*cursor, base) )
		{
			strValue.push_back(*cursor);
			cursor++;
			remains--;
		}
		iValue = strtol(strValue.c_str(), NULL, base);
		return true;
	}
	return false;
}

bool Reader::decimal(double &fValue)
{
	std::string strValue;
	if ( remains && *cursor >= '0' && *cursor <= '9' )
	{
		strValue.push_back(*cursor);
		cursor++;
		remains--;
		while ( remains && *cursor >= '0' && *cursor <= '9' )
		{
			strValue.push_back(*cursor);
			cursor++;
			remains--;
		}
		fValue = strtod(strValue.c_str(), NULL);
		return true;
	}
	return false;
}

bool Reader::string(std::string &strValue)
{
	if ( match('"') )
	{
		strValue.clear();
		// TODO: remember where we saw opening quote in case no closing quote.
		while (1)
		{
			if ( match('"') )
			{
				return true;
			}
			else if (remains)
			{
				strValue.push_back(*cursor);
				// consume
				cursor++;
				remains--;
			}
			else break;
		}
		throw __LINE__; // bad syntax
	}
	return false;
}

bool Reader::value()
{
	std::string value;
	return string(value) || 
		number(value) || 
		array() ||
		object() ||
		literal("true") ||
		literal("false") ||
		literal("null");
}

bool Reader::array()
{
	if ( match('[') )
	{
		space();
		if ( value() )
		{
			space();
			if ( match(',') )
			{
				space();
				while ( value() )
				{
					space();
					if ( match(',') )
					{
						space();
						continue;
					}
					break;
				}
			}
		}
		if ( endArray() )
		{
			return true;
		}
		throw __LINE__; // bad syntax
	}
	return false;
}

bool Reader::object()
{
	space();
	if ( beginObject() )
	{
		while (1)
		{
			std::string name;
			space();
			if ( endObject() )
			{
				return true;
			}
			else if ( string(name) )
			{
				space();
				if ( match(':') )
				{
					space();
					if ( value() )
					{
						continue;
					}
				}
			}
			break;
		}
		throw __LINE__;
	}
	return false;
}

bool Reader::beginObject()
{
	space();
	return match('{');
}

bool Reader::endObject()
{
	space();
	return match('}');
}

bool Reader::beginArray()
{
	return match('[');
}

bool Reader::beginNamedArray(const char *name)
{
	const char *keep = cursor;
	size_t leaves = remains;

	if ( quotedLiteral(name) )
	{
		space();
		if (match(':') )
		{
			space();
			if (beginArray())
				return true;
		}
	}
	cursor = keep;
	remains = leaves;
	return false;
}

bool Reader::endArray()
{
	return match(']');
}

bool Reader::boolean(bool &bValue)
{
	if ( literal("true") )
	{
		bValue = true;
		return true;
	}
	else if ( literal("false") )
	{
		bValue = false;
		return true;
	}
	return false;
}

bool Reader::null()
{
	return literal("null");
}

bool Reader::namedValue(const char *name, std::string &value)
{
	const char *keep = cursor;
	size_t leaves = remains;

	if ( quotedLiteral(name) )
	{
		space();
		if (match(':') )
		{
			space();
			if ( string(value) )
			{
				return true;
			}
		}
	}
	cursor = keep;
	remains = leaves;
	return false;
}

bool Reader::namedValue(const char *name, std::wstring &value)
{
	std::string text;
	bool bOK = namedValue(name, text);
	if (bOK)
	{
		size_t size = text.size();
		value.resize(' ', size * 2 + 1);
		mbstowcs_s(&size, &value[0], value.size(), text.c_str(), text.size());
		value.resize(size);
	}
	return bOK;
}

bool Reader::namedValue(const char *name, bool &bValue)
{
	const char *keep = cursor;
	size_t leaves = remains;

	if ( quotedLiteral(name) )
	{
		space();
		if (match(':') )
		{
			space();
			if ( boolean(bValue) )
			{
				return true;
			}
		}
	}
	cursor = keep;
	remains = leaves;
	return false;
}

bool Reader::namedValue(const char *name, long &iValue)
{
	const char *keep = cursor;
	size_t leaves = remains;

	if ( quotedLiteral(name) )
	{
		space();
		if (match(':') )
		{
			space();
			if ( integer(iValue) )
			{
				return true;
			}
		}
	}
	cursor = keep;
	remains = leaves;
	return false;
}

bool Reader::namedValue(const char *name, double &fValue)
{
	const char *keep = cursor;
	size_t leaves = remains;

	if ( quotedLiteral(name) )
	{
		space();
		if (match(':') )
		{
			space();
			if ( decimal(fValue) )
			{
				return true;
			}
		}
	}
	cursor = keep;
	remains = leaves;
	return false;
}

bool Reader::beginNamedObject(const char *name)
{
	const char *keep = cursor;
	size_t leaves = remains;

	if ( quotedLiteral(name) )
	{
		space();
		if (match(':') )
		{
			space();
			if ( beginObject() )
				return true;
		}
	}
	cursor = keep;
	remains = leaves;
	return false;
}

bool Reader::quotedLiteral(const char *match)
{
	space();
	const char *scan = cursor;
	size_t leaves = remains;
	if (leaves && *scan == '"')
	{
		scan++;
		leaves--;
		while (leaves && *match && *scan == *match)
		{
			match++;
			scan++;
			leaves--;
		}
		// complete match?
		if (*match == 0 && leaves && *scan == '"')
		{
			scan++;
			leaves--;
			cursor = scan;
			remains = leaves;
			return true;
		}
	}
	return false;
}

bool Reader::comma()
{
	space();
	return match(',');
}

static void testJSON()
{
	static const char text[] = 
		"{\"a\" : \"me\", \"b\" : false, \"c\" : 12.3, \"d\" : 123 }";

	Reader json(text, sizeof text);

	if ( json.beginObject() )
	{
		std::string value;
		bool bValue = false;
		double fValue = 0.f;
		long iValue = 0;
		if ( json.namedValue("a", value) &&
			json.namedValue("b", bValue) &&
			json.namedValue("c", fValue) &&
			json.namedValue("d", iValue) &&
			json.endObject() )
		{
			puts("pass");
		}
		else
		{
			puts("fail");
		}
	}
}

};