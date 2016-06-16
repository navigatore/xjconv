#include "xmlparser.h"
#include "element.h"

//*********************************************************************************************************************
XmlParser::XmlParser(UnicodeString input) : lexer(input), token(1,1)
{
    token = lexer.readOther();
    domRoot = documentRule();
}
//*********************************************************************************************************************
ElementP XmlParser::documentRule()
{
    auto root = ElementP(new Element("/", "/"));
    auto elem = prologRule();
    if (elem != nullptr)
        root->addElement(elem);
    root->addElement(mainElementRule());
    return root;
}
//*********************************************************************************************************************
ElementP XmlParser::prologRule()
{
    auto root = ElementP(new Element("prolog", "prolog"));

    while (true)
    {
        auto xmlElem = xmlTagRule();
        if (xmlElem != nullptr)
            root->addElement(xmlElem);
        else
        {
            auto doctypeElem = doctypeTagRule();
            if (doctypeElem != nullptr)
                root->addElement(doctypeElem);
            else
                break;
        }
    }

    if (root->getChildren().size() == 0)
        return nullptr;
    return root;
}
//*********************************************************************************************************************
ElementP XmlParser::xmlTagRule()
{
    if (token.type == XmlLexer::xmlTagOpen)
    {
        token = lexer.readOther();
        auto elem = tagInteriorRule();
        if (token.type == XmlLexer::xmlTagClose)
        {
            token = lexer.readString();
            return elem;
        }
        throw ParserError("'?>' expected", token.line, token.charNo);
    }
    else
        return nullptr;
}
//*********************************************************************************************************************
ElementP XmlParser::doctypeTagRule()
{
    if (token.type == XmlLexer::doctypeTagOpen)
    {
        token = lexer.readOther();
        auto elem = tagInteriorRule();
        if (token.type == XmlLexer::tagEnd)
        {
            token = lexer.readString();
            return elem;
        }
        throw ParserError("'>' expected", token.line, token.charNo);
    }
    else
        return nullptr;
}
//*********************************************************************************************************************
ElementP XmlParser::mainElementRule()
{
    if (token.type == XmlLexer::tagStart)
    {
        token = lexer.readOther();
        auto elem = tagInteriorRule();

        if (token.type == XmlLexer::emptyTagEnd)
        {
            token = lexer.readString();
        }
        else if(token.type == XmlLexer::tagEnd)
        {
            token = lexer.readString();
            stack.push(elem->getName());
            ElementP nested;
            while((nested = elementRule()) != nullptr)
                elem->addElement(nested);
            closingTagRule();
        }
        else
            throw ParserError("'/>' or '>' expected", token.line, token.charNo);

        return elem;
    }
    else
        throw ParserError("XML document must contain main element!", token.line, token.charNo);
}
//*********************************************************************************************************************
ElementP XmlParser::elementRule()
{
    if (token.type == XmlLexer::closingTagStart)
        return nullptr;

    if (token.type == XmlLexer::tagStart)
    {
        token = lexer.readOther();
        auto elem = tagInteriorRule();

        if (token.type == XmlLexer::emptyTagEnd)
        {
            token = lexer.readString();
        }
        else if(token.type == XmlLexer::tagEnd)
        {
            token = lexer.readString();
            stack.push(elem->getName());
            ElementP nested;
            while((nested = elementRule()) != nullptr)
                elem->addElement(nested);
            closingTagRule();
        }
        else
            throw ParserError("'/>' or '>' expected", token.line, token.charNo);

        return elem;
    }
    else if (token.type == XmlLexer::string)
    {
        auto stringElem = ElementP(new Element(token.content));
        token = lexer.readOther();
        return stringElem;
    }
    else
        throw ParserError("'<' or string expected", token.line, token.charNo);
}
//*********************************************************************************************************************
ElementP XmlParser::tagInteriorRule()
{
    auto elem = keyNameRule();
    if (elem == nullptr)
        throw ParserError("tag name expected", token.line, token.charNo);
    ElementP attr;
    while ((attr = attributeRule()) != nullptr)
    {
        elem->addElement(attr);
    }
    return elem;
}
//*********************************************************************************************************************
ElementP XmlParser::attributeRule()
{
    auto key = keyNameRule();
    if (key == nullptr)
        return nullptr;

    auto value = valueNameRule();
    if (value != nullptr)
        key->addElement(value);
    return key;
}
//*********************************************************************************************************************
ElementP XmlParser::keyNameRule()
{
    if (token.type == XmlLexer::key)
    {
        auto elem = ElementP(new Element());
        elem->setName(token.content);
        token = lexer.readOther();
        return elem;
    }
    return nullptr;
}
//*********************************************************************************************************************
ElementP XmlParser::attributeNameRule()
{
    if (token.type == XmlLexer::key)
    {
        auto elem = ElementP(new Element());
        elem->setName("@" + token.content);
        token = lexer.readOther();
        return elem;
    }
    return nullptr;
}
//*********************************************************************************************************************
ElementP XmlParser::valueNameRule()
{
    if (token.type == XmlLexer::value)
    {
        auto elem = ElementP(new Element());
        elem->setName(token.content);
        token = lexer.readOther();
        return elem;
    }
    return nullptr;
}
//*********************************************************************************************************************
void XmlParser::closingTagRule()
{
    if (token.type != XmlLexer::closingTagStart)
        throw ParserError("'</' expected", token.line, token.charNo);
    token = lexer.readOther();

    if(token.type != XmlLexer::key)
        throw ParserError("key expected", token.line, token.charNo);
    if(token.content != stack.top())
        throw ParserError("unexpected closing tag", token.line, token.charNo);
    stack.pop();
    token = lexer.readOther();

    if (token.type != XmlLexer::tagEnd)
        throw ParserError("'>' expected", token.line, token.charNo);

    token = lexer.readString();
}
//*********************************************************************************************************************
