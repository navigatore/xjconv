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
#include "xmlparser.h"
#include <memory>
#include "element.h"

using namespace std;

void analyzeDom(std::shared_ptr<Element> root, int offset = 0);

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

    XmlParser parser(input);

    std::shared_ptr<Element> dom = parser.getDom();

    analyzeDom(dom);
}
//*********************************************************************************************************************
void printOffset(int offset)
{
    for (int i = 0 ; i < offset ; ++i)
        cout << "|   ";
}

void analyzeDom(ElementP root, int offset)
{
    printOffset(offset);
    cout << root->getName() << endl;

    for (auto child : root->getChildren())
        analyzeDom(child, offset+1);
}
//*********************************************************************************************************************
