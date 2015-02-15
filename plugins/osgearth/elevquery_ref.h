#pragma once

namespace osgEarth {
    class ElevationQuery;
}

namespace sgi {
namespace osgearth_plugin {

    class ElevationQueryRef : public osg::Referenced
    {
    public:
        ElevationQueryRef(osgEarth::ElevationQuery * query=NULL)
            : osg::Referenced()
            , _query(query)
        {
        }
        ElevationQueryRef(const ElevationQueryRef & rhs)
            : osg::Referenced(rhs)
            , _query(rhs._query)
        {
        }
        void assign(const ElevationQueryRef & rhs)
        {
            _query = rhs._query;
        }

        ElevationQueryRef& operator = (const ElevationQueryRef& rp)
        {
            assign(rp);
            return *this;
        }

        osgEarth::ElevationQuery& operator*() const { return *_query; }
        osgEarth::ElevationQuery* operator->() const { return _query; }
        osgEarth::ElevationQuery* get() const { return _query; }

        bool operator!() const   { return _query==0; } // not required
        bool valid() const       { return _query!=0; }
    private:
        osgEarth::ElevationQuery * _query;
    };

} // namespace osgearth_plugin
} // namespace sgi
