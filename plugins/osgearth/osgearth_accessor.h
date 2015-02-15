#pragma once

#include <osgEarthUtil/Controls>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarth/VirtualProgram>
#include <osgEarth/ElevationQuery>
#include <osgEarthFeatures/FeatureModelSource>
#include <osg/io_utils>

namespace std {

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::DateTime & t)
{
    return os << t.asRFC1123();
}

} // namespace std

namespace sgi {
namespace osgearth_plugin {

    class ControlCanvasAccess : public osgEarth::Util::Controls::ControlCanvas
    {
    public:
        const osgEarth::Util::Controls::ControlContext& controlContext() const { return _context; }
        bool isContextDirty() const { return _contextDirty; }
        bool isUpdatePending() const { return _updatePending; }
        const osgEarth::Util::Controls::ControlNodeBin * controlNodeBin() const { return _controlNodeBin.get(); }
        void dirty() { _contextDirty = true; }
    };
    class ControlAccess : public osgEarth::Util::Controls::Control
    {
    public:
        const osgEarth::Util::Controls::ControlEventHandlerList & eventHandlers() const { return _eventHandlers; }
        const osg::Geode * geode() { return osgEarth::Util::Controls::Control::getGeode(); }
    };

    class ControlNodeAccess : public osgEarth::Util::Controls::ControlNode
    {
    public:
        void appendAllViews(std::basic_ostream<char>& os) const
        {
            os << "<ul></li>" << std::endl;
#ifdef OSGEARTH_CONTROLS_V2_5
            for(TravSpecificDataMap::const_iterator it = _travDataMap.begin(); it != _travDataMap.end(); it++)
            {
                const osg::Camera* camera = it->first;
                os << "<li>0x" << std::hex << (void*)camera << "<br/>" << std::dec;
                appendCamera(os, it->first, it->second);
                os << "</li>" << std::endl;
            }
#else // OSGEARTH_CONTROLS_V2_5
            for(PerViewDataMap::const_iterator it = _perViewData.begin(); it != _perViewData.end(); it++)
            {
                const osg::View * view = it->first;
                os << "<li>0x" << std::hex << (void*)view << "<br/>" << std::dec;
                appendView(os, it->first, it->second);
                os << "</li>" << std::endl;
            }
#endif // OSGEARTH_CONTROLS_V2_5
            os << "</ul>" << std::endl;
        }
    protected:
#ifdef OSGEARTH_CONTROLS_V2_5
        void appendCamera(std::basic_ostream<char>& os, const osg::Camera * camera, const TravSpecificData & viewdata) const
#else // OSGEARTH_CONTROLS_V2_5
        void appendView(std::basic_ostream<char>& os, const osg::View * view, const PerViewData & viewdata) const
#endif // OSGEARTH_CONTROLS_V2_5
        {
            os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            os << "<tr><td>obscured</td><td>" << (viewdata._obscured?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>screenPos</td><td>" << viewdata._screenPos << "</td></tr>" << std::endl;
            os << "<tr><td>visibleTime</td><td>" << viewdata._visibleTime << "</td></tr>" << std::endl;
            os << "<tr><td>visitFrame</td><td>" << viewdata._visitFrame << "</td></tr>" << std::endl;
            os << "<tr><td>_uniform</td><td>" << viewdata._uniform << "</td></tr>" << std::endl;
            os << "<tr><td>_canvas</td><td>";
            if(viewdata._canvas.valid())
                os << viewdata._canvas.get();
            os << "</td></tr>" << std::endl;
            os << "</table>" << std::endl;
        }
    };
    class TerrainLayerAccessor : public osgEarth::TerrainLayer
    {
    public:
        inline void setCachePolicy( const osgEarth::CachePolicy& cp ) { osgEarth::TerrainLayer::setCachePolicy(cp); }
        inline const osgEarth::CachePolicy& getCachePolicy() const { return osgEarth::TerrainLayer::getCachePolicy(); }
        inline std::string getURL() const
        {
            std::string url;
            const osgEarth::TerrainLayerOptions & layerOpts = getTerrainLayerRuntimeOptions();
            osgEarth::Config layerConf = layerOpts.getConfig();
            if(layerConf.hasValue("url"))
                url = layerConf.value("url");
            return url;
        }
        inline void setEnabled(bool /*enable*/)
        {
            (void)(0);
        }

        inline osgEarth::TileSource * tileSourceNoInit() { return _tileSource.get(); }
        inline const osgEarth::Profile * profileNoInit() { return _profile.get(); }
        inline const osgEarth::Profile * targetProfileNoInit() { return _targetProfileHint.get(); }
        
        inline bool tileSourceInitAttempted() const { return _tileSourceInitAttempted; }
        inline bool tileSourceInitFailed() const { return _tileSourceInitFailed; }
        inline unsigned tileSize() const { return _tileSize; }
        inline osgDB::Options * dbOptions() { return _dbOptions.get(); }
    };
    class ElevationLayerAccessor : public osgEarth::ElevationLayer
    {
    public:
        inline std::string suggestCacheFormatStr() const
        {
            return suggestCacheFormat();
        }
    };

    class ModelLayerAccessor : public osgEarth::ModelLayer
    {
    public:
        inline void setEnabled(bool /*enable*/)
        {
            (void)(0);
        }
    };

    class AutoClipPlaneCullCallbackAccessor : public osgEarth::Util::AutoClipPlaneCullCallback
    {
    public:
        bool isActive() const { return _active; }
        double minimumRadius() const { return _rp; }
    };

    class VirtualProgramAccessor : public osgEarth::VirtualProgram
    {
    public:
        typedef osgEarth::VirtualProgram::ShaderEntry ShaderEntry;
        typedef osgEarth::VirtualProgram::ProgramMap ProgramMap;

        const ProgramMap & programCache() const { return _programCache; }
        void getShaderMap( ShaderMap& out ) const { osgEarth::VirtualProgram::getShaderMap(out); }
        void getFunctions( osgEarth::ShaderComp::FunctionLocationMap& out ) const { osgEarth::VirtualProgram::getFunctions(out); }

        unsigned int                 mask() const { return _mask; }
        void                         setMask(unsigned int mask) { _mask = mask; }
        const AttribBindingList &    attribBindingList() const { return _attribBindingList; }
        const AttribAliasMap &       attribAliases() const { return _attribAliases; }
        bool                         inherit() const { return _inherit; }
        bool                         inheritSet() const { return _inheritSet; }
        
        void                         setInheritShadersToUnspecified()
        {
            if (_inheritSet )
            {
                // clear the program cache please
                {
                    osgEarth::Threading::ScopedWriteLock exclusive(_programCacheMutex);
                    _programCache.clear();
                }

                _inheritSet = false;
            }
        }

    };


    template<typename K, typename T, typename COMPARE=std::less<K> >
    class LRUCacheAccess : public osgEarth::LRUCache<K, T, COMPARE>
    {
    public:
        typedef typename std::list<K>::iterator      lru_iter;
        typedef typename std::list<K>::const_iterator lru_const_iter;
        typedef typename std::list<K>                lru_type;
        typedef typename std::pair<T, lru_iter>      map_value_type;
        typedef typename std::map<K, map_value_type> map_type;
        typedef typename map_type::iterator          map_iter;
        typedef typename map_type::const_iterator    map_const_iter;
        
        const map_type & map() const { return osgEarth::LRUCache<K, T, COMPARE>::_map; }
        const lru_type & lru() const { return osgEarth::LRUCache<K, T, COMPARE>::_lru; }
    };

    class ElevationQueryAccess : public osgEarth::ElevationQuery
    {
    public:
        typedef LRUCacheAccess< osgEarth::TileKey, osg::ref_ptr<osg::HeightField> > TileCache;
    private:
        class DataAccess
        {
        private:
            DataAccess(const DataAccess & rhs);
        public:
            DataAccess() : _mapf(NULL) {}
            virtual ~DataAccess() { }
            osgEarth::MapFrame  _mapf;
            unsigned  _maxCacheSize;
            int       _tileSize;
            int       _maxLevelOverride;

            TileCache _tileCache;

            double _queries;
            double _totalTime;
        };
    public:
        const TileCache & tileCache() { return ((DataAccess*)this)->_tileCache; }
        /** Accesses the profile/rendering info about the source map. */
        const osgEarth::MapInfo& getMapInfo() const { return ((DataAccess*)this)->_mapf.getMapInfo(); }
        /** Convenience method to access the map's profile */
        const osgEarth::Profile* getProfile() const { return ((DataAccess*)this)->_mapf.getProfile(); }
    };


    class TileBlacklistAccess : public osgEarth::TileBlacklist
    {
    public:

    };

    class CacheAccess : public osgEarth::Cache
    {
    public:
        const osgEarth::CacheBin *    getDefaultBin() const { return _defaultBin.get(); }
        const osgEarth::ThreadSafeCacheBinMap & getCacheBinMap() const { return _bins; }
    };

    template<typename KEY, typename DATA>
    struct PerObjectRefMapAccessor
    {
        typedef std::map<KEY,osg::ref_ptr<DATA> > MAP;
    public:
        MAP    _data;
        osgEarth::Threading::ReadWriteMutex  _mutex;
    };

    typedef osgEarth::Threading::PerObjectRefMap<std::string, osgEarth::CacheBin> ThreadSafeCacheBinMap;
    typedef PerObjectRefMapAccessor<std::string, osgEarth::CacheBin> ThreadSafeCacheBinMapAccessor;

    class FeatureModelSourceAccess : public osgEarth::Features::FeatureModelSource
    {
    public:
        bool getMap(osg::ref_ptr<const osgEarth::Map> & map) const { return _map.lock(map); }
        osgEarth::Features::FeatureNodeFactory * getFeatureNodeFactory() { return _factory.get(); }
        const osgDB::Options * getDBOptions() { return _dbOptions.get(); }
    };

} // namespace osgearth_plugin
} // namespace sgi
