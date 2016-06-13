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
    ElementP documentRule();
    ElementP prologRule();
    ElementP xmlTagRule();
    ElementP attributeRule();
    ElementP nameRule(bool isAttribute = false);
    ElementP mainElementRule();
    ElementP elementRule();
    ElementP tagInteriorRule();
    ElementP doctypeTagRule();
    void closingTagRule();

    XmlLexer lexer;
    XmlLexer::Token token;
    ElementP domRoot;
    std::stack<UnicodeString> stack;
};

#endif // XMLPARSER_H
