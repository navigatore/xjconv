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
            throw ParserError("'<?' or '<!' expected", token.line, token.charNo);
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
            stack.push(tagElem->getName());
            while(elementRule(tagElem));
            closingTagRule();
        }
        else
            throw ParserError("'/>' or '>' expected", token.line, token.charNo);
    }
    else if (token.type == XmlLexer::string)
    {
        ElementP stringElem = ElementP(new Element(token.content));
        takeString();
        root->addElement(stringElem);
    }
    else
        throw ParserError("'<' or string expected", token.line, token.charNo);

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
            throw ParserError("'\"' expected", token.line, token.charNo);
    }
    else if (token.type == XmlLexer::singleQuote)
    {
        token = lexer.read(XmlLexer::value);
        elem->setName(token.content);
        token = lexer.read(XmlLexer::key);
        if (token.type != XmlLexer::singleQuote)
            throw ParserError("''' expected", token.line, token.charNo);
    }
    else
        throw ParserError("'\"' or ''' expected", token.line, token.charNo);
    token = lexer.read(XmlLexer::key);
}
//*********************************************************************************************************************
void XmlParser::closingTagRule()
{
    takeClosingTagStart();
    if(token.type != XmlLexer::key)
        throw ParserError("key expected", token.line, token.charNo);
    if(token.content != stack.top())
        throw ParserError("unexpected closing tag", token.line, token.charNo);
    stack.pop();
    token = lexer.read(XmlLexer::key);
    takeTagEnd();
}
//*********************************************************************************************************************
void XmlParser::takeTagStart()
{
    if(!take(XmlLexer::tagStart, XmlLexer::key))
        throw ParserError("< expected", token.line, token.charNo);
}
//*********************************************************************************************************************
void XmlParser::takeEmptyTagEnd()
{
    if(!take(XmlLexer::emptyTagEnd, XmlLexer::string))
        throw ParserError("/> expected", token.line, token.charNo);
}
//*********************************************************************************************************************
void XmlParser::takeTagEnd()
{
    if(!take(XmlLexer::tagEnd, XmlLexer::string))
        throw ParserError("> expected", token.line, token.charNo);
}
//*********************************************************************************************************************
void XmlParser::takeString()
{
    if(!take(XmlLexer::string, XmlLexer::key))
        throw ParserError("string expected", token.line, token.charNo);
}
//*********************************************************************************************************************
void XmlParser::takeClosingTagStart()
{
    if(!take(XmlLexer::closingTagStart, XmlLexer::key))
        throw ParserError("'</' expected", token.line, token.charNo);
}
//*********************************************************************************************************************
void XmlParser::takeEquals()
{
    if(!take(XmlLexer::equals, XmlLexer::key))
        throw ParserError("'=' expected", token.line, token.charNo);
}
//*********************************************************************************************************************
void XmlParser::takeXmlTagOpen()
{
    if(!take(XmlLexer::xmlTagOpen, XmlLexer::key))
        throw ParserError("'<?' expected", token.line, token.charNo);
}
//*********************************************************************************************************************
void XmlParser::takeXmlTagClose()
{
    if(!take(XmlLexer::xmlTagClose, XmlLexer::string))
        throw ParserError("'?>' expected", token.line, token.charNo);
}
//*********************************************************************************************************************
void XmlParser::takeDoctypeTagOpen()
{
    if(!take(XmlLexer::doctypeTagOpen, XmlLexer::key))
        throw ParserError("'<!' expected", token.line, token.charNo);
}
//*********************************************************************************************************************
bool XmlParser::take(XmlLexer::TokenType tokenType, XmlLexer::TokenType nextExpectedType)
{
    if (token.type != tokenType)
        return false;
    token = lexer.read(nextExpectedType);
    return true;
}
//*********************************************************************************************************************
