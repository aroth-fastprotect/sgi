#pragma once

#ifdef MAPNODE_ACCESS_HACK
#define private protected
#endif
#include <osgEarth/MapNode>
#include <osgEarth/Map>
#ifdef MAPNODE_ACCESS_HACK
#undef private
#endif
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
#include <osgEarth/MapFrame>
#endif

#include <osgEarthUtil/Controls>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/RTTPicker>
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/Registry>
#include <osgEarth/VirtualProgram>
#ifdef ELEVATIONQUERY_ACCESS_HACK
#define private protected
#endif
#include <osgEarth/ElevationQuery>
#ifdef ELEVATIONQUERY_ACCESS_HACK
#undef private
#endif
#include <osgEarthFeatures/FeatureModelSource>
#include <osg/io_utils>

class QWidget;

namespace std {

inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::DateTime & t)
{
    return os << t.asRFC1123();
}

} // namespace std

namespace sgi {
namespace osgearth_plugin {

	class RegistryAccess : public osgEarth::Registry
	{
	public:
		unsigned numBlacklistFilenames()
		{
			unsigned ret;
			_blacklistMutex.readLock();
			ret = _blacklistedFilenames.size();
			_blacklistMutex.readUnlock();
			return ret;
		}
		void getBlacklistFilenames(StringSet & filenames)
		{
			_blacklistMutex.readLock();
			filenames = _blacklistedFilenames;
			_blacklistMutex.readUnlock();
		}
	};
    class MapAccess : public osgEarth::Map
    {
    public:
        void getMapCallbacks(osgEarth::MapCallbackList & mapCallbacks) const;
    };

	class MapNodeAccess : public osgEarth::MapNode
	{
	public:
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
		const osgEarth::Terrain* terrain() const { return _terrainEngine.valid() ? _terrainEngine->getTerrain() : NULL; }
		osgEarth::TerrainEngineNode * terrainEngineNode() const { return _terrainEngine.get(); }
		bool terrainEngineInitialized() const { return _terrainEngineInitialized; }
		osg::Group* terrainEngineContainer() const { return _terrainEngineContainer; }
#endif
		bool hasMapInspector() const;
		void toggleMapInspector();
		
		bool isTerrainProfileActive() const;
		void toggleTerrainProfile(QWidget * parent);

#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
        bool getCullDataCameras(osg::NodeList & cameras);
        const osgEarth::MapNodeCullData * getCullDataForCamera(osg::Camera* camera);
#endif
	};

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
            for(TravSpecificDataMap::const_iterator it = _travDataMap.begin(); it != _travDataMap.end(); it++)
            {
                const osg::Camera* camera = it->first;
                os << "<li>0x" << std::hex << (void*)camera << "<br/>" << std::dec;
                appendCamera(os, it->first, it->second);
                os << "</li>" << std::endl;
            }
            os << "</ul>" << std::endl;
        }
    protected:
        void appendCamera(std::basic_ostream<char>& os, const osg::Camera * camera, const TravSpecificData & viewdata) const
        {
            os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;
            os << "<tr><td>obscured</td><td>" << (viewdata._obscured?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>screenPos</td><td>" << viewdata._screenPos << "</td></tr>" << std::endl;
            os << "<tr><td>visibleTime</td><td>" << viewdata._visibleTime << "</td></tr>" << std::endl;
            os << "<tr><td>visitFrame</td><td>" << viewdata._visitFrame << "</td></tr>" << std::endl;
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            os << "<tr><td>_uniform</td><td>" << viewdata._uniform << "</td></tr>" << std::endl;
#else
            os << "<tr><td>_uniform</td><td>" << viewdata._uniform.get() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>_canvas</td><td>";
            if(viewdata._canvas.valid())
                os << viewdata._canvas.get();
            os << "</td></tr>" << std::endl;
            os << "</table>" << std::endl;
        }
    };
    class LayerAccessor : public osgEarth::Layer
    {
    public:
#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
        typedef std::vector<osg::ref_ptr<osgEarth::LayerCallback> > LayerCallbackList;
        void getLayerCallbacks(LayerCallbackList & callbacks) const;
        osgEarth::CacheSettings * getCacheSettings() const;
#endif
    };

    class TerrainLayerAccessor : public osgEarth::TerrainLayer
    {
    public:
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
        inline void setCachePolicy( const osgEarth::CachePolicy& cp ) { osgEarth::TerrainLayer::setCachePolicy(cp); }
        inline const osgEarth::CachePolicy& getCachePolicy() const { return osgEarth::TerrainLayer::getCachePolicy(); }
#endif
        inline std::string getURL() const
        {
            std::string url;
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
            const osgEarth::TerrainLayerOptions & layerOpts = getTerrainLayerRuntimeOptions();
            osgEarth::Config layerConf = layerOpts.getConfig();
#else
            osgEarth::Config layerConf = getConfig();
#endif
            if(layerConf.hasValue("url"))
                url = layerConf.value("url");
            return url;
        }
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
        inline void setEnabled(bool /*enable*/)
        {
            (void)(0);
        }
#endif

#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
        inline osgEarth::TileSource * tileSourceNoInit() { return _tileSource.get(); }
		inline bool tileSourceInitFailed() const { return _tileSourceInitFailed; }
#endif
		inline const osgEarth::Profile * profileNoInit() { return _profile.get(); }
        inline const osgEarth::Profile * targetProfileHintNoInit() { return _targetProfileHint.get(); }
        
        inline unsigned tileSize() const { return _tileSize; }
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
        inline bool tileSourceInitAttempted() const { return _tileSourceInitAttempted; }
        inline osgDB::Options * dbOptions() { return _dbOptions.get(); }
#endif
    };
    class ElevationLayerAccessor : public osgEarth::ElevationLayer
    {
    public:
#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
        inline std::string suggestCacheFormatStr() const
        {
            return suggestCacheFormat();
        }
#endif
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

#if OSGEARTH_VERSION_LESS_OR_EQUAL(2,7,0)
        void getShaderMap( ShaderMap& out ) const { osgEarth::VirtualProgram::getShaderMap(out); }
        void getFunctions( osgEarth::ShaderComp::FunctionLocationMap& out ) const { osgEarth::VirtualProgram::getFunctions(out); }
#endif

        unsigned int                 mask() const { return _mask; }
        void                         setMask(unsigned int mask) { _mask = mask; }
        const AttribBindingList &    attribBindingList() const { return _attribBindingList; }
        const AttribAliasMap &       attribAliases() const { return _attribAliases; }
        bool                         inherit() const { return _inherit; }
        bool                         inheritSet() const { return _inheritSet; }

        bool                         getShaderLogging() const { return _logShaders; }
        const std::string &          getShaderLogFile() const { return _logPath; }

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,9,0)
        void                         getGLSLExtensions(ExtensionsSet & extensions);
#endif
        
        
        void                         setInheritShadersToUnspecified()
        {
            if (_inheritSet )
            {
                // clear the program cache please
                {
#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
                    osgEarth::Threading::ScopedWriteLock exclusive(_programCacheMutex);
                    _programCache.clear();
#endif
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
            DataAccess() 
#if OSGEARTH_VERSION_LESS_THAN(2,10,0)
                : _mapf(NULL)
#endif
            {}
            virtual ~DataAccess() { }
#if OSGEARTH_VERSION_LESS_THAN(2,10,0)
            osgEarth::MapFrame  _mapf;
#endif
            unsigned  _maxCacheSize;
            int       _tileSize;
            int       _maxLevelOverride;

            TileCache _tileCache;

            double _queries;
            double _totalTime;
        };
    public:
        const TileCache & tileCache() { return ((DataAccess*)this)->_tileCache; }
#if OSGEARTH_VERSION_LESS_THAN(2,10,0)
        /** Accesses the profile/rendering info about the source map. */
        const osgEarth::MapInfo& getMapInfo() const;
        /** Convenience method to access the map's profile */
        const osgEarth::Profile* getProfile() const;
#else
        const osgEarth::Map* getMap() const;
#endif
    };

#if OSGEARTH_VERSION_GREATER_OR_EQUAL(2,10,0)
    class ElevationPoolAccess : public osgEarth::ElevationPool
    {
    public:
        typedef std::list<osg::ref_ptr<Tile> > MRU;
        typedef std::map<osgEarth::TileKey, osg::observer_ptr<Tile> > Tiles;

        void getTiles(Tiles & tiles);
        void getMRU(MRU & mru);
    };
#endif

    class TileBlacklistAccess : public osgEarth::TileBlacklist
    {
    public:
        typedef std::set<osgEarth::TileKey> TileKeySet;

#if OSGEARTH_VERSION_LESS_OR_EQUAL(2,7,0) && defined(OSGEARTH_WITH_FAST_MODIFICATIONS)
        void getTileKeySet(TileKeySet & ts)
        {
            osgEarth::Threading::ScopedReadLock lock(_mutex);
            ts = _tiles;
        }
#endif
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

#if OSGEARTH_VERSION_LESS_THAN(2,7,0)
    typedef osgEarth::Threading::PerObjectRefMap<std::string, osgEarth::CacheBin> ThreadSafeCacheBinMap;
#else
	typedef osgEarth::PerObjectRefMap<std::string, osgEarth::CacheBin> ThreadSafeCacheBinMap;
#endif
    typedef PerObjectRefMapAccessor<std::string, osgEarth::CacheBin> ThreadSafeCacheBinMapAccessor;

    class FeatureModelSourceAccess : public osgEarth::Features::FeatureModelSource
    {
    public:
        bool getMap(osg::ref_ptr<const osgEarth::Map> & map) const { return _map.lock(map); }
        osgEarth::Features::FeatureNodeFactory * getFeatureNodeFactory() { return _factory.get(); }
#if OSGEARTH_VERSION_LESS_THAN(2,8,0)
        const osgDB::Options * getDBOptions() { return _dbOptions.get(); }
#endif
    };

    class RTTPickerAccess : public osgEarth::Util::RTTPicker 
    {
    public:
        int                    getRttSize() const { return _rttSize; }

        Callback *              getDefaultCallback() const { return _defaultCallback.get(); }

#if OSGEARTH_VERSION_LESS_THAN(2,9,0)
        typedef std::vector<PickContext> PickContexts;
#else
        typedef std::list<PickContext> PickContexts;
#endif
        void getPickContexts( PickContexts & contexts) const;

        osg::Group *            getGroup() const { return _group.get(); }
    };

} // namespace osgearth_plugin
} // namespace sgi
