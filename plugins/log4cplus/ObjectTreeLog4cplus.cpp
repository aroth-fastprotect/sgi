#include "stdafx.h"
#include "ObjectTreeLog4cplus.h"
#include "SGIItemLog4cplus"

#include <sgi/SGIItemInternal>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIProxyItem.h>
#include <sgi/plugins/SGIHostItemOsg.h>
#include <sgi/helpers/string>
#include <log4cplus/hierarchy.h>

namespace sgi {
namespace log4cplus_plugin {

OBJECT_TREE_BUILD_IMPL_REGISTER(Log4cplusObjectLogger)
OBJECT_TREE_BUILD_IMPL_REGISTER(Log4cplusObjectHierarchy)
OBJECT_TREE_BUILD_IMPL_REGISTER(Log4cplusObjectAppender)
OBJECT_TREE_BUILD_IMPL_REGISTER(Log4cplusObjectLayout)
OBJECT_TREE_BUILD_IMPL_REGISTER(Log4cplusObjectSimpleLayout)
OBJECT_TREE_BUILD_IMPL_REGISTER(Log4cplusObjectPatternLayout)

bool objectTreeBuildImpl<Log4cplusObjectLogger>::build(IObjectTreeItem * treeItem)
{
    Log4cplusObjectLogger * object_ptr = static_cast<Log4cplusObjectLogger*>(item<SGIItemLog4cplus>()->object());
    log4cplus::Logger object = object_ptr->object();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            std::string currentLoggerName = object.getName();
            if(!object_ptr->isRoot())
            {
                log4cplus::Logger parentLogger = object.getParent();
                SGIHostItemLog4cplus parent(new Log4cplusObjectLogger(parentLogger));
                if(parent.hasObject())
                    treeItem->addChild("Parent", &parent);
            }

            unsigned numChilds = 0;
            {
                log4cplus::LoggerList allLoggers = log4cplus::Logger::getCurrentLoggers();
                for(log4cplus::LoggerList::const_iterator it = allLoggers.begin(); it != allLoggers.end(); it++)
                {
                    const log4cplus::Logger & child = *it;
                    if(child.getName() != currentLoggerName && child.getParent().getName() == currentLoggerName)
                        numChilds++;
                }
            }

            if(numChilds)
                treeItem->addChild(helpers::str_plus_count("Childs", numChilds), cloneItem<SGIItemLog4cplus>(SGIItemTypeChilds));

            SGIHostItemLog4cplus hierarchy(new Log4cplusObjectHierarchy(object.getHierarchy()));
            if(hierarchy.hasObject())
                treeItem->addChild("Hierarchy", &hierarchy);

            log4cplus::SharedAppenderPtrList allAppenders = object.getAllAppenders();
            if(!allAppenders.empty())
                treeItem->addChild(helpers::str_plus_count("Appenders", allAppenders.size()), cloneItem<SGIItemLog4cplus>(SGIItemTypeAppenders));

            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            std::string currentLoggerName = object.getName();
            log4cplus::LoggerList allLoggers = log4cplus::Logger::getCurrentLoggers();
            for(log4cplus::LoggerList::const_iterator it = allLoggers.begin(); it != allLoggers.end(); it++)
            {
                const log4cplus::Logger & child = *it;
                if(child.getName() != currentLoggerName && child.getParent().getName() == currentLoggerName)
                {
                    SGIHostItemLog4cplus childItem(new Log4cplusObjectLogger(child));
                    std::string childName = child.getName();
                    treeItem->addChild(childName, &childItem);
                }
            }
            ret = true;
        }
        break;
    case SGIItemTypeAppenders:
        {
            log4cplus::SharedAppenderPtrList allAppenders = object.getAllAppenders();
            for(log4cplus::SharedAppenderPtrList::const_iterator it = allAppenders.begin(); it != allAppenders.end(); it++)
            {
                const log4cplus::SharedAppenderPtr & child = *it;
                SGIHostItemLog4cplus childItem(new Log4cplusObjectAppender(child));
                std::string childName = child->getName();
                treeItem->addChild(childName, &childItem);
            }
            ret = true;
        }
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<Log4cplusObjectHierarchy>::build(IObjectTreeItem * treeItem)
{
    Log4cplusObjectHierarchy * object_ptr = static_cast<Log4cplusObjectHierarchy*>(item<SGIItemLog4cplus>()->object());
    log4cplus::Hierarchy & object = object_ptr->object();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            unsigned numChilds = 0;
            {
                log4cplus::LoggerList allLoggers = object.getCurrentLoggers();
                numChilds = allLoggers.size();
            }

            if(numChilds)
                treeItem->addChild(helpers::str_plus_count("Childs", numChilds), cloneItem<SGIItemLog4cplus>(SGIItemTypeChilds));

            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            log4cplus::LoggerList allLoggers = object.getCurrentLoggers();
            for(log4cplus::LoggerList::const_iterator it = allLoggers.begin(); it != allLoggers.end(); it++)
            {
                const log4cplus::Logger & child = *it;
                SGIHostItemLog4cplus childItem(new Log4cplusObjectLogger(child));
                std::string childName = child.getName();
                treeItem->addChild(childName, &childItem);
            }
            ret = true;
        }
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<Log4cplusObjectAppender>::build(IObjectTreeItem * treeItem)
{
    Log4cplusObjectAppender * object_ptr = static_cast<Log4cplusObjectAppender*>(item<SGIItemLog4cplus>()->object());
    log4cplus::SharedAppenderPtr object = object_ptr->object();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            log4cplus::Layout * object_layout = object->getLayout();
            Log4cplusObjectLayout * layout_ptr = NULL;
            if(log4cplus::SimpleLayout* simple = dynamic_cast<log4cplus::SimpleLayout*>(object_layout))
                layout_ptr = new Log4cplusObjectSimpleLayout(simple);
            else if(log4cplus::PatternLayout * pattern = dynamic_cast<log4cplus::PatternLayout*>(object_layout))
                layout_ptr = new Log4cplusObjectPatternLayout(pattern);
            SGIHostItemLog4cplus layout(layout_ptr);
            if(layout.hasObject())
                treeItem->addChild("Layout", &layout);
            ret = true;
        }
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<Log4cplusObjectLayout>::build(IObjectTreeItem * treeItem)
{
    Log4cplusObjectLayout * object_ptr = static_cast<Log4cplusObjectLayout*>(item<SGIItemLog4cplus>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            ret = true;
        }
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<Log4cplusObjectSimpleLayout>::build(IObjectTreeItem * treeItem)
{
    Log4cplusObjectSimpleLayout * object_ptr = static_cast<Log4cplusObjectSimpleLayout*>(item<SGIItemLog4cplus>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            ret = true;
        }
        break;
    }
    return ret;
}

bool objectTreeBuildImpl<Log4cplusObjectPatternLayout>::build(IObjectTreeItem * treeItem)
{
    Log4cplusObjectPatternLayout * object_ptr = static_cast<Log4cplusObjectPatternLayout*>(item<SGIItemLog4cplus>()->object());
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            ret = true;
        }
        break;
    }
    return ret;
}

OBJECT_TREE_BUILD_ROOT_IMPL_REGISTER(ISceneGraphDialog)

struct LoggerRootSingleton
{
    SGIItemBase * operator()(SGIPluginHostInterface * hostInterface)
    {
        SGIItemBasePtr item;
        log4cplus::Logger rootLogger = log4cplus::Logger::getRoot();
        SGIHostItemLog4cplus hostItemLogger(new log4cplus_plugin::Log4cplusObjectLogger(rootLogger));
        hostInterface->generateItem(item, &hostItemLogger);
        return item.release();
    }
};

bool objectTreeBuildRootImpl<ISceneGraphDialog>::build(IObjectTreeItem * treeItem)
{
    ISceneGraphDialog * object = static_cast<ISceneGraphDialog*>(item<SGIItemInternal>()->object());

    SGIHostItemOsg hostItem(new SGIProxyItemT<LoggerRootSingleton>(_hostInterface, "log4cplus::RootLogger"));
    treeItem->addChild(std::string(), &hostItem);
    return true;
}

} // namespace log4cplus_plugin
} // namespace sgi
