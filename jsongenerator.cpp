#include "jsongenerator.h"
//*********************************************************************************************************************
void JsonGenerator::loadSettings()
{

}
//*********************************************************************************************************************
UnicodeString JsonGenerator::getJson()
{
    loadSettings();
    if (dom->getChildren().size() == 1 && dom->getChildren()[0]->getChildren().size() == 0)
    {
        UnicodeString builder;
        builder.append("{\"");
        builder.append(dom->getChildren()[0]->getName());
        builder.append("\":null}");
        return builder;
    }
    return generateJson(dom);
}
//*********************************************************************************************************************
UnicodeString JsonGenerator::generateJson(ElementP root)
{
    UnicodeString builder;

    if (root->getChildren().size() == 0)
    {
        builder.append("null");
    }
    else if (root->getChildren().size() == 1 && root->getChildren()[0]->getChildren().size() == 0)
    {
        builder.append('"');
        builder.append(root->getChildren()[0]->getName());
        builder.append('"');
    }
    else
    {

        builder.append("{");

        ElementP child;
        while (root->getChildren().size() > 0)
        {
            child = root->getChildren()[0];
            builder.append('"');
            builder.append(child->getName());
            builder.append("\":");

            if (root->getChildrenLikeFirst().size() == 1)
            {
                builder.append(generateJson(child));
                root->eraseIndexesLikeFirst();
            }

            else
            {
                builder.append("[");
                for (auto child : root->getChildrenLikeFirst() )
                {
                    builder.append(generateJson(child));
                    builder.append(',');
                }

                root->eraseIndexesLikeFirst();

                builder.remove(builder.length() - 1, 1);
                builder.append("]");
            }
            builder.append(',');

        }

        builder.remove(builder.length() - 1, 1);
        builder.append("}");
        }

    return builder;
}
//*********************************************************************************************************************
