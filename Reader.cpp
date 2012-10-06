#include "stdafx.h"
#include "Reader.h"

/*
Copyright 2011 Rick Parrish
*/

namespace JSON
{

Reader::Reader() : _quote(false)
{
}

bool Reader::space()
{
	return _parser.skipspace();
}

bool Reader::match(char match)
{
	return _parser.parseMatch(match);
}

bool Reader::literal(const char *match)
{
	return _parser.parseMatch(match);
}

bool Reader::number(std::string &strValue)
{
	strValue.clear();
	while ( !_parser.eof() )
	{
		char peek = _parser.peek();
		if ( peek >= '0' && peek <= '9' )
		{
			strValue.push_back(peek);
			_parser.consume(1);
		}
		else
			break;
	}
	return strValue.size() > 0;
}

static bool isdigit(char digit, char base = 10)
{
	switch(base)
	{
		case 2:
			return digit == '0' || digit == '1';

		case 8:
			return digit >= '0' && digit <= '7';

		case 16:
			return digit >= '0' && digit <= '9' ||
				digit >= 'A' && digit <= 'F' ||
				digit >= 'a' && digit <= 'f';

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

	if ( _parser.parseMatch("0x") )
	{
		base = 16;
	}
	if ( isdigit(_parser.peek(), base) )
	{
		strValue.push_back(_parser.peek());
		_parser.consume(1);
		while ( !_parser.eof() && isdigit(_parser.peek(), base) )
		{
			strValue.push_back(_parser.peek());
		}
		iValue = strtol(strValue.c_str(), NULL, base);
		return true;
	}
	return false;
}

bool Reader::decimal(double &fValue)
{
	std::string strValue;
	if ( !_parser.eof() && _parser.peek() >= '0' && _parser.peek() <= '9' )
	{
		strValue.push_back(_parser.peek());
		_parser.consume(1);
		while ( !_parser.eof() && _parser.peek() >= '0' && _parser.peek() <= '9' )
		{
			strValue.push_back(_parser.peek());
			_parser.consume(1);
		}
		fValue = strtod(strValue.c_str(), NULL);
		return true;
	}
	return false;
}

bool Reader::string(std::string &strValue)
{
	if ( _quote || match('"') )
	{
		_quote = true;
		strValue.clear();
		// TODO: remember where we saw opening quote in case no closing quote.
		while ( !_parser.eof() )
		{
			if ( match('"') )
			{
				_quote = false;
				return true;
			}
			else
			{
				strValue.push_back(_parser.peek());
				// consume
				_parser.consume(1);
			}
			throw __LINE__; // bad syntax
		}
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
	if ( quotedLiteral(name) )
	{
		space();
		if (match(':') )
		{
			space();
			if (beginArray())
				return true;
		}
		throw __LINE__;
	}
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
		throw __LINE__;
	}
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
		throw __LINE__;
	}
	return false;
}

bool Reader::namedValue(const char *name, long &iValue)
{
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
		throw __LINE__;
	}
	return false;
}

bool Reader::namedValue(const char *name, double &fValue)
{
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
		throw __LINE__;
	}
	return false;
}

bool Reader::beginNamedObject(const char *name)
{
	if ( quotedLiteral(name) )
	{
		space();
		if (match(':') )
		{
			space();
			if ( beginObject() )
				return true;
		}
		throw __LINE__;
	}
	return false;
}

bool Reader::quotedLiteral(const char *match)
{
	space();
	if ( !_parser.eof() && (_quote || _parser.parseMatch('"')) )
	{
		// remember the quote.
		_quote = true;
		// complete match?
		if ( _parser.parseMatch(match) )
		{
			// if this fails, we've gone too far and can't back out.
			if ( _parser.parseMatch('"') )
			{
				_quote = false;
				return true;
			}
			else
				throw __LINE__;
		}
	}
	return false;
}

bool Reader::comma()
{
	space();
	return match(',');
}

#include "../Stream/MemoryInputStream.h"

static void testJSON()
{
	static const char text[] = 
		"{\"a\" : \"me\", \"b\" : false, \"c\" : 12.3, \"d\" : 123 }";

	MemoryInputStream stream(reinterpret_cast<const unsigned char *>(&text[0]), sizeof text);

	Reader json;

	if ( json.open(&stream) && json.beginObject() )
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

bool Reader::open(IInputStream *stream)
{
	return _parser.open(stream);
}

void Reader::close()
{
	_parser.close();
}

};