#ifndef ELEMENT_H
#define ELEMENT_H

#include <memory>
#include <vector>

class Element
{
public:
    Element(UnicodeString name = "", UnicodeString nameAfterMap = "") : name(name), nameAfterMap(nameAfterMap) { }


    UnicodeString getName() { return name; }
    UnicodeString getNameAfterMap() { return name; }
    std::vector<std::shared_ptr<Element>> getChildren() { return children; }

    void setName(UnicodeString name) { this->name = name; }
    void setNameAfterMap(UnicodeString name) { this->nameAfterMap = name; }

    void addElement(std::shared_ptr<Element> element) { children.push_back(element); }

private:
    UnicodeString name, nameAfterMap;
    std::vector<std::shared_ptr<Element>> children;
};

typedef std::shared_ptr<Element> ElementP;

#endif // ELEMENT_H
