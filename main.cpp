/*
 * TODO
 *
 * Refactor
 *
*/
#include <unicode/unistr.h>
#include <unicode/ustring.h>
#include <unicode/ustream.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <iterator>
#include "xmllexer.h"

using namespace std;

void printTokenInfo(XmlLexer::Token token);

//*********************************************************************************************************************
int main(int argc, char* argv[])
{
    istream *is = &cin;
    ifstream ifs;

    if (!(argc % 2))
    {
        cout << "Improper usage!" << endl;
        return -1;
    }
    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "--in") == 0)
        {
            ifs.open(argv[i+1]);
            if(!ifs)
            {
                cout << "Input file does not exist!" << endl;
                return -1;
            }
            is = &ifs;
        }
    }


    string input_string((std::istreambuf_iterator<char>(*is)), std::istreambuf_iterator<char>());
    UnicodeString input(input_string.c_str());

    XmlLexer lexer(input);


    // PARSER MOCK
    int singleQuoteOpened = 0, doubleQuoteOpened = 0;
    XmlLexer::TokenType last = XmlLexer::key;
    XmlLexer::Token token;

    while(true)
    {
        if (singleQuoteOpened == 1)
        {
            token = lexer.read(XmlLexer::value);
            singleQuoteOpened++;
        }
        else if (doubleQuoteOpened == 1)
        {
            token = lexer.read(XmlLexer::value);
            doubleQuoteOpened++;
        }
        else if (last == XmlLexer::tagEnd || last == XmlLexer::emptyTagEnd || last == XmlLexer::xmlTagClose || last == XmlLexer::hexChar || last == XmlLexer::specialChar)
        {
            token = lexer.read(XmlLexer::string);
        }
        else if (last == XmlLexer::startComment)
        {
            token = lexer.read(XmlLexer::comment);
        }
        else if (last == XmlLexer::hex)
        {
            token = lexer.read(XmlLexer::hexChar);
        }
        else if (last == XmlLexer::special)
        {
            token = lexer.read(XmlLexer::specialChar);
        }
        else
        {
            token = lexer.read(XmlLexer::key);
        }

        last = token.type;
        if (last == XmlLexer::singleQuote)
        {
            singleQuoteOpened++;
            if (singleQuoteOpened == 3)
                singleQuoteOpened = 0;
        }
        else if (last == XmlLexer::doubleQuote)
        {
            doubleQuoteOpened++;
            if (doubleQuoteOpened == 3)
                doubleQuoteOpened = 0;
        }

        printTokenInfo(token);
        if (token.type == XmlLexer::endOfFile)
            break;
    };
}
//*********************************************************************************************************************
void printTokenInfo(XmlLexer::Token token)
{
    if (token.type == XmlLexer::endOfFile)
    {
        cout << "EOF\t\t|\t" << endl;
    }
    else if (token.type == XmlLexer::tagStart)
    {
        cout << "Start of tag\t|\t<" << endl;
    }
    else if (token.type == XmlLexer::startComment)
    {
        cout << "Start Comment\t|\t<!--" << endl;
    }
    else if (token.type == XmlLexer::endComment)
    {
        cout << "End Comment\t|\t-->" << endl;
    }
    else if (token.type == XmlLexer::closingTagStart)
    {
        cout << "Clos. tag start\t|\t</" << endl;
    }
    else if (token.type == XmlLexer::tagEnd)
    {
        cout << "End of tag\t|\t>" << endl;
    }
    else if (token.type == XmlLexer::emptyTagEnd)
    {
        cout << "Empty tag end\t|\t/>" << endl;
    }
    else if (token.type == XmlLexer::xmlTagOpen)
    {
        cout << "XML opening tag\t|\t<?" << endl;
    }
    else if (token.type == XmlLexer::xmlTagClose)
    {
        cout << "XML closing tag\t|\t?>" << endl;
    }
    else if (token.type == XmlLexer::doctypeTagOpen)
    {
        cout << "DOCTYPE opening tag\t|\t<!" << endl;
    }
    else if (token.type == XmlLexer::equals)
    {
        cout << "Equals tag\t|\t=" << endl;
    }
    else if (token.type == XmlLexer::special)
    {
        cout << "Spec. char tag\t|\t&" << endl;
    }
    else if (token.type == XmlLexer::hex)
    {
        cout << "Hex escape\t|\t&#" << endl;
    }
    else if (token.type == XmlLexer::specialChar)
    {
        cout << "Special value\t|\t[" << token.content<< "]" << endl;
    }
    else if (token.type == XmlLexer::hexChar)
    {
        cout << "Hex value\t|\t[" << token.content << "]" << endl;
    }
    else if (token.type == XmlLexer::string)
    {
        cout << "String token\t|\t[" << token.content << "]" << endl;
    }
    else if (token.type == XmlLexer::key)
    {
        cout << "Key token\t|\t[" << token.content << "]" << endl;
    }
    else if (token.type == XmlLexer::value)
    {
        cout << "Value token\t|\t[" << token.content << "]" << endl;
    }
    else if (token.type == XmlLexer::singleQuote)
    {
        cout << "Single quote\t|\t'" << endl;
    }
    else if (token.type == XmlLexer::doubleQuote)
    {
        cout << "Double quote\t|\t\"" << endl;
    }
    else if (token.type == XmlLexer::comment)
    {
        cout << "Comment   \t|\t[" << token.content << "]" << endl;
    }
    else
    {
        cout << "Token not known!" << endl;
    }
}
//*********************************************************************************************************************
