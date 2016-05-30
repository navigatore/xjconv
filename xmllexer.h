#ifndef XMLLEXER_H
#define XMLLEXER_H

#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <iostream>
#include <unicode/schriter.h>

class XmlLexer
{
public:
    enum TokenType { unspecified, endOfFile, key, value, string, hex, special, comment, tagStart, closingTagStart, tagEnd, emptyTagEnd, xmlTagOpen, xmlTagClose, doctypeTagOpen, equals, specialChar, hexChar, startComment, endComment, singleQuote, doubleQuote };
    struct Token
    {
        TokenType type;
        UnicodeString content;
    };

    XmlLexer(UnicodeString &input);
    Token read(TokenType expected);

private:
    bool currentIsStringChar();
    bool currentIsValueChar();
    bool currentIsKeyChar();
    bool currentIsKeyFirstChar();
    bool currentIsHexChar();
    int valueOfHex(UChar c);

    Token readComment();
    Token readString();
    Token readValue();
    Token readHex();
    Token readSpecial();
    Token readKey();
    Token readWhenKeyExpected();

    void skipWhite();

    StringCharacterIterator it;
    int pos;
};

#endif // XMLLEXER_H
