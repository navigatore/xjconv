#ifndef ELEMENT_H
#define ELEMENT_H

#include <memory>
#include <vector>
#include <unicode/unistr.h>

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
    void popFirst() { children.erase(children.begin()); }
    void pop(int index) { children.erase(children.begin() + index); }
    std::vector<int> getIndexesLikeFirst()
    {
        std::vector<int> indexes(0);
        UnicodeString name = children[0]->name;
        for (unsigned int i = 1 ; i < children.size() ; ++i)
            if (children[i]->name == name)
                indexes.push_back(i);
        return indexes;
    }

private:
    UnicodeString name, nameAfterMap;
    std::vector<std::shared_ptr<Element>> children;
};

typedef std::shared_ptr<Element> ElementP;

#endif // ELEMENT_H
