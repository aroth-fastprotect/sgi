#pragma once
#include "SGIHostItemBase.h"
#include <osgEarth/Config>

namespace osg {
    class Referenced;
}

namespace osgEarth {
    class Config;
    class ConfigOptions;
}

namespace sgi {
namespace support {

class SGIHostItemOsgEarthConfig : public SGIHostItemImpl<osgEarth::Config>
{
public:
    SGIHostItemOsgEarthConfig(osgEarth::Config * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::Config>(object, userData, flags), _allocated(false)
        {}
    SGIHostItemOsgEarthConfig(const osgEarth::Config * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::Config>(const_cast<osgEarth::Config*>(object), userData, flags), _allocated(false)
        {}
    SGIHostItemOsgEarthConfig(const osgEarth::Config & object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::Config>(new osgEarth::Config(object), userData, flags), _allocated(true)
        {}
    virtual ~SGIHostItemOsgEarthConfig()
    {
        if(_allocated)
            delete _object;
    }
    bool isAllocated() const { return _allocated; }
    void releaseObject() { _allocated = false; _object = NULL; }
private:
    bool _allocated;
};

class SGIHostItemOsgEarthConfigOptions : public SGIHostItemImpl<osgEarth::ConfigOptions>
{
public:
    explicit SGIHostItemOsgEarthConfigOptions(osgEarth::ConfigOptions * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::ConfigOptions>(object, userData, flags), _allocated(false)
        {}
    explicit SGIHostItemOsgEarthConfigOptions(const osgEarth::ConfigOptions * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::ConfigOptions>(const_cast<osgEarth::ConfigOptions*>(object), userData, flags), _allocated(false)
        {}
    enum AllocationMode {
        NO_DELETE,
        USE_NEW_DELETE
    };
    explicit SGIHostItemOsgEarthConfigOptions(AllocationMode allocationMode, osgEarth::ConfigOptions * object, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::ConfigOptions>(object, userData, flags), _allocated(allocationMode==NO_DELETE?false:true)
        {}
    template <typename VALUE_TYPE>
    SGIHostItemOsgEarthConfigOptions(const osgEarth::optional<VALUE_TYPE> & optionalConfigOption, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::ConfigOptions>(
            optionalConfigOption.isSet()?
                static_cast<osgEarth::ConfigOptions*>(const_cast<VALUE_TYPE*>(&optionalConfigOption.value()))
                :NULL,
            userData,
            flags), _allocated(false)
        {}
    template <typename VALUE_TYPE>
    SGIHostItemOsgEarthConfigOptions(const VALUE_TYPE & optionalConfigOption, osg::Referenced * userData=NULL, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::ConfigOptions>(
            static_cast<osgEarth::ConfigOptions*>(const_cast<VALUE_TYPE*>(&optionalConfigOption)),
            userData,
            flags), _allocated(false)
        {}
    virtual ~SGIHostItemOsgEarthConfigOptions()
    {
        if(_allocated)
            delete _object;
    }
    bool isAllocated() const { return _allocated; }
    void releaseObject() { _allocated = false; _object = NULL; }
private:
    bool _allocated;
};

} // namespace support

typedef support::SGIHostItemOsgEarthConfig SGIHostItemOsgEarthConfig;
typedef support::SGIHostItemOsgEarthConfigOptions SGIHostItemOsgEarthConfigOptions;

} // namespace sgi
