#include "xmlparser.h"
#include "xmlerror.h"
#include <unicode/ustream.h>
#include <iostream>
#include <fstream>
#include <cstring>


void analyzeDom(std::shared_ptr<Element> root, int offset = 0);

//*********************************************************************************************************************
int main(int argc, char* argv[])
{
    std::istream *is = &std::cin;
    std::ifstream ifs;

    if (!(argc % 2))
    {
        std::cout << "Improper usage!" << std::endl;
        return -1;
    }
    for (int i = 1; i < argc; i += 2)
    {
        if (strcmp(argv[i], "--in") == 0)
        {
            ifs.open(argv[i+1]);
            if(!ifs)
            {
                std::cout << "Input file does not exist!" << std::endl;
                return -1;
            }
            is = &ifs;
        }
    }

    std::string input_string((std::istreambuf_iterator<char>(*is)), std::istreambuf_iterator<char>());
    UnicodeString input(input_string.c_str());

    try
    {
        XmlParser parser(input);

        std::shared_ptr<Element> dom = parser.getDom();

        analyzeDom(dom);
    }
    catch(XmlError &e)
    {
        std::cout << e.reasonStr() << std::endl;
    }
}
//*********************************************************************************************************************
void printOffset(int offset)
{
    for (int i = 0 ; i < offset ; ++i)
        std::cout << "|   ";
}
//*********************************************************************************************************************
void analyzeDom(ElementP root, int offset)
{
    printOffset(offset);
    std::cout << root->getName() << std::endl;

    for (auto child : root->getChildren())
        analyzeDom(child, offset+1);
}
//*********************************************************************************************************************
