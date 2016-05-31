#include "xmlparser.h"
#include "element.h"

//*********************************************************************************************************************
XmlParser::XmlParser(UnicodeString input) : lexer(input)
{
    domRoot = std::shared_ptr<Element>(new Element("/", "/"));
    token = lexer.read(XmlLexer::key);
    documentRule(domRoot);
}
//*********************************************************************************************************************
void XmlParser::documentRule(ElementP elem)
{
    if (token.type == XmlLexer::xmlTagOpen || token.type == XmlLexer::doctypeTagOpen)
        prologRule(elem);
    elementRule(elem);
}
//*********************************************************************************************************************
void XmlParser::prologRule(ElementP elem)
{
    do
    {
        if (token.type == XmlLexer::xmlTagOpen)
            xmlTagRule(elem);
        else if (token.type == XmlLexer::doctypeTagOpen)
            doctypeTagRule(elem);
        else
            throw -1;
    } while (token.type == XmlLexer::xmlTagOpen || token.type == XmlLexer::doctypeTagOpen);
}
//*********************************************************************************************************************
void XmlParser::xmlTagRule(ElementP elem)
{
    takeXmlTagOpen();
    ElementP xmlElem = ElementP(new Element());
    elem->addElement(xmlElem);
    tagInteriorRule(xmlElem);
    takeXmlTagClose();
}
//*********************************************************************************************************************
void XmlParser::doctypeTagRule(ElementP elem)
{
    takeDoctypeTagOpen();
    ElementP doctypeElem(new Element());
    elem->addElement(doctypeElem);
    tagInteriorRule(doctypeElem);
    takeTagEnd();
}
//*********************************************************************************************************************
bool XmlParser::elementRule(ElementP root)
{
    // It is possible that there is nothing where element was expected
    if (token.type == XmlLexer::closingTagStart)
        return false;

    if (token.type == XmlLexer::tagStart)
    {
        takeTagStart();
        ElementP tagElem = ElementP(new Element());
        root->addElement(tagElem);
        tagInteriorRule(tagElem);

        if (token.type == XmlLexer::emptyTagEnd)
        {
            takeEmptyTagEnd();
        }
        else if(token.type == XmlLexer::tagEnd)
        {
            takeTagEnd();
            while(elementRule(tagElem));
            closingTagRule();
        }
        else
            throw -1;
    }
    else if (token.type == XmlLexer::string)
    {
        ElementP stringElem = ElementP(new Element(token.content));
        takeString();
        root->addElement(stringElem);
    }
    else
        throw -1;

    return true;
}
//*********************************************************************************************************************
void XmlParser::tagInteriorRule(ElementP elem)
{
    nameRule(elem);
    while (token.type == XmlLexer::key)
        attributeRule(elem);
}
//*********************************************************************************************************************
void XmlParser::attributeRule(ElementP elem)
{
    ElementP attr = ElementP(new Element());
    elem->addElement(attr);
    nameRule(attr, true);
    if (token.type == XmlLexer::equals)
    {
        takeEquals();
        ElementP value = ElementP(new Element());
        attr->addElement(value);
        valueRule(value);
    }
}
//*********************************************************************************************************************
void XmlParser::nameRule(ElementP elem, bool isAttribute)
{
    if (isAttribute)
        elem->setName("@" + token.content);
    else
        elem->setName(token.content);
    token = lexer.read(XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::valueRule(ElementP elem)
{

    if (token.type == XmlLexer::doubleQuote)
    {
        token = lexer.read(XmlLexer::value);
        elem->setName(token.content);
        token = lexer.read(XmlLexer::key);
        if (token.type != XmlLexer::doubleQuote)
            throw -1;
    }
    else if (token.type == XmlLexer::singleQuote)
    {
        token = lexer.read(XmlLexer::value);
        elem->setName(token.content);
        token = lexer.read(XmlLexer::key);
        if (token.type != XmlLexer::singleQuote)
            throw -1;
    }
    else
        throw -1;
    token = lexer.read(XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::closingTagRule()
{
    takeClosingTagStart();
    if(token.type != XmlLexer::key)
        throw -1;
    token = lexer.read(XmlLexer::key);
    takeTagEnd();
}
//*********************************************************************************************************************
void XmlParser::takeTagStart()
{
    take(XmlLexer::tagStart, XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::takeEmptyTagEnd()
{
    take(XmlLexer::emptyTagEnd, XmlLexer::string);
}
//*********************************************************************************************************************
void XmlParser::takeTagEnd()
{
    take(XmlLexer::tagEnd, XmlLexer::string);
}
//*********************************************************************************************************************
void XmlParser::takeString()
{
    take(XmlLexer::string, XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::takeClosingTagStart()
{
    take(XmlLexer::closingTagStart, XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::takeEquals()
{
    take(XmlLexer::equals, XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::takeXmlTagOpen()
{
    take(XmlLexer::xmlTagOpen, XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::takeXmlTagClose()
{
    take(XmlLexer::xmlTagClose, XmlLexer::string);
}
//*********************************************************************************************************************
void XmlParser::takeDoctypeTagOpen()
{
    take(XmlLexer::doctypeTagOpen, XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::take(XmlLexer::TokenType tokenType, XmlLexer::TokenType nextExpectedType)
{
    if (token.type != tokenType)
    {
        throw -1;
    }
    token = lexer.read(nextExpectedType);
}
//*********************************************************************************************************************
