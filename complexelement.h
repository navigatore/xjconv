#ifndef COMPLEXELEMENT_H
#define COMPLEXELEMENT_H

#include "element.h"
#include <unicode/unistr.h>
#include <memory>
#include <vector>

class ComplexElement : public Element
{
public:
    ComplexElement(UnicodeString name = "", UnicodeString nameAfterMap = "") : name(name), nameAfterMap(nameAfterMap) { }

    UnicodeString getName() { return name; }
    UnicodeString getNameAfterMap() { return name; }
    std::vector<std::shared_ptr<Element>> getChildren() { return children; }

    void setName(UnicodeString name) { this->name = name; }
    void setNameAfterMap(UnicodeString name) { this->nameAfterMap = name; }

    void addElement(std::shared_ptr<Element> element) { children.push_back(element); }

    virtual bool complex() { return true; }

private:
    UnicodeString name, nameAfterMap;
    std::vector<std::shared_ptr<Element>> children;
};

#endif // COMPLEXELEMENT_H
