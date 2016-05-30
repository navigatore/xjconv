#ifndef SIMPLEELEMENT_H
#define SIMPLEELEMENT_H

#include "element.h"
#include <unicode/unistr.h>

class SimpleElement : public Element
{
public:
    SimpleElement(UnicodeString content): content(content) { }
    virtual bool complex() { return false; }
    UnicodeString getContent() { return content; }
private:
    UnicodeString content;
};

#endif // SIMPLEELEMENT_H
