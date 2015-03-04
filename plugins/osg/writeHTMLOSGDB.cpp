#include "stdafx.h"
#include <ostream>
#include "writeHTMLOSG.h"
#include "writeHTMLOSGDB.h"
#include "SGIItemOsg"

#include <osg/Version>
#include <osgDB/Registry>
#include <osgDB/ImagePager>
#include <osgDB/DatabasePager>

#include <osg/ClipNode>

#include <sgi/helpers/string>

#include <sgi/helpers/osg>
#include "osgdb_accessor.h"

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::Registry)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::Options)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::ReaderWriter)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::ImagePager)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::DatabasePager)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::FileCache)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::DatabaseRevision)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::DatabaseRevisions)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::FileList)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::ObjectWrapper)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::ObjectWrapperManager)
WRITE_PRETTY_HTML_IMPL_REGISTER(osgDB::BaseSerializer)

bool writePrettyHTMLImpl<osgDB::Registry>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::Registry * object = getObject<osgDB::Registry,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            callNextHandler(os);

            // add Registry properties
            os << "<tr><td>mime-type map</td><td><table border=\'1\' align=\'left\' width=\'100%\'>";
            const osgDB::Registry::MimeTypeExtensionMap & mimemap = object->getMimeTypeExtensionMap();
            for(osgDB::Registry::MimeTypeExtensionMap::const_iterator it = mimemap.begin(); it != mimemap.end(); it++)
            {
                const std::string & format = it->first;
                const std::string & desc = it->second;
                os << "<tr><td>" << format << "</td><td>" << desc << "</td></tr>" << std::endl;
            }
            os << "</table></td></tr>" << std::endl;

            os << "<tr><td>buildKdTreesHint</td><td>" << object->getBuildKdTreesHint() << "</td></tr>" << std::endl;
            os << "<tr><td>createNodeFromImage</td><td>" << (object->getCreateNodeFromImage()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>dataFile path</td><td><ol>";
            const osgDB::FilePathList & datafilepath = object->getDataFilePathList();
            for(osgDB::FilePathList::const_iterator it = datafilepath.begin(); it != datafilepath.end(); it++)
            {
                const std::string & item = *it;
                os << "<li>" << item << "</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;
            os << "<tr><td>libraryFile path</td><td><ol>";
            const osgDB::FilePathList & libraryfilepath = object->getLibraryFilePathList();
            for(osgDB::FilePathList::const_iterator it = libraryfilepath.begin(); it != libraryfilepath.end(); it++)
            {
                const std::string & item = *it;
                os << "<li>" << item << "</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;
            os << "<tr><td>expiryDelay</td><td>" << object->getExpiryDelay() << "</td></tr>" << std::endl;

            os << "<tr><td>archiveExtensions</td><td><ol>";
            const osgDB::Registry::ArchiveExtensionList & archExts = object->getArchiveExtensions();
            for(osgDB::Registry::ArchiveExtensionList::const_iterator it = archExts.begin(); it != archExts.end(); it++)
            {
                const std::string & item = *it;
                os << "<li>" << item << "</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;

            RegistryAccessor * access = (RegistryAccessor *)object;
            os << "<tr><td>archiveExtensions</td><td><ul>";
            const RegistryAccessor::DynamicLibraryList & dllist = access->getDynamicLibraryList();
            for(RegistryAccessor::DynamicLibraryList::const_iterator it = dllist.begin(); it != dllist.end(); it++)
            {
                const osg::ref_ptr<osgDB::DynamicLibrary> & item = *it;
                os << "<li>" << item->getName() << "<br/>" << item->getFullName() << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // first add all callbacks from base classes
            callNextHandler(os);

            os << "<tr><td>ReadFileCallback</td><td>" << osg_helpers::getObjectNameAndType(object->getReadFileCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>WriteFileCallback</td><td>" << osg_helpers::getObjectNameAndType(object->getWriteFileCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>FileLocationCallback</td><td>" << osg_helpers::getObjectNameAndType(object->getFileLocationCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>FindFileCallback</td><td>" << osg_helpers::getObjectNameAndType(object->getFindFileCallback()) << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeReadersWriters:
        {
            os << "<ul>";
            const osgDB::Registry::ReaderWriterList& list = object->getReaderWriterList();
            for(osgDB::Registry::ReaderWriterList::const_iterator it = list.begin(); it != list.end(); it++)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType((*it).get()) << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeImageProcessors:
        {
            os << "<ul>";
            const osgDB::Registry::ImageProcessorList& list = object->getImageProcessorList();
            for(osgDB::Registry::ImageProcessorList::const_iterator it = list.begin(); it != list.end(); it++)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType((*it).get()) << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgDB::Options>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::Options * object = getObject<osgDB::Options,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>option string</td><td>" << object->getOptionString() << "</td></tr>" << std::endl;
            os << "<tr><td>database path</td><td><ol>";
            const osgDB::FilePathList & dbpath = object->getDatabasePathList();
            for(osgDB::FilePathList::const_iterator it = dbpath.begin(); it != dbpath.end(); it++)
            {
                const std::string & item = *it;
                os << "<li>" << item << "</li>" << std::endl;
            }
            os << "</ol></td></tr>" << std::endl;

            os << "<tr><td>cache hint</td><td>" << object->getObjectCacheHint() << "</td></tr>" << std::endl;
            os << "<tr><td>precision hint</td><td>" << object->getPrecisionHint() << "</td></tr>" << std::endl;
            os << "<tr><td>build KD tree hint</td><td>" << object->getBuildKdTreesHint() << "</td></tr>" << std::endl;

            os << "<tr><td>authentication map</td><td>" << (object->getAuthenticationMap()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>plugin data</td><td>not available yet</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCallbacks:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // first add all callbacks from base classes
            callNextHandler(os);

            os << "<tr><td>ReadFileCallback</td><td>" << osg_helpers::getObjectNameAndType(object->getReadFileCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>WriteFileCallback</td><td>" << osg_helpers::getObjectNameAndType(object->getWriteFileCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>FileLocationCallback</td><td>" << osg_helpers::getObjectNameAndType(object->getFileLocationCallback()) << "</td></tr>" << std::endl;
            os << "<tr><td>FindFileCallback</td><td>" << osg_helpers::getObjectNameAndType(object->getFindFileCallback()) << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgDB::ReaderWriter>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::ReaderWriter * object = getObject<osgDB::ReaderWriter,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>supportedFeatures</td><td><ul>";
            const osgDB::ReaderWriter::FeatureList & features = osgDB::ReaderWriter::featureAsString(object->supportedFeatures());
            for(osgDB::ReaderWriter::FeatureList::const_iterator it = features.begin(); it != features.end(); it++)
                os << "<li>" << *it << "</li>" << std::endl;
            os << "</ul></td></tr>" << std::endl;

            os << "<tr><td>supportedProtocols</td><td><table border=\'1\' align=\'left\' width=\'100%\'>";
            const osgDB::ReaderWriter::FormatDescriptionMap & protocols = object->supportedProtocols();
            for(osgDB::ReaderWriter::FormatDescriptionMap::const_iterator it = protocols.begin(); it != protocols.end(); it++)
            {
                const std::string & format = it->first;
                const std::string & desc = it->second;
                os << "<tr><td>" << format << "</td><td>" << desc << "</td></tr>" << std::endl;
            }
            os << "</table></td></tr>" << std::endl;

            os << "<tr><td>supportedExtensions</td><td><table border=\'1\' align=\'left\' width=\'100%\'>";
            const osgDB::ReaderWriter::FormatDescriptionMap & extentions = object->supportedExtensions();
            for(osgDB::ReaderWriter::FormatDescriptionMap::const_iterator it = extentions.begin(); it != extentions.end(); it++)
            {
                const std::string & format = it->first;
                const std::string & desc = it->second;
                os << "<tr><td>" << format << "</td><td>" << desc << "</td></tr>" << std::endl;
            }
            os << "</table></td></tr>" << std::endl;

            os << "<tr><td>supportedOptions</td><td><table border=\'1\' align=\'left\' width=\'100%\'>";
            const osgDB::ReaderWriter::FormatDescriptionMap & options = object->supportedOptions();
            for(osgDB::ReaderWriter::FormatDescriptionMap::const_iterator it = options.begin(); it != options.end(); it++)
            {
                const std::string & format = it->first;
                const std::string & desc = it->second;
                os << "<tr><td>" << format << "</td><td>" << desc << "</td></tr>" << std::endl;
            }
            os << "</table></td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgDB::ImagePager>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::ImagePager * object = getObject<osgDB::ImagePager,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>num threads</td><td>" << object->getNumImageThreads() << "</td></tr>" << std::endl;
            os << "<tr><td>require update</td><td>" << (object->requiresUpdateSceneGraph()?"true":"false") << "</td></tr>" << std::endl;

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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgDB::DatabasePager::DatabaseThread::Mode & t)
{
    switch(t)
    {
    case osgDB::DatabasePager::DatabaseThread::HANDLE_ALL_REQUESTS: os << "AllReq"; break;
    case osgDB::DatabasePager::DatabaseThread::HANDLE_NON_HTTP: os << "NonHttp"; break;
    case osgDB::DatabasePager::DatabaseThread::HANDLE_ONLY_HTTP: os << "OnlyHttp"; break;
    default: os << (int)t; break;
    }
    return os;
}

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager::DatabaseThread)

bool writePrettyHTMLImpl<osgDB::DatabasePager::DatabaseThread>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::DatabasePager::DatabaseThread * object = getObject<osgDB::DatabasePager::DatabaseThread,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            const DatabaseThreadAccess * access = (const DatabaseThreadAccess *)object;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>mode</td><td>" << access->getMode() << "</td></tr>" << std::endl;
            os << "<tr><td>active</td><td>" << (object->getActive()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>done</td><td>" << (object->getDone()?"true":"false") << "</td></tr>" << std::endl;

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

#if 0
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager::DatabaseRequest)

bool writePrettyHTMLImpl<osgDB::DatabasePager::DatabaseRequest>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::DatabasePager::DatabaseRequest * object = getObject<osgDB::DatabasePager::DatabaseRequest,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
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
#endif // 0

bool writePrettyHTMLImpl<osgDB::DatabasePager>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::DatabasePager * object = getObject<osgDB::DatabasePager,SGIItemOsg>();
    DatabasePagerAccessor * access = (DatabasePagerAccessor*)object;
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>num threads</td><td>" << object->getNumDatabaseThreads() << "</td></tr>" << std::endl;
            os << "<tr><td>threads</td><td><ul>";
            for(unsigned num = 0; num < object->getNumDatabaseThreads(); num++)
            {
                osg::ref_ptr<const osgDB::DatabasePager::DatabaseThread> thread = object->getDatabaseThread(num);
                os << "<li>" << osg_helpers::getObjectNameAndType(thread.get()) << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

            os << "<tr><td>paused</td><td>" << (object->getDatabasePagerThreadPause()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>pre-compile</td><td>" << (object->getDoPreCompile()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>accept new requests</td><td>" << (object->getAcceptNewDatabaseRequests()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>active frames</td><td>" << object->getNumFramesActive() << "</td></tr>" << std::endl;
            os << "<tr><td>target max number of page lod</td><td>" << object->getTargetMaximumNumberOfPageLOD() << "</td></tr>" << std::endl;
            os << "<tr><td>delete removed subgraphs</td><td>" << (object->getDeleteRemovedSubgraphsInDatabaseThread()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>num file requests</td><td>" << access->getLocalFileRequestListSize() << "</td></tr>" << std::endl;
            os << "<tr><td>num http requests</td><td>" << access->getHttpRequestListSize() << "</td></tr>" << std::endl;
            os << "<tr><td>num compiles</td><td>" << object->getDataToCompileListSize() << "</td></tr>" << std::endl;
            os << "<tr><td>num merges</td><td>" << object->getDataToMergeListSize() << "</td></tr>" << std::endl;
            os << "<tr><td>require update</td><td>" << (object->requiresUpdateSceneGraph()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>pre-compile</td><td>" << (object->getDoPreCompile()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>apply PBO to images</td><td>" << (object->getApplyPBOToImages()?"true":"false") << "</td></tr>" << std::endl;

            os << "<tr><td>active PagedLODs</td><td>" << access->numberOfPagedLODs() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeThreads:
        {
            os << "<ul>";
            for(unsigned n = 0; n < object->getNumDatabaseThreads(); n++)
            {
                osgDB::DatabasePager::DatabaseThread * th = object->getDatabaseThread(n);
                os << "<li>" << helpers::str_plus_number("Thread", n)  << "[id=" << th->getThreadId() << "," << th->getName() << "]: "
                    << (th->getActive()?"active":"inactive") << ", "
                    << (th->isRunning()?"running":"not running") << ", "
                    << (th->getDone()?"done":"not done")
                    << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeActivePagedLODs:
        {
            os << "<ul>";
            const DatabasePagerAccessor::SetBasedPagedLODList * list = access->activePagedLODList();
            for(DatabasePagerAccessor::SetBasedPagedLODList::const_iterator it = list->begin(); it != list->end(); it++)
            {
                const osg::observer_ptr<osg::PagedLOD> & value = *it;
                osg::ref_ptr<osg::PagedLOD> node;
                if(value.lock(node))
                {
                    os << "<li>" << osg_helpers::getObjectNameAndType(node.get()) << "</li>";
                }
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerFileRequests:
        {
            os << "<ul>";
            DatabasePagerAccessor::RequestList requestList;
            access->copyFileRequests(requestList);
            for(auto it = requestList.begin(); it != requestList.end(); it++)
            {
                const DatabasePagerAccessor::DatabaseRequestAccess * req = (const DatabasePagerAccessor::DatabaseRequestAccess *)(*it).get();
                os << "<li>" << osg_helpers::getObjectNameAndType(req) << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerHttpRequests:
        {
            os << "<ul>";
            DatabasePagerAccessor::RequestList requestList;
            access->copyHttpRequests(requestList);
            for(auto it = requestList.begin(); it != requestList.end(); it++)
            {
                const DatabasePagerAccessor::DatabaseRequestAccess * req = (const DatabasePagerAccessor::DatabaseRequestAccess *)(*it).get();
                os << "<li>" << osg_helpers::getObjectNameAndType(req) << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerDataToCompile:
        {
            os << "<ul>";
            DatabasePagerAccessor::RequestList requestList;
            access->copyDataToCompile(requestList);
            for(auto it = requestList.begin(); it != requestList.end(); it++)
            {
                const DatabasePagerAccessor::DatabaseRequestAccess * req = (const DatabasePagerAccessor::DatabaseRequestAccess *)(*it).get();
                os << "<li>" << osg_helpers::getObjectNameAndType(req) << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerDataToMerge:
        {
            os << "<ul>";
            DatabasePagerAccessor::RequestList requestList;
            access->copyDataToMerge(requestList);
            for(auto it = requestList.begin(); it != requestList.end(); it++)
            {
                const DatabasePagerAccessor::DatabaseRequestAccess * req = (const DatabasePagerAccessor::DatabaseRequestAccess *)(*it).get();
                os << "<li>" << osg_helpers::getObjectNameAndType(req) << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgDB::FileCache>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::FileCache * object = getObject<osgDB::FileCache,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>path</td><td>" << object->getFileCachePath() << "</td></tr>" << std::endl;
            const osgDB::FileCache::DatabaseRevisionsList& revlist = object->getDatabaseRevisionsList();
            os << "<tr><td>numRevisions</td><td>" << revlist.size() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeDatabaseRevisions:
        {
            os << "<ol>";
            const osgDB::FileCache::DatabaseRevisionsList& revlist = object->getDatabaseRevisionsList();
            for(osgDB::FileCache::DatabaseRevisionsList::const_iterator it = revlist.begin(); it != revlist.end(); it++)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType((*it).get()) << "</li>";
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

bool writePrettyHTMLImpl<osgDB::DatabaseRevision>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::DatabaseRevision * object = getObject<osgDB::DatabaseRevision,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>database path</td><td>" << object->getDatabasePath() << "</td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgDB::DatabaseRevisions>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::DatabaseRevisions * object = getObject<osgDB::DatabaseRevisions,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>database path</td><td>" << object->getDatabasePath() << "</td></tr>" << std::endl;
            const osgDB::DatabaseRevisions::DatabaseRevisionList& revlist = object->getDatabaseRevisionList();
            os << "<tr><td>numRevisions</td><td>" << revlist.size() << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeDatabaseRevisions:
        {
            os << "<ol>";
            const osgDB::DatabaseRevisions::DatabaseRevisionList& revlist = object->getDatabaseRevisionList();
            for(osgDB::DatabaseRevisions::DatabaseRevisionList::const_iterator it = revlist.begin(); it != revlist.end(); it++)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType((*it).get()) << "</li>";
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

bool writePrettyHTMLImpl<osgDB::FileList>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::FileList * object = getObject<osgDB::FileList,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>files</td><td><ul>";
            const osgDB::FileList::FileNames& files = object->getFileNames();
            for(osgDB::FileList::FileNames::const_iterator it = files.begin(); it != files.end(); it++)
            {
                os << "<li>" << *it << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

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

bool writePrettyHTMLImpl<osgDB::ObjectWrapper>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::ObjectWrapper * object = getObject<osgDB::ObjectWrapper,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
            os << "<tr><td>domain</td><td>" << object->getDomain() << "</td></tr>" << std::endl;
#endif
            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;
            os << "<tr><td>updated version</td><td>" << object->getUpdatedVersion() << "</td></tr>" << std::endl;

            os << "<tr><td>associates</td><td><ul>";
            const osgDB::StringList & associates = object->getAssociates();
            for(osgDB::StringList::const_iterator it = associates.begin(); it != associates.end(); it++)
            {
                os << "<li>" << *it << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
            os << "<tr><td>type list</td><td><ul>";
            const osgDB::ObjectWrapper::TypeList & typelist = object->getTypeList();
            for(osgDB::ObjectWrapper::TypeList::const_iterator it = typelist.begin(); it != typelist.end(); it++)
            {
                os << "<li>" << *it << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;
#endif

#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
            os << "<tr><td>serializers</td><td><ul>";
            const osgDB::ObjectWrapper::SerializerList & serializers = object->getSerializerList();
            for(osgDB::ObjectWrapper::SerializerList::const_iterator it = serializers.begin(); it != serializers.end(); it++)
            {
                os << "<li>" << osg_helpers::getObjectNameAndType(*it) << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;
#endif

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

bool writePrettyHTMLImpl<osgDB::ObjectWrapperManager>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::ObjectWrapperManager * object = getObject<osgDB::ObjectWrapperManager,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>wrappers</td><td><ul>";
            const osgDB::ObjectWrapperManager::WrapperMap & wrappers = object->getWrapperMap();
            for(auto it = wrappers.begin(); it != wrappers.end(); it++)
            {
                os << "<li>" << it->first << "=" << osg_helpers::getObjectNameAndType(it->second.get()) << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

            os << "<tr><td>compressors</td><td><ul>";
            const osgDB::ObjectWrapperManager::CompressorMap & compressors = object->getCompressorMap();
            for(auto it = compressors.begin(); it != compressors.end(); it++)
            {
                os << "<li>" << it->first << "=" << osg_helpers::getObjectNameAndType(it->second.get()) << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;

#if OSG_VERSION_GREATER_OR_EQUAL(3,3,0)
            os << "<tr><td>global int map</td><td><ul>";
            const osgDB::ObjectWrapperManager::IntLookupMap & intlookupmap = object->getLookupMap();
            for(auto it = intlookupmap.begin(); it != intlookupmap.end(); it++)
            {
                os << "<li>" << it->first << "</li>";
            }
            os << "</ul></td></tr>" << std::endl;
#endif

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeWrappers:
        {
            os << "<ul>";
            const osgDB::ObjectWrapperManager::WrapperMap & wrappers = object->getWrapperMap();
            for(auto it = wrappers.begin(); it != wrappers.end(); it++)
            {
                os << "<li>" << it->first << "=" << osg_helpers::getObjectNameAndType(it->second.get()) << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    case SGIItemTypeCompressors:
        {
            os << "<ul>";
            const osgDB::ObjectWrapperManager::CompressorMap & compressors = object->getCompressorMap();
            for(auto it = compressors.begin(); it != compressors.end(); it++)
            {
                os << "<li>" << it->first << "=" << osg_helpers::getObjectNameAndType(it->second.get()) << "</li>";
            }
            os << "</ul>";
            ret = true;
        }
        break;
    default:
        ret = callNextHandler(os);
        break;
    }
    return ret;
}

bool writePrettyHTMLImpl<osgDB::BaseSerializer>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    osgDB::BaseSerializer * object = getObject<osgDB::BaseSerializer,SGIItemOsg>();
    switch(itemType())
    {
    case SGIItemTypeObject:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>name</td><td>" << object->getName() << "</td></tr>" << std::endl;

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

} // namespace osg_plugin
} // namespace sgi

