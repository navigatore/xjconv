#include "xmlparser.h"
#include "complexelement.h"

//*********************************************************************************************************************
XmlParser::XmlParser(UnicodeString input) : lexer(input)
{
    domRoot = std::shared_ptr<ComplexElement>(new ComplexElement("root", "root"));
}
//*********************************************************************************************************************
void XmlParser::documentRule(ComplexElementP elem)
{
    if (token.type == XmlLexer::xmlTagOpen || token.type == XmlLexer::doctypeTagOpen)
        prologRule(elem);
    elementRule(elem);
}
//*********************************************************************************************************************
void XmlParser::prologRule(ComplexElementP elem)
{
    do
    {
        if (token.type == XmlLexer::xmlTagOpen)
            xmlTagRule(elem);
        else if (token.type == XmlLexer::doctypeTagOpen)
            doctypeTagRule();
        else
            throw -1;
    } while (token.type == XmlLexer::xmlTagOpen || token.type == XmlLexer::xmlTagClose);
}
//*********************************************************************************************************************
void XmlParser::xmlTagRule(ComplexElementP elem)
{
    if (token.type == XmlLexer::xmlTagOpen)
    {
        token = lexer.read(XmlLexer::key);
        tagInteriorRule();
        if (token.type == XmlLexer::xmlTagClose)
            token = lexer.read(XmlLexer::string);
        else
            throw -1;
    }
    else
        throw -1;
}
//*********************************************************************************************************************
void XmlParser::doctypeTagRule()
{
    if (token.type == XmlLexer::doctypeTagOpen)
    {
        token = lexer.read(XmlLexer::key);
        tagInteriorRule();
        if (token.type == XmlLexer::tagEnd)
            token = lexer.read(XmlLexer::string);
        else
            throw -1;
    }
    else
        throw -1;
}
//*********************************************************************************************************************
void XmlParser::elementRule(ComplexElementP elem)
{
    // It is possible that there is nothing where element was expected
    if (token.type == XmlLexer::tagStart || token.type == XmlLexer::closingTagStart)
        return;

    if (token.type == XmlLexer::tagStart)
    {
        token = lexer.read(XmlLexer::key);
        tagInteriorRule(elem);
        if (token.type != XmlLexer::emptyTagEnd)
        {
            elementRule(elem);
            closingTagRule();
        }
    }
    else if (token.type == XmlLexer::string)
        ;
    else
        throw -1;
}
//*********************************************************************************************************************
void XmlParser::tagInteriorRule(ComplexElementP elem)
{
    ComplexElementP newElem = ComplexElementP(new ComplexElement());
    elem->addElement(newElem);
    nameRule(newElem);
    while (token.type == XmlLexer::key)
        attributeRule(newElem);
}

//*********************************************************************************************************************
void XmlParser::attributeRule(ComplexElementP elem)
{
    ComplexElementP attr = ComplexElementP(new ComplexElement());
    elem->addElement(attr);
    nameRule(attr);
    if (token.type == XmlLexer::equals)
    {
        token = lexer.read(XmlLexer::key);
        SimpleElementP value = SimpleElementP(new SimpleElement());
        attr->addElement(value);
        valueRule();
    }
}
//*********************************************************************************************************************
void XmlParser::nameRule(ComplexElementP elem)
{
    elem->setName(token.content);
    elem->setNameAfterMap(token.content);
    token = lexer.read(XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::valueRule()
{
    // HOHO HO HO
}
//*********************************************************************************************************************
void XmlParser::closingTagRule()
{
    if (token.type == XmlLexer::closingTagStart)
    {
        token = lexer.read(XmlLexer::key);
        if (token.type == XmlLexer::key)
        {
            token = lexer.read(XmlLexer::key);
            if (token.type != XmlLexer::tagEnd)
            {
                throw -1;
            }
        }
        else
        {
            throw -1;
        }
    }
    else
        throw -1;
}

//*********************************************************************************************************************
