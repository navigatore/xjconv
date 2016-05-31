#ifndef XMLERROR_H
#define XMLERROR_H

#include <string>

class XmlError
{
public:
    XmlError(std::string reason, unsigned int line, unsigned int charNo, std::string moduleName) : reason(reason), line(line), charNo(charNo), moduleName(moduleName) { }
    std::string reasonStr() const noexcept
    {
        std::string description;
        description += moduleName + " error: " + reason + ", line: " + std::to_string(line) + ", char: " + std::to_string(charNo);
        return description;
    }
private:
    std::string reason;
    unsigned int line;
    unsigned int charNo;
    std::string moduleName;
};

class LexerError : public XmlError
{
public:
    LexerError(std::string reason, unsigned int line, unsigned int charNo) : XmlError(reason, line, charNo, "Lexer") { }
};

class ParserError : public XmlError
{
public:
    ParserError(std::string reason, unsigned int line, unsigned int charNo) : XmlError(reason, line, charNo, "Parser") { }
};

#endif // XMLERROR_H
