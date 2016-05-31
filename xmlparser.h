#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "xmllexer.h"
#include "element.h"
#include <stack>

class XmlParser
{
public:
    XmlParser(UnicodeString input);
    ElementP getDom() { return domRoot; }

private:
    void documentRule(ElementP elem);
    void prologRule(ElementP elem);
    void xmlTagRule(ElementP elem);
    void attributeRule(ElementP elem);
    void nameRule(ElementP elem, bool isAttribute = false);
    void valueRule(ElementP elem);
    void valueCharRule();
    bool elementRule(ElementP elem);
    void closingTagRule();
    void tagInteriorRule(ElementP elem);
    void doctypeTagRule(ElementP elem);

    bool take(XmlLexer::TokenType tokenType, XmlLexer::TokenType nextExpectedType);
    void takeTagStart();
    void takeEmptyTagEnd();
    void takeTagEnd();
    void takeString();
    void takeClosingTagStart();
    void takeEquals();
    void takeXmlTagOpen();
    void takeXmlTagClose();
    void takeDoctypeTagOpen();

    void skipComments(XmlLexer::TokenType expected);

    XmlLexer lexer;
    XmlLexer::Token token;
    ElementP domRoot;
    std::stack<UnicodeString> stack;
};

#endif // XMLPARSER_H
