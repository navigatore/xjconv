#ifndef XMLLEXER_H
#define XMLLEXER_H

#include "xmlerror.h"
#include <unicode/schriter.h>

class XmlLexer
{
public:
    enum TokenType { unspecified, endOfFile, key, value, string, hex, special, tagStart, closingTagStart, tagEnd, emptyTagEnd, xmlTagOpen, xmlTagClose, doctypeTagOpen, specialChar, hexChar };
    class Token
    {
    public:
        Token(unsigned int line, unsigned int charNo) : type(unspecified), content(""), line(line), charNo(charNo)  { }
        void set(TokenType type, UnicodeString content = "")
        {
            this->type = type;
            this->content = content;
        }

        TokenType type;
        UnicodeString content;
        unsigned int line, charNo;
    };

    XmlLexer(UnicodeString &input) : it(input), line(1), charNo(1) { }
    Token readString();
    Token readOther();

private:
    bool currentIsStringChar();
    bool currentIsValueChar();
    bool currentIsKeyChar();
    bool currentIsKeyFirstChar();
    bool currentIsHexChar();
    bool onlyWhite(UnicodeString s);

    Token readValue(Token t);
    Token readKey();
    Token readXmlTagClose(Token t);
    Token readTagOpen(Token t);
    Token readEmptyTagEnd(Token t);

    UChar getHex();
    UChar getSpecial();
    int valueOfHex(UChar c);

    void skipComments();
    void skipWhite();
    void next();
    void step();
    Token createToken();

    StringCharacterIterator it;
    unsigned int line, charNo;
};

#endif // XMLLEXER_H
