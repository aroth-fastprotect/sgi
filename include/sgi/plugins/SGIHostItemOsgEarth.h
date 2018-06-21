// kate: syntax C++;
// SGI - Copyright (C) 2012-2018 FAST Protect, Andreas Roth

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

typedef SGIItemInfoPlainPtr<osgEarth::Config> SGIItemInfoOsgEarthConfig;
typedef SGIHostItemImpl<SGIItemInfoOsgEarthConfig> SGIHostItemOsgEarthConfig;
typedef SGIItemInfoPlainPtr<osgEarth::ConfigOptions> SGIItemInfoOsgEarthConfigOptions;
typedef SGIHostItemImpl<SGIItemInfoOsgEarthConfigOptions> SGIHostItemOsgEarthConfigOptions;

#if 0

class SGIHostItemOsgEarthConfigOptions : public SGIHostItemImpl<osgEarth::ConfigOptions>
{
public:
    explicit SGIHostItemOsgEarthConfigOptions(osgEarth::ConfigOptions * object, osg::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::ConfigOptions>(object, userData, flags), _allocated(false)
        {}
    explicit SGIHostItemOsgEarthConfigOptions(const osgEarth::ConfigOptions * object, osg::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::ConfigOptions>(const_cast<osgEarth::ConfigOptions*>(object), userData, flags), _allocated(false)
        {}
    enum AllocationMode {
        NO_DELETE,
        USE_NEW_DELETE
    };
    explicit SGIHostItemOsgEarthConfigOptions(AllocationMode allocationMode, osgEarth::ConfigOptions * object, osg::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::ConfigOptions>(object, userData, flags), _allocated(allocationMode==NO_DELETE?false:true)
        {}
    template <typename VALUE_TYPE>
    SGIHostItemOsgEarthConfigOptions(const osgEarth::optional<VALUE_TYPE> & optionalConfigOption, osg::Referenced * userData=nullptr, unsigned flags=0)
        : SGIHostItemImpl<osgEarth::ConfigOptions>(
            optionalConfigOption.isSet()?
                static_cast<osgEarth::ConfigOptions*>(const_cast<VALUE_TYPE*>(&optionalConfigOption.value()))
                :nullptr,
            userData,
            flags), _allocated(false)
        {}
    template <typename VALUE_TYPE>
    SGIHostItemOsgEarthConfigOptions(const VALUE_TYPE & optionalConfigOption, osg::Referenced * userData=nullptr, unsigned flags=0)
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
    void releaseObject() { _allocated = false; _object = nullptr; }
private:
    bool _allocated;
};
#endif // 0

} // namespace sgi
