#include "stdafx.h"
#include "ContextMenuLog4cplus.h"
#include "SGIItemLog4cplus"
#include <sgi/helpers/string>
#include "MenuActionLog4cplus.h"

namespace sgi {

namespace log4cplus_plugin {

CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectLogger)
CONTEXT_MENU_POPULATE_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectAppender)

bool contextMenuPopulateImpl<Log4cplusObjectLogger>::populate(IContextMenuItem * menuItem)
{
    Log4cplusObjectLogger * object_ptr = static_cast<Log4cplusObjectLogger*>(item<SGIItemLog4cplus>()->object());
    log4cplus::Logger object = object_ptr->object();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            menuItem->addSimpleAction(MenuActionObjectInfo, "Object info...", _item);

            log4cplus::LogLevel logLevel = object.getLogLevel();
            IContextMenuItem * logLevelMenu = menuItem->addModeMenu(MenuActionLoggerLogLevel, "Log level", _item, logLevel);
            if(logLevelMenu)
            {
                logLevelMenu->addModeAction("Fatal", log4cplus::FATAL_LOG_LEVEL);
                logLevelMenu->addModeAction("Error", log4cplus::ERROR_LOG_LEVEL);
                logLevelMenu->addModeAction("Warning", log4cplus::WARN_LOG_LEVEL);
                logLevelMenu->addModeAction("Info", log4cplus::INFO_LOG_LEVEL);
                logLevelMenu->addModeAction("Debug", log4cplus::DEBUG_LOG_LEVEL);
                logLevelMenu->addModeAction("All", log4cplus::ALL_LOG_LEVEL);
                logLevelMenu->addModeAction("Not set", log4cplus::NOT_SET_LOG_LEVEL);
            }

            std::string currentLoggerName = object.getName();
            if(currentLoggerName != "root")
            {
                log4cplus::Logger parentLogger = object.getParent();
                SGIHostItemLog4cplus parent(new Log4cplusObjectLogger(parentLogger));
                if(parent.hasObject())
                    menuItem->addMenu("Parent", &parent);
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
                menuItem->addMenu(helpers::str_plus_count("Childs", numChilds), cloneItem<SGIItemLog4cplus>(SGIItemTypeChilds));

            SGIHostItemLog4cplus hierarchy(new Log4cplusObjectHierarchy(object.getHierarchy()));
            if(hierarchy.hasObject())
                menuItem->addMenu("Hierarchy", &hierarchy);

            log4cplus::SharedAppenderPtrList allAppenders = object.getAllAppenders();
            if(!allAppenders.empty())
                menuItem->addMenu(helpers::str_plus_count("Appenders", allAppenders.size()), cloneItem<SGIItemLog4cplus>(SGIItemTypeAppenders));
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
                    menuItem->addMenu(childName, &childItem);
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
                menuItem->addMenu(childName, &childItem);
            }
            ret = true;
        }
        break;
    }
    return ret;
}

bool contextMenuPopulateImpl<Log4cplusObjectAppender>::populate(IContextMenuItem * menuItem)
{
    Log4cplusObjectAppender * object_ptr = static_cast<Log4cplusObjectAppender*>(item<SGIItemLog4cplus>()->object());
    log4cplus::SharedAppenderPtr object = object_ptr->object();
    bool ret = false;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            menuItem->addSimpleAction(MenuActionObjectInfo, "Object info...", _item);
            ret = true;
        }
        break;
    }
    return ret;
}

} // namespace log4cplus_plugin
} // namespace sgi
