// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#pragma once
#include "SGIHostItemBase.h"
#include <log4cplus/logger.h>

namespace osg {
    class Referenced;
}

namespace sgi {
namespace log4cplus_plugin {
class Log4cplusObjectBase : public osg::Referenced
{
public:
    Log4cplusObjectBase() {}
    Log4cplusObjectBase(const Log4cplusObjectBase & rhs) {}
    virtual ~Log4cplusObjectBase() {}

    bool operator == (const Log4cplusObjectBase & rhs) const { return true; }
    bool operator != (const Log4cplusObjectBase & rhs) const { return false; }
    bool operator < (const Log4cplusObjectBase & rhs) const { return false; }
    Log4cplusObjectBase & operator = (const Log4cplusObjectBase & rhs) { return *this; }
};

template<typename LOGPLUSTYPE, typename LOGPLUSTYPE_STORAGETYPE=LOGPLUSTYPE*, typename LOGPLUSTYPE_REFTYPE=LOGPLUSTYPE*>
class Log4cplusObjectT : public Log4cplusObjectBase
{
public:
    typedef LOGPLUSTYPE ObjectType;
    typedef LOGPLUSTYPE_REFTYPE ObjectRefType;
    typedef LOGPLUSTYPE_STORAGETYPE ObjectStorageType;
    Log4cplusObjectT(ObjectRefType object) : _object(object) {}
    Log4cplusObjectT(const Log4cplusObjectT<LOGPLUSTYPE, LOGPLUSTYPE_STORAGETYPE> & rhs) : _object(rhs._object) {}
    virtual ~Log4cplusObjectT() {}

    ObjectRefType object() { return _object; }

protected:
    ObjectStorageType _object;
};

class Log4cplusLoggerCompare : public log4cplus::Logger
{
public:
    bool equal(const log4cplus::Logger & logger) const { return value == ((const Log4cplusLoggerCompare&)logger).value; }
};

class Log4cplusObjectLogger : public Log4cplusObjectBase
{
public:
    typedef log4cplus::Logger ObjectType;
    Log4cplusObjectLogger(const ObjectType & object)
        : _loggerName()
        {
            const Log4cplusLoggerCompare & compareObject = (const Log4cplusLoggerCompare&)object;
            if(!compareObject.equal(log4cplus::Logger::getRoot()))
                _loggerName = object.getName();
        }
    Log4cplusObjectLogger(const Log4cplusObjectLogger & rhs) : _loggerName(rhs._loggerName) {}
    virtual ~Log4cplusObjectLogger() {}

    ObjectType object() { return log4cplus::Logger::getInstance(_loggerName); }
    bool isRoot() const { return _loggerName.empty(); }
    const std::string & name() const { return _loggerName; }

protected:
    std::string _loggerName;
};

class Log4cplusObjectAppender : public Log4cplusObjectBase
{
public:
    typedef log4cplus::SharedAppenderPtr ObjectType;
    Log4cplusObjectAppender(const ObjectType & object) : _object(object) {}
    Log4cplusObjectAppender(const Log4cplusObjectAppender & rhs) : _object(rhs._object) {}
    virtual ~Log4cplusObjectAppender() {}

    ObjectType object() { return _object; }

protected:
    ObjectType _object;
};

typedef Log4cplusObjectT<log4cplus::Hierarchy, log4cplus::Hierarchy&, log4cplus::Hierarchy&> Log4cplusObjectHierarchy;
typedef Log4cplusObjectT<log4cplus::Layout> Log4cplusObjectLayout;

class Log4cplusObjectSimpleLayout : public Log4cplusObjectLayout
{
public:
    Log4cplusObjectSimpleLayout(log4cplus::SimpleLayout * object) : Log4cplusObjectLayout(object) {}
    Log4cplusObjectSimpleLayout(const Log4cplusObjectSimpleLayout & rhs) : Log4cplusObjectLayout(rhs) {}
    virtual ~Log4cplusObjectSimpleLayout() {}

    log4cplus::SimpleLayout * object() { return (log4cplus::SimpleLayout *)_object; }
};

class Log4cplusObjectPatternLayout : public Log4cplusObjectLayout
{
public:
    Log4cplusObjectPatternLayout(log4cplus::PatternLayout * object) : Log4cplusObjectLayout(object) {}
    Log4cplusObjectPatternLayout(const Log4cplusObjectPatternLayout & rhs) : Log4cplusObjectLayout(rhs) {}
    virtual ~Log4cplusObjectPatternLayout() {}

    log4cplus::PatternLayout * object() { return (log4cplus::PatternLayout *)_object; }
};

} // namespace log4cplus_plugin

typedef SGIHostItemImpl<log4cplus_plugin::Log4cplusObjectBase, osg::ref_ptr<log4cplus_plugin::Log4cplusObjectBase> > SGIHostItemLog4cplus;

} // namespace sgi
