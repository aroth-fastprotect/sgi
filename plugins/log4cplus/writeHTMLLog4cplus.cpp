#include "stdafx.h"
#include <ostream>
#include <log4cplus/hierarchy.h>
#include "writeHTMLLog4cplus.h"
#include "SGIItemLog4cplus"

#include <sgi/helpers/rtti>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace log4cplus_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectBase)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectLogger)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectHierarchy)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectAppender)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectLayout)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectSimpleLayout)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(Log4cplusObjectPatternLayout)

bool writePrettyHTMLImpl<Log4cplusObjectBase>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    Log4cplusObjectBase * object_ptr = static_cast<Log4cplusObjectBase*>(item<SGIItemLog4cplus>()->object());
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>type</td><td>" << helpers::getRTTITypename_html(object_ptr) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<Log4cplusObjectLogger>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    Log4cplusObjectLogger * object_ptr = static_cast<Log4cplusObjectLogger*>(item<SGIItemLog4cplus>()->object());
    log4cplus::Logger object = object_ptr->object();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Log4cplusObjectBase properties first
            callNextHandler(os);

            log4cplus::LogLevelManager& llmgr = log4cplus::getLogLevelManager();

            os << "<tr><td>name</td><td>" << object.getName() << "</td></tr>" << std::endl;
            if(!object_ptr->isRoot())
                os << "<tr><td>parent name</td><td>" << object.getParent().getName() << "</td></tr>" << std::endl;
            os << "<tr><td>chainedLogLevel</td><td>" << llmgr.toString(object.getChainedLogLevel()) << "</td></tr>" << std::endl;
            os << "<tr><td>logLevel</td><td>" << llmgr.toString(object.getLogLevel()) << "</td></tr>" << std::endl;
            os << "<tr><td>additivity</td><td>" << (object.getAdditivity()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>log levels</td><td><ol>";
            os << "<li>OFF&nbsp;=&nbsp;" << (object.isEnabledFor(log4cplus::OFF_LOG_LEVEL)?"enabled":"disabled") << "</li>" << std::endl;
            os << "<li>FATAL&nbsp;=&nbsp;" << (object.isEnabledFor(log4cplus::FATAL_LOG_LEVEL)?"enabled":"disabled") << "</li>" << std::endl;
            os << "<li>ERROR&nbsp;=&nbsp;" << (object.isEnabledFor(log4cplus::ERROR_LOG_LEVEL)?"enabled":"disabled") << "</li>" << std::endl;
            os << "<li>WARN&nbsp;=&nbsp;" << (object.isEnabledFor(log4cplus::WARN_LOG_LEVEL)?"enabled":"disabled") << "</li>" << std::endl;
            os << "<li>INFO&nbsp;=&nbsp;" << (object.isEnabledFor(log4cplus::INFO_LOG_LEVEL)?"enabled":"disabled") << "</li>" << std::endl;
            os << "<li>DEBUG&nbsp;=&nbsp;" << (object.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL)?"enabled":"disabled") << "</li>" << std::endl;
            os << "<li>TRACE&nbsp;=&nbsp;" << (object.isEnabledFor(log4cplus::TRACE_LOG_LEVEL)?"enabled":"disabled") << "</li>" << std::endl;
            os << "</td></tr>" << std::endl;


            os << "<tr><td>appenders</td><td><ol>";
            log4cplus::SharedAppenderPtrList appenders = object.getAllAppenders();
            for(log4cplus::SharedAppenderPtrList::const_iterator it = appenders.begin(); it != appenders.end(); it++)
            {
                const log4cplus::SharedAppenderPtr & appender = *it;
                os << "<li>" << appender->getName() << "</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            std::string currentLoggerName = object.getName();
            os << "<ol>";
            log4cplus::LoggerList allLoggers = log4cplus::Logger::getCurrentLoggers();
            for(log4cplus::LoggerList::const_iterator it = allLoggers.begin(); it != allLoggers.end(); it++)
            {
                const log4cplus::Logger & child = *it;
                if(child.getName() != currentLoggerName && child.getParent().getName() == currentLoggerName)
                {
                    std::string childName = child.getName();
                    os << "<li>" << childName << "</li>";
                }
            }
            os << "</ol>";
            ret = true;
        }
        break;
    case SGIItemTypeParents:
        {
            os << "<ol>";
            if(!object_ptr->isRoot())
            {
                log4cplus::Logger parentLogger = object.getParent();
                std::string parentName = parentLogger.getName();
                os << "<li>" << parentName << "</li>";
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<Log4cplusObjectHierarchy>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    Log4cplusObjectHierarchy * object_ptr = static_cast<Log4cplusObjectHierarchy*>(item<SGIItemLog4cplus>()->object());
    log4cplus::Hierarchy & object = object_ptr->object();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Log4cplusObjectBase properties first
            callNextHandler(os);

            os << "<tr><td>log levels</td><td><ol>";
            os << "<li>OFF&nbsp;=&nbsp;" << (object.isDisabled(log4cplus::OFF_LOG_LEVEL)?"disabled":"enabled") << "</li>" << std::endl;
            os << "<li>FATAL&nbsp;=&nbsp;" << (object.isDisabled(log4cplus::FATAL_LOG_LEVEL)?"disabled":"enabled") << "</li>" << std::endl;
            os << "<li>ERROR&nbsp;=&nbsp;" << (object.isDisabled(log4cplus::ERROR_LOG_LEVEL)?"disabled":"enabled") << "</li>" << std::endl;
            os << "<li>WARN&nbsp;=&nbsp;" << (object.isDisabled(log4cplus::WARN_LOG_LEVEL)?"disabled":"enabled") << "</li>" << std::endl;
            os << "<li>INFO&nbsp;=&nbsp;" << (object.isDisabled(log4cplus::INFO_LOG_LEVEL)?"disabled":"enabled") << "</li>" << std::endl;
            os << "<li>DEBUG&nbsp;=&nbsp;" << (object.isDisabled(log4cplus::DEBUG_LOG_LEVEL)?"disabled":"enabled") << "</li>" << std::endl;
            os << "<li>TRACE&nbsp;=&nbsp;" << (object.isDisabled(log4cplus::TRACE_LOG_LEVEL)?"disabled":"enabled") << "</li>" << std::endl;
            os << "</td></tr>" << std::endl;

            os << "<tr><td>root</td><td>" << object.getRoot().getName() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeChilds:
        {
            os << "<ol>";
            log4cplus::LoggerList allLoggers = object.getCurrentLoggers();
            for(log4cplus::LoggerList::const_iterator it = allLoggers.begin(); it != allLoggers.end(); it++)
            {
                const log4cplus::Logger & child = *it;
                std::string childName = child.getName();
                os << "<li>" << childName << "</li>";
            }
            os << "</ol>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<Log4cplusObjectAppender>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    Log4cplusObjectAppender * object_ptr = static_cast<Log4cplusObjectAppender*>(item<SGIItemLog4cplus>()->object());
    const log4cplus::SharedAppenderPtr & object = object_ptr->object();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Log4cplusObjectBase properties first
            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>threshold</td><td>" << object->getThreshold() << "</td></tr>" << std::endl;
            os << "<tr><td>layout</td><td>" << object->getLayout() << "</td></tr>" << std::endl;
            os << "<tr><td>filter</td><td>" << object->getFilter() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<Log4cplusObjectLayout>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    Log4cplusObjectLayout * object_ptr = static_cast<Log4cplusObjectLayout*>(item<SGIItemLog4cplus>()->object());
    const log4cplus::Layout * object = object_ptr->object();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Log4cplusObjectBase properties first
            callNextHandler(os);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}


bool writePrettyHTMLImpl<Log4cplusObjectSimpleLayout>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    Log4cplusObjectSimpleLayout * object_ptr = static_cast<Log4cplusObjectSimpleLayout*>(item<SGIItemLog4cplus>()->object());
    const log4cplus::SimpleLayout * object = object_ptr->object();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Log4cplusObjectBase properties first
            callNextHandler(os);

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

namespace {
    class PatternLayoutAccess : public log4cplus::PatternLayout
    {
    public:
        const log4cplus::tstring & getPattern() const { return pattern; }
        typedef std::vector<log4cplus::pattern::PatternConverter*> PatternConverterVector;
        const PatternConverterVector & getParsedPattern() const { return parsedPattern; }
    };
}

bool writePrettyHTMLImpl<Log4cplusObjectPatternLayout>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    Log4cplusObjectPatternLayout * object_ptr = static_cast<Log4cplusObjectPatternLayout*>(item<SGIItemLog4cplus>()->object());
    const log4cplus::PatternLayout * object = object_ptr->object();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add Log4cplusObjectBase properties first
            callNextHandler(os);

            const PatternLayoutAccess * access = (const PatternLayoutAccess *)object;
            os << "<tr><td>pattern</td><td>" << access->getPattern() << "</td></tr>" << std::endl;
            os << "<tr><td>parsed pattern</td><td><ol>";
            const PatternLayoutAccess::PatternConverterVector & parsedPattern = access->getParsedPattern();
            for(auto it = parsedPattern.begin(); it != parsedPattern.end(); ++it)
            {
                const log4cplus::pattern::PatternConverter * conv = *it;
                os << "<li>" << conv << "</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;


            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

} // namespace log4cplus_plugin
} // namespace sgi
