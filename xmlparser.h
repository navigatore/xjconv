#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "xmllexer.h"
#include "element.h"
#include "complexelement.h"
#include <memory>

class XmlParser
{
public:
    typedef std::shared_ptr<ComplexElement> ComplexElementP;

    XmlParser(UnicodeString input);
    ComplexElementP getDom() { return domRoot; }

private:

    void documentRule(ComplexElementP elem);
    void prologRule(ComplexElementP elem);
    void xmlTagRule(ComplexElementP elem);
    void attributeRule(ComplexElementP elem);
    void nameRule(ComplexElementP elem);
    void nameStartCharRule();
    void nameCharRule();
    void valueRule();
    void valueCharRule();
    void charRule();
    void elementRule(ComplexElementP elem);
    void closingTagRule();
    void tagFrontRule();
    void tagInteriorRule(ComplexElementP elem);
    void doctypeTagRule();
    void hexTokenRule();
    void hexRule();

    XmlLexer lexer;
    XmlLexer::Token token;
    ComplexElementP domRoot;
};

#endif // XMLPARSER_H
