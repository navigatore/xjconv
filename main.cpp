/*
 * TODO
 * wczytywanie configa
 * poprawienie new-engine
 * super wypaśne opcje generacji - np. ten myk z zamianą nazwy
 * refaktor
 * dokumentacja
 */

#include "xmlparser.h"
#include "xmlerror.h"
#include "jsongenerator.h"
#include <unicode/ustream.h>
#include <iostream>
#include <fstream>
#include <cstring>

void analyzeDom(std::shared_ptr<Element> root, int offset = 0);

//*********************************************************************************************************************
int main(int argc, char* argv[])
{
    std::istream *is = &std::cin;
    std::ostream *os = &std::cout;

    std::ifstream ifs;
    std::ifstream cfs;
    std::ofstream ofs;

    bool default_cfg = true;

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
        else if (strcmp(argv[i], "--config") == 0)
        {
            cfs.open(argv[i+1]);
            if(!cfs)
            {
                std::cout << "Config file does not exist!" << std::endl;
                return -1;
            }
            default_cfg = false;
        }
        else if (strcmp(argv[i], "--out") == 0)
        {
            ofs.open(argv[i+1]);
            if(!ofs)
            {
                std::cout << "Can't open output file!" << std::endl;
                return -1;
            }
            os = &ofs;
        }
    }

    if (default_cfg)
    {
        cfs.open("xjconvert.conf");
        if(!cfs)
        {
            std::cout << "Can't open config file!" << std::endl;
            return -1;
        }

    }

    std::string input_string((std::istreambuf_iterator<char>(*is)), std::istreambuf_iterator<char>());
    UnicodeString input(input_string.c_str());

    std::string config_string((std::istreambuf_iterator<char>(cfs)), std::istreambuf_iterator<char>());
    UnicodeString config(config_string.c_str());

    try
    {
        std::cout << "Input XML file:" << std::endl;
        XmlParser parser(input);

        auto dom = parser.getDom();

        analyzeDom(dom);

        std::cout << "\nConverter config file:" << std::endl;
        XmlParser configParser(config);

        auto configTree = configParser.getDom();

        analyzeDom(configTree);

        auto generator = JsonGenerator(dom, configTree);
        auto json = generator.getJson();


        (*os) << json << std::endl;
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
