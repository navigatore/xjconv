#include "xmllexer.h"
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::read(TokenType expected)
{
    if (expected == string)
    {
        return readString();
    }
    if (expected == value)
    {
        return readValue();
    }
    else    // expected -> key
    {
        return readWhenKeyExpected();
    }
}
//*********************************************************************************************************************
bool XmlLexer::currentIsStringChar()
{
    return it.hasNext() && ( it.current() != '<' && it.current() != '>');
}
//*********************************************************************************************************************
bool XmlLexer::currentIsValueChar()
{
    return it.hasNext()
            && currentIsStringChar()
            && it.current() != '\''
            && it.current() != '\"';
}
//*********************************************************************************************************************
bool XmlLexer::currentIsKeyChar()
{
    return it.hasNext()
            && (currentIsKeyFirstChar()
                || (it.current() >= '0' && it.current() <= '9')
                || it.current() == '.'
                || it.current() == '-');
}
//*********************************************************************************************************************
bool XmlLexer::currentIsKeyFirstChar()
{
    return it.hasNext()
            && (it.current() == ':'
                || it.current() == '_'
                || (it.current() >= 'a' && it.current() <= 'z')
                || (it.current() >= 'A' && it.current() <= 'Z'));
}
//*********************************************************************************************************************
bool XmlLexer::currentIsHexChar()
{
    return it.hasNext()
            && ( (it.current() >= '0' && it.current() <= '9')
                || (it.current() >= 'a' && it.current() <= 'f')
                || (it.current() >= 'A' && it.current() <= 'F'));
}
//*********************************************************************************************************************
int XmlLexer::valueOfHex(UChar c)
{
    if (c >= '0' && c <= '9')
    {
        int b = c - '0';
        return b;
    }
    else if (c >= 'a' && c <= 'f')
    {
        int b = c - 'a' + 10;
        return b;
    }
    else if (c >= 'A' && c <= 'F')
    {
        int b = c - 'A' + 10;
        return b;
    }
    else
        throw LexerError("Hex digit expected", line, charNo);
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readComment()
{
    UChar prev1 = '.', prev2 = '.';
    UnicodeString builder = "";
    while(it.hasNext())
    {
        if (prev2 == '-' && prev1 == '-' && it.current() == '>')
        {
            next();
            break;
        }
        builder.append(prev2);
        prev2 = prev1;
        prev1 = it.current();
        next();
    }
    builder.remove(0, 2);
    Token token;
    token.type = comment;
    token.content = builder;
    return token;
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readString()
{
    Token token;
    token.type = string;
    token.line = line;
    token.charNo = charNo;
    UnicodeString builder = "";
    while (currentIsStringChar())
    {
        if (it.current() == '&')
        {
            next();
            if (it.hasNext() && it.current() == '#')
            {
                next();
                builder.append(getHex());
            }
            else if (it.hasNext())
            {
                builder.append(getSpecial());
            }
            else
                throw LexerError("'#' or special character expected", line, charNo);
        }
        else
        {
            builder.append(it.current());
            next();
        }
    }

    token.content = builder;

    bool onlyWhite = true;
    for(int i = 0 ; i < token.content.length() ; ++i)
    {
        if (! (token.content[i] == '\t' || token.content[i] == ' ' || token.content[i] == '\r' || token.content[i] == '\n') )
        {
            onlyWhite = false;
            break;
        }
    }

    if(onlyWhite)
        token = read(key);

    return token;
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readValue()
{
    Token token;
    token.type = value;
    token.line = line;
    token.charNo = charNo;

    UnicodeString builder = "";
    while (currentIsValueChar())
    {
        if (it.current() == '&')
        {
            next();
            if (it.hasNext() && it.current() == '#')
            {
                next();
                builder.append(getHex());
            }
            else if (it.hasNext())
            {
                builder.append(getSpecial());
            }
            else
                throw LexerError("Unexpected end of file", line, charNo);
        }
        else
        {
            builder.append(it.current());
            next();
        }
    }

    token.content = builder;
    return token;
}
//*********************************************************************************************************************
UChar XmlLexer::getHex()
{
    UChar builder;
    int i;
    int escaped = 0;
    for (i = 0; currentIsHexChar() && i < 4; ++i)
    {
        escaped <<= 4;
        escaped += valueOfHex(it.current());
        next();
    }
    if (i == 4 && it.hasNext() && it.current() == ';')
    {
        next();
        builder=static_cast<UChar>(escaped);
        return builder;
    }
    throw LexerError("4 hex digits expected", line, charNo);
}
//*********************************************************************************************************************
UChar XmlLexer::getSpecial()
{
    UChar content;
    bool recognized = false;

    if (it.current() == 'l')
    {
        next();
        if (it.current() == 't')
        {
            next();
            if (it.current() == ';')
            {
                next();
                content = '<';
                recognized = true;
            }
        }
    }
    else if (it.current() == 'g')
    {
        next();
        if (it.current() == 't')
        {
            next();
            if (it.current() == ';')
            {
                next();
                content = '>';
                recognized = true;
            }
        }
    }
    else if (it.current() == 'a')
    {
        next();
        if (it.current() == 'm')
        {
            next();
            if (it.current() == 'p')
            {
                next();
                if (it.current() == ';')
                {
                    next();
                    content = '&';
                    recognized = true;
                }
            }
        }
    }

    if (!recognized)
        throw LexerError("Special character not recognized", line, charNo);

    return content;
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readWhenKeyExpected()
{
    Token token;
    do
    {
        token.type = unspecified;
        token.content = "";

        skipWhite();
        token.line = line;
        token.charNo = charNo;

        if (!it.hasNext())
        {
            token.type = endOfFile;
            token.content = "";
        }

        else if (it.current() == '=')    // Equals token
        {
            next();
            token.type = equals;
        }
        else if (it.current() == '>')   // End of tag
        {
            next();
            token.type = tagEnd;
        }
        else if (it.current() == '\'')
        {
            next();
            token.type = singleQuote;
        }
        else if (it.current() == '"')
        {
            next();
            token.type = doubleQuote;
        }
        else if (it.current() == '&')
        {
            next();
            if (it.current() == '#')    // Hex escape
            {
                next();
                token.type = hex;
            }
            else    // Special character
            {
                token.type = special;
            }
        }

        else if (it.current() == '/')
        {
            next();
            if(it.current() == '>')
            {
                next();
                token.type = emptyTagEnd;
            }
        }

        else if (it.current() == '<')
        {
            next();

            if (it.current() == '?')    // XML opening tag
            {
                token.type = xmlTagOpen;
                next();
            }
            else if (it.current() == '!')
            {
                next();
                if (it.current() == '-')
                {
                    next();
                    if (it.current() == '-')    // Comment opening tag
                    {
                        next();
                        token.type = comment;
                        readComment();
                    }
                }
                else    // Doctype opening tag
                {
                    token.type = doctypeTagOpen;
                    return token;
                }
            }
            else if (it.current() == '/')
            {
                next();
                {
                    token.type = closingTagStart;
                }

            }
            else
            {
                token.type = tagStart;
            }
        }

        else if (it.current() == '?')
        {
            next();
            if (it.current() == '>')  // XML closing tag
            {
                next();
                token.type = xmlTagClose;
            }
        }

        else // keyToken
        {
            return readKey();
        }

    } while (token.type == comment);

    if (token.type != unspecified)
        return token;
    throw LexerError("Not recognized token", line, charNo);
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readKey()
{
    Token token;
    token.type = key;
    token.line = line;
    token.charNo = charNo;
    if (currentIsKeyFirstChar())
    {
        UnicodeString builder = "";
        builder.append(it.current());
        next();
        while (currentIsKeyChar())
        {
            builder.append(it.current());
            next();
        }

        token.content = builder;
        return token;
    }
    throw LexerError("Key first char expected", line, charNo);
}
//*********************************************************************************************************************
void XmlLexer::skipWhite()
{
    while (it.current() == ' ' || it.current() == '\t' || it.current() == '\n' || it.current() == '\r')
    {
        next();
    }
}
//*********************************************************************************************************************
void XmlLexer::next()
{
    if (it.current() == '\n')
    {
        line++;
        charNo = 1;
    }
    else
        charNo++;
    it.next();
}
//*********************************************************************************************************************
