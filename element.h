#ifndef ELEMENT_H
#define ELEMENT_H

#include <memory>
#include <vector>
#include <unicode/unistr.h>

class Element;
typedef std::shared_ptr<Element> ElementP;

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
    void eraseIndexesLikeFirst()
    {
        auto firstChildName = children[0]->name;
        for (auto it = children.begin() + 1 ; it != children.end() ; )
        {
            if ((*it)->name == firstChildName)
                it = children.erase(it);
            else
                ++it;
        }
        children.erase(children.begin());
    }
    void pop(int index) { children.erase(children.begin() + index); }

    std::vector<ElementP> getChildrenLikeFirst()
    {
        std::vector<ElementP> matchingChildren;
        UnicodeString name = children[0]->name;
        for (auto child : children)
            if (child->name == name)
                matchingChildren.push_back(child);
        return matchingChildren;
    }

private:
    UnicodeString name, nameAfterMap;
    std::vector<ElementP> children;
};


#endif // ELEMENT_H
