#ifndef XMLLEXER_H
#define XMLLEXER_H

#include "xmlerror.h"
#include <unicode/schriter.h>

class XmlLexer
{
public:
    enum TokenType { unspecified, endOfFile, key, value, string, hex, special, comment, tagStart, closingTagStart, tagEnd, emptyTagEnd, xmlTagOpen, xmlTagClose, doctypeTagOpen, equals, specialChar, hexChar,  singleQuote, doubleQuote };
    struct Token
    {
        TokenType type;
        UnicodeString content;
        unsigned int line, charNo;
    };

    XmlLexer(UnicodeString &input) : it(input), line(1), charNo(1) { }
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
    Token readKey();
    Token readWhenKeyExpected();
    UChar getHex();
    UChar getSpecial();

    void skipWhite();
    void next();

    StringCharacterIterator it;
    unsigned int line, charNo;
};

#endif // XMLLEXER_H
