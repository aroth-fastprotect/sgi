// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <sgi/plugins/SGIItemBase.h>

namespace sgi {
// some method implementations from SGIItemBase which are only
// used within the SGI base and not in any plugin or caller.

void SGIItemBase::setPluginInfo(const ISGIPluginInfo * pluginInfo)
{
    SGIItemBasePtr item = this;
    while(item.valid())
    {
        item->_pluginInfo = pluginInfo;
        item = item->_next;
    }
}

/// @brief determines the length of the list
/// @return length of the list; zero if this item is the end of the list
size_t SGIItemBase::listSize() const
{
    size_t ret = 0;
    SGIItemBase * next = nextBase();
    while(next)
    {
        ret++;
        next = next->nextBase();
    }
    return ret;
}

/// @brief inserts the given item (sorted by the score of the item)
/// @param item pointer to item to be inserted into the list
/// @return pointer to the head of the list (with the highest score)
void SGIItemBase::insertByScore(SGIItemBase * item, SGIItemBasePtr & front)
{
    front = this;
    SGIItemBasePtr itemToInsert = item;
    while(itemToInsert.valid())
    {
        SGIItemBasePtr nextItemToInsert = itemToInsert->_next;
        // drop the reference to it's predecessor to avoid circles
        if(nextItemToInsert.valid())
            nextItemToInsert->_prev = NULL;
        itemToInsert->_next = NULL;

        SGIItemBasePtr insertionPoint = front;
        while(insertionPoint->_score >= itemToInsert->_score)
        {
            if(insertionPoint->_score == itemToInsert->_score)
            {
                if(insertionPoint->_pluginInfo->pluginScore() > itemToInsert->_pluginInfo->pluginScore())
                {
                    if(insertionPoint->_next.valid())
                        insertionPoint = insertionPoint->_next;
                    else
                        break;
                }
                else
                    break;
            }
            else
            {
                if(insertionPoint->_next.valid())
                    insertionPoint = insertionPoint->_next;
                else
                    break;
            }
        }
        if(itemToInsert->_score == insertionPoint->_score)
        {
            if(itemToInsert->_pluginInfo->pluginScore() > insertionPoint->_pluginInfo->pluginScore())
            {
                insertionPoint->insertBefore(itemToInsert);
                if(insertionPoint == front)
                    front = itemToInsert;
            }
            else
                insertionPoint->insertAfter(itemToInsert);
        }
        else if(itemToInsert->_score > insertionPoint->_score)
        {
            insertionPoint->insertBefore(itemToInsert);
            if(insertionPoint == front)
                front = itemToInsert;
        }
        else
            insertionPoint->insertAfter(itemToInsert);
        itemToInsert = nextItemToInsert;
    }
}

bool SGIItemBase::isListValid() const
{
    bool ret = true;
    std::set<const SGIItemBase*> foundItems;
    const SGIItemBase * cur = this;
    while(cur)
    {
        if(foundItems.find(cur) != foundItems.end())
        {
            // found re-occuring item
            ret = false;
        }
        foundItems.insert(cur);
        const SGIItemBase * next = cur->_next.get();
        if(next)
        {
            if(!(next->score() <= cur->score()))
            {
                int vvv = 0;
                ret = false;
            }
        }
        cur = next;
    }
    return ret;
}

}
