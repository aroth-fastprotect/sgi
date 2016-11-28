#pragma once
namespace osgDB {
    class DynamicLibrary;
}

namespace sgi {
namespace osg_plugin {

class RegistryAccessor : public osgDB::Registry
{
public:
    typedef std::vector< osg::ref_ptr<osgDB::DynamicLibrary> >             DynamicLibraryList;

    const DynamicLibraryList & getDynamicLibraryList() const { return _dlList; }
};

class DatabasePagerAccessor : public osgDB::DatabasePager
{
public:
	typedef DatabaseRequest DatabaseRequestAccess;

    unsigned int numberOfPagedLODs() const
    {
        return _activePagedLODList->size();
    }
    unsigned int getLocalFileRequestListSize() const 
    { 
        return _fileRequestQueue->size();
    }
    unsigned int getHttpRequestListSize() const 
    { 
        return _httpRequestQueue->size();
    }
    unsigned int getDataToCompileListSize() const
    {
        return _dataToCompileList->size();
    }
    unsigned int getDataToMergeListSize() const
    {
        return _dataToMergeList->size();
    }
    class SetBasedPagedLODList : public osgDB::DatabasePager::PagedLODList
    {
    public:

        typedef std::set< osg::observer_ptr<osg::PagedLOD> > PagedLODs;
        PagedLODs _pagedLODs;
        
        typedef PagedLODs::iterator iterator;
        typedef PagedLODs::const_iterator const_iterator;
        
        iterator begin() { return _pagedLODs.begin(); }
        iterator end() { return _pagedLODs.end(); }
        const_iterator begin() const { return _pagedLODs.begin(); }
        const_iterator end() const { return _pagedLODs.end(); }
    };
    PagedLODList * activePagedLODList() const
    {
        return _activePagedLODList.get();
    }

    typedef std::list< osg::ref_ptr<DatabaseRequest> > RequestList;

    struct RequestQueueAccess : public RequestQueue
    {
    public:
        void copy(RequestList& requestList) { requestList = _requestList; }
    };

    void copyFileRequests(RequestList& requestList)
    {
        ((RequestQueueAccess*)_fileRequestQueue.get())->copy(requestList);
    }
    void copyHttpRequests(RequestList& requestList)
    {
        ((RequestQueueAccess*)_httpRequestQueue.get())->copy(requestList);
    }
    void copyDataToCompile(RequestList& requestList)
    {
        ((RequestQueueAccess*)_dataToCompileList.get())->copy(requestList);
    }
    void copyDataToMerge(RequestList& requestList)
    {
        ((RequestQueueAccess*)_dataToMergeList.get())->copy(requestList);
    }
};

class DatabaseThreadAccess : public osgDB::DatabasePager::DatabaseThread
{
public:
    Mode                getMode() const { return _mode; }
};

class OptionsAccess : public osgDB::Options
{
public:
    typedef std::map<std::string, void*> PluginDataMap;
    typedef std::map<std::string, std::string> PluginStringDataMap;

    const PluginDataMap & getPluginDataMap() const 
    {
        return _pluginData;
    }
    const PluginStringDataMap & getPluginStringDataMap() const
    {
        return _pluginStringData;
    }
};

} // namespace osg_plugin
} // namespace sgi

