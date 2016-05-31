#include "xmllexer.h"
//*********************************************************************************************************************
XmlLexer::XmlLexer(UnicodeString &input) : it(input), pos(0) { }
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
        throw -1;
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
            it.next();
            break;
        }
        builder.append(prev2);
        prev2 = prev1;
        prev1 = it.current();
        it.next();
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
    UnicodeString builder = "";
    while (currentIsStringChar())
    {
        if (it.current() == '&')
        {
            it.next();
            if (it.hasNext() && it.current() == '#')
            {
                it.next();
                builder.append(getHex());
            }
            else if (it.hasNext())
            {
                builder.append(getSpecial());
            }
            else
                throw -1;
        }
        else
        {
            builder.append(it.current());
            it.next();
        }
    }
    Token token;
    token.type = string;
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
    UnicodeString builder = "";
    while (currentIsValueChar())
    {
        if (it.current() == '&')
        {
            it.next();
            if (it.hasNext() && it.current() == '#')
            {
                it.next();
                builder.append(getHex());
            }
            else if (it.hasNext())
            {
                builder.append(getSpecial());
            }
            else
                throw -1;
        }
        else
        {
            builder.append(it.current());
            it.next();
        }
    }
    Token token;
    token.type = value;
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
        it.next();
    }
    if (i == 4 && it.hasNext() && it.current() == ';')
    {
        it.next();
        builder=static_cast<UChar>(escaped);
        return builder;
    }
    throw -1;
}
//*********************************************************************************************************************
UChar XmlLexer::getSpecial()
{
    UChar content;

    if (it.current() == 'l')
    {
        it.next();
        if (it.current() == 't')
        {
            it.next();
            if (it.current() == ';')
            {
                it.next();
                content = '<';
            }
        }
    }
    else if (it.current() == 'g')
    {
        it.next();
        if (it.current() == 't')
        {
            it.next();
            if (it.current() == ';')
            {
                it.next();
                content = '>';
            }
        }
    }
    else if (it.current() == 'a')
    {
        it.next();
        if (it.current() == 'm')
        {
            it.next();
            if (it.current() == 'p')
            {
                it.next();
                if (it.current() == ';')
                {
                    it.next();
                    content = '&';
                }
            }
        }
    }
    else
        throw -1;

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

        if (!it.hasNext())
        {
            token.type = endOfFile;
            token.content = "";
        }

        else if (it.current() == '=')    // Equals token
        {
            it.next();
            token.type = equals;
        }
        else if (it.current() == '>')   // End of tag
        {
            it.next();
            token.type = tagEnd;
        }
        else if (it.current() == '\'')
        {
            it.next();
            token.type = singleQuote;
        }
        else if (it.current() == '"')
        {
            it.next();
            token.type = doubleQuote;
        }
        else if (it.current() == '&')
        {
            it.next();
            if (it.current() == '#')    // Hex escape
            {
                it.next();
                token.type = hex;
            }
            else    // Special character
            {
                token.type = special;
            }
        }

        else if (it.current() == '/')
        {
            it.next();
            if(it.current() == '>')
            {
                it.next();
                token.type = emptyTagEnd;
            }
        }

        else if (it.current() == '<')
        {
            it.next();

            if (it.current() == '?')    // XML opening tag
            {
                token.type = xmlTagOpen;
                it.next();
            }
            else if (it.current() == '!')
            {
                it.next();
                if (it.current() == '-')
                {
                    it.next();
                    if (it.current() == '-')    // Comment opening tag
                    {
                        it.next();
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
                it.next();
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
            it.next();
            if (it.current() == '>')  // XML closing tag
            {
                it.next();
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
    throw -1;
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readKey()
{
    if (currentIsKeyFirstChar())
    {
        UnicodeString builder = "";
        builder.append(it.current());
        it.next();
        while (currentIsKeyChar())
        {
            builder.append(it.current());
            it.next();
        }
        Token token;
        token.type = key;
        token.content = builder;
        return token;
    }
    throw -1;
}
//*********************************************************************************************************************
void XmlLexer::skipWhite()
{
    while (it.current() == ' ' || it.current() == '\t' || it.current() == '\n' || it.current() == '\r')
    {
        it.next();
    }
}
//*********************************************************************************************************************
