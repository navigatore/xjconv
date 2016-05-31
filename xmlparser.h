#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "xmllexer.h"
#include "element.h"
#include <memory>

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
    void nameStartCharRule();
    void nameCharRule();
    void valueRule(ElementP elem);
    void valueCharRule();
    void charRule();
    bool elementRule(ElementP elem);
    void closingTagRule();
    void tagFrontRule();
    void tagInteriorRule(ElementP elem);
    void doctypeTagRule(ElementP elem);
    void hexTokenRule();
    void hexRule();

    void take(XmlLexer::TokenType tokenType, XmlLexer::TokenType nextExpectedType);
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
};

#endif // XMLPARSER_H
