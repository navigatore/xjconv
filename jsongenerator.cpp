#include "jsongenerator.h"
//*********************************************************************************************************************
void JsonGenerator::loadSettings()
{

}
//*********************************************************************************************************************
UnicodeString JsonGenerator::getJson()
{
    loadSettings();
    UnicodeString builder;
    builder.append("{");
    builder.append(generateJson(dom));
    builder.append("}");
    return builder;
}
//*********************************************************************************************************************
UnicodeString JsonGenerator::generateJson(ElementP root)
{
    UnicodeString builder;

    if (root->getChildren().size() == 0)
    {
        builder.append("null");
        return builder;
    }

    if (root->getChildren().size() == 1)
    {
        builder.append('"');
        builder.append(root->getChildren()[0]->getName());
        builder.append("\":");
        builder.append(generateJson(root->getChildren()[0]));
        root->popFirst();
        return builder;
    }

    builder.append("{");

    ElementP child;
    while (root->getChildren().size() > 0)
    {
        child = root->getChildren()[0];
        builder.append('"');
        builder.append(child->getName());
        builder.append("\":");

        if (root->getIndexesLikeFirst().size() == 1)
        {
            builder.append(generateJson(child));
            root->popFirst();
        }

        else
        {
            builder.append("[");
            for (auto index : root->getIndexesLikeFirst() )
            {
                builder.append(generateJson(child));
                root->pop(index);
                builder.append(',');
            }
            builder.remove(builder.length() - 1, 1);
            builder.append("]");
        }
        builder.append(',');

    }

    builder.remove(builder.length() - 1, 1);
    builder.append("}");

    return builder;
}
//*********************************************************************************************************************
