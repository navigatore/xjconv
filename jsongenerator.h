#ifndef JSONGENERATOR_H
#define JSONGENERATOR_H

#include "element.h"

class JsonGenerator
{
public:
    JsonGenerator(ElementP dom, ElementP config) : dom(dom), config(config) { }
    UnicodeString getJson();

private:
    void loadSettings();  // TODO load possible settings
    UnicodeString generateJson(ElementP root);

    ElementP dom, config;
};

#endif // JSONGENERATOR_H
