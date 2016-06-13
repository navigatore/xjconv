#include "xmllexer.h"
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
XmlLexer::Token XmlLexer::readString()
{
    auto t = createToken();
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

    if(onlyWhite(builder))
        return readOther();

    t.set(string, builder);
    return t;
}
//*********************************************************************************************************************
bool XmlLexer::onlyWhite(UnicodeString s)
{
    for(int i = 0 ; i < s.length() ; ++i)
    {
        if (!(s[i] == '\t' || s[i] == ' ' || s[i] == '\r' || s[i] == '\n'))
        {
            return false;
        }
    }
    return true;
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readValue(Token t)
{
    UnicodeString builder = "";
    bool doubleQuote = (it.current() == '"');
    if (!doubleQuote && it.current() != '\'')
        throw LexerError("Single or double quote mark expected", line, charNo);

    next();
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

    if (doubleQuote)
    {
        if (it.current() != '"')
        {
            throw LexerError("Double quote mark expected", line, charNo);
        }
    }
    else
    {
        if (it.current() != '\'')
        {
            throw LexerError("Single quote mark expeted", line, charNo);
        }
    }
    t.set(value, builder);
    return t;
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
XmlLexer::Token XmlLexer::readOther()
{
    skipWhite();
    auto t = createToken();

    if (!it.hasNext())
    {
        t.set(endOfFile);
        return t;
    }
    if (it.current() == '=')    // Equals token
    {
        next();
        return readValue(t);
    }
    if (it.current() == '>')   // End of tag
    {
        next();
        t.set(tagEnd);
        return t;
    }
    if (it.current() == '/')
    {
        next();
        return readEmptyTagEnd(t);
    }
    if (it.current() == '<')
    {
        next();
        return readTagOpen(t);
    }
    if (it.current() == '?')
    {
        next();
        return readXmlTagClose(t);
    }

    return readKey();
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readXmlTagClose(Token t)
{
    if (it.current() == '>')  // XML closing tag
    {
        next();
        t.set(xmlTagClose);
        return t;
    }
    throw LexerError("'>' expected", line, charNo);
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readTagOpen(Token t)
{
    if (it.current() == '?')    // XML opening tag
    {
        next();
        t.set(xmlTagOpen);
    }
    else if (it.current() == '!')
    {
        next();
        t.set(doctypeTagOpen);
    }
    else if (it.current() == '/')
    {
        next();
        t.set(closingTagStart);
    }
    else
    {
        t.set(tagStart);
    }
    return t;
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readEmptyTagEnd(Token t)
{
    if (it.current() == '>')
    {
        next();
        t.set(emptyTagEnd);
        return t;
    }
    throw LexerError("'>' expected", line, charNo);
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::readKey()
{
    auto t = createToken();
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
        t.set(key, builder);
        return t;
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
    step();
    skipComments();
}
//*********************************************************************************************************************
void XmlLexer::step()
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
void XmlLexer::skipComments()
{
    while(true)
    {
        bool isComment = false;

        if (it.hasNext() && it.current() == '<')
        {
            step();
            if (it.hasNext() && it.current() == '-')
            {
                step();
                if (it.hasNext() && it.current() == '-')
                {
                    isComment = true;
                }
                else
                {
                    it.previous();
                    it.previous();
                }
            }
            else
            {
                it.previous();
            }
        }

        if(!isComment)
        {
            break;
        }

        UChar prev1 = '.', prev2 = '.';
        while(it.hasNext())
        {
            if (prev2 == '-' && prev1 == '-' && it.current() == '>')
            {
                step();
                break;
            }
            prev2 = prev1;
            prev1 = it.current();
            next();
        }
    }
}
//*********************************************************************************************************************
XmlLexer::Token XmlLexer::createToken()
{
    return Token(line, charNo);
}
//*********************************************************************************************************************
