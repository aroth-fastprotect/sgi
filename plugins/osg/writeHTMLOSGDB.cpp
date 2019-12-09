#include "stdafx.h"
#include <ostream>
#include "writeHTMLOSG.h"
#include "SGIItemOsg"

#include <osg/Version>
#include <osg/Config>
#include <osg/GL>
#include <osgDB/Registry>
#include <osgDB/ImagePager>
#define protected public
#include <osgDB/DatabasePager>
#undef protected
#include <osgDB/ObjectCache>

#include <osg/ClipNode>

#include <sgi/helpers/string>

#include <sgi/helpers/osg>
#include "osgdb_accessor.h"

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::Registry)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::Options)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::ReaderWriter)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::ImagePager)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::FileCache)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::ObjectCache)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::DatabaseRevision)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::DatabaseRevisions)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::FileList)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::ObjectWrapper)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::ObjectWrapperManager)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::BaseSerializer)

WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager::DatabaseRequest)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager::RequestQueue)
WRITE_PRETTY_HTML_IMPL_DECLARE_AND_REGISTER(osgDB::DatabasePager::ReadQueue)


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
            os << "<tr><td>dynamic libraries</td><td><ul>";
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
    case SGIItemTypeBuildInfo:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            os << "<tr><td>OSG version</td><td>" << osgGetVersion() << "</td></tr>" << std::endl;
            os << "<tr><td>OSG SO version</td><td>" << osgGetSOVersion() << "</td></tr>" << std::endl;
            os << "<tr><td>OSG library name</td><td>" << osgGetLibraryName() << "</td></tr>" << std::endl;
            os << "<tr><td>Notify disabled</td><td>";
#ifdef OSG_NOTIFY_DISABLED
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>Use float matrix</td><td>";
#ifdef OSG_USE_FLOAT_MATRIX
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>Use float plane</td><td>";
#ifdef OSG_USE_FLOAT_PLANE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>Use float bounding sphere</td><td>";
#ifdef OSG_USE_FLOAT_BOUNDINGSPHERE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>Use float bounding box</td><td>";
#ifdef OSG_USE_FLOAT_BOUNDINGBOX
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>Use ref_ptr implicit output conversion</td><td>";
#ifdef OSG_USE_REF_PTR_IMPLICIT_OUTPUT_CONVERSION
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>Use ref_ptr safe dereference</td><td>";
#ifdef OSG_USE_REF_PTR_SAFE_DEREFERENCE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>Use UTF8 filename</td><td>";
#ifdef OSG_USE_UTF8_FILENAME
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>Disable MSVC warnings</td><td>";
#ifdef OSG_DISABLE_MSVC_WARNINGS
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>Profile ReadFile</td><td>";
#ifdef OSG_PROVIDE_READFILE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>Use deprecated API</td><td>";
#ifdef OSG_USE_DEPRECATED_API
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>Environment variable support</td><td>";
#ifdef OSG_ENVVAR_SUPPORTED
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>OSG_GL1_AVAILABLE</td><td>";
#ifdef OSG_GL1_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>OSG_GL2_AVAILABLE</td><td>";
#ifdef OSG_GL2_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>OSG_GL3_AVAILABLE</td><td>";
#ifdef OSG_GL3_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>OSG_GLES1_AVAILABLE</td><td>";
#ifdef OSG_GLES1_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>OSG_GLES2_AVAILABLE</td><td>";
#ifdef OSG_GLES2_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>OSG_GLES3_AVAILABLE</td><td>";
#ifdef OSG_GLES3_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>OSG_GL_CONTEXT_VERSION</td><td>";
#ifdef OSG_GL_CONTEXT_VERSION
            os << OSG_GL_CONTEXT_VERSION;
#else
            os << "N/A";
#endif
            os << "</td></tr>" << std::endl;

            os << "<tr><td>OSG_GL_LIBRARY_STATIC</td><td>";
#ifdef OSG_GL_LIBRARY_STATIC
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>OSG_GL_DISPLAYLISTS_AVAILABLE</td><td>";
#ifdef OSG_GL_DISPLAYLISTS_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>OSG_GL_MATRICES_AVAILABLE</td><td>";
#ifdef OSG_GL_MATRICES_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>OSG_GL_VERTEX_FUNCS_AVAILABLE</td><td>";
#ifdef OSG_GL_VERTEX_FUNCS_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE</td><td>";
#ifdef OSG_GL_VERTEX_ARRAY_FUNCS_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>OSG_GL_FIXED_FUNCTION_AVAILABLE</td><td>";
#ifdef OSG_GL_FIXED_FUNCTION_AVAILABLE
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>GL_HEADER_HAS_GLINT64</td><td>";
#ifdef GL_HEADER_HAS_GLINT64
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            os << "<tr><td>GL_HEADER_HAS_GLUINT64</td><td>";
#ifdef GL_HEADER_HAS_GLUINT64
            os << "true";
#else
            os << "false";
#endif
            os << "</td></tr>" << std::endl;
            if (_table)
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
	case SGIItemTypeGraphicsContexts:
		{
			os << "<ul>";
			const osg::GraphicsContext::GraphicsContexts & list = osg::GraphicsContext::getAllRegisteredGraphicsContexts();
			for (osg::GraphicsContext::GraphicsContexts::const_iterator it = list.begin(); it != list.end(); it++)
			{
				os << "<li>" << osg_helpers::getObjectNameAndType(*it) << "</li>";
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

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgDB::Options::CacheHintOptions & t)
{
    if (t == osgDB::Options::CACHE_NONE)
        os << "none";
    else
    {
        std::vector<std::string> l;
        if (t & osgDB::Options::CACHE_ARCHIVES)
            l.push_back("archives");
        if (t & osgDB::Options::CACHE_NODES)
            l.push_back("nodes");
        if (t & osgDB::Options::CACHE_IMAGES)
            l.push_back("images");
        if (t & osgDB::Options::CACHE_HEIGHTFIELDS)
            l.push_back("heightfields");
        if (t & osgDB::Options::CACHE_OBJECTS)
            l.push_back("objects");
        if (t & osgDB::Options::CACHE_SHADERS)
            l.push_back("shaders");
        os << helpers::joinStrings(l, ',');
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgDB::Options::PrecisionHint & t)
{
    if (t == osgDB::Options::FLOAT_PRECISION_ALL)
        os << "float";
    else
    {
        std::vector<std::string> l;
        if (t & osgDB::Options::DOUBLE_PRECISION_VERTEX)
            l.push_back("double vertex");
        if (t & osgDB::Options::DOUBLE_PRECISION_NORMAL)
            l.push_back("double normal");
        if (t & osgDB::Options::DOUBLE_PRECISION_COLOR)
            l.push_back("double color");
        if (t & osgDB::Options::DOUBLE_PRECISION_SECONDARY_COLOR)
            l.push_back("double second color");
        if (t & osgDB::Options::DOUBLE_PRECISION_FOG_COORD)
            l.push_back("double fog coord");
        if (t & osgDB::Options::DOUBLE_PRECISION_TEX_COORD)
            l.push_back("double tex coord");
        if (t & osgDB::Options::DOUBLE_PRECISION_VERTEX_ATTRIB)
            l.push_back("double vertex attrib");
        os << helpers::joinStrings(l, ',');
    }
    return os;
}

std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgDB::Options::BuildKdTreesHint & t)
{
    switch (t)
    {
    case osgDB::Options::NO_PREFERENCE: os << "NO_PREFERENCE"; break;
    case osgDB::Options::DO_NOT_BUILD_KDTREES: os << "DO_NOT_BUILD_KDTREES"; break;
    case osgDB::Options::BUILD_KDTREES: os << "BUILD_KDTREES"; break;
    default: os << "unknown" << (int)t; break;
    }
    return os;
}

bool writePrettyHTMLImpl<osgDB::Options>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    OptionsAccess * object = static_cast<OptionsAccess *>(getObject<osgDB::Options, SGIItemOsg>());
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

            os << "<tr><td>authentication map</td><td>" << osg_helpers::getObjectNameAndType(object->getAuthenticationMap()) << "</td></tr>" << std::endl;

            os << "<tr><td>terrain</td><td>";
            osg::ref_ptr<osg::Node> terrain;
            object->getTerrain().lock(terrain);
            os << osg_helpers::getObjectNameAndType(terrain.get());
            os << "</td></tr>" << std::endl;

            os << "<tr><td>parent group</td><td>";
            osg::ref_ptr<osg::Group> parentGroup;
            object->getParentGroup().lock(parentGroup);
            os << osg_helpers::getObjectNameAndType(parentGroup.get());
            os << "</td></tr>" << std::endl;

            const OptionsAccess::PluginDataMap & pluginData = object->getPluginDataMap();
            os << "<tr><td>plugin data</td><td><ul>";
            for(auto it = pluginData.begin(); it != pluginData.end(); ++it)
            {
                const std::string & key = it->first;
                const void * data = it->second;
                os << "<li>" << key << "=" << data << "</li>" << std::endl;
            }
            os << "<ul></td></tr>" << std::endl;

            const OptionsAccess::PluginStringDataMap & pluginStringData = object->getPluginStringDataMap();
            os << "<tr><td>plugin string data</td><td><ul>";
            for (auto it = pluginStringData.begin(); it != pluginStringData.end(); ++it)
            {
                const std::string & key = it->first;
                const std::string & data = it->second;
                os << "<li>" << key << "=" << data << "</li>" << std::endl;
            }
            os << "<ul></td></tr>" << std::endl;

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

extern bool writePrettyHTMLImpl_OpenThreads_Thread(std::basic_ostream<char>& os, OpenThreads::Thread * object);
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

            writePrettyHTMLImpl_OpenThreads_Thread(os, object);

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

			os << "<tr><td>valid</td><td>" << (object->_valid ? "true" : "false") << "</td></tr>" << std::endl;
			os << "<tr><td>fileName</td><td>" << object->_fileName << "</td></tr>" << std::endl;
			os << "<tr><td>frame no first request</td><td>" << object->_frameNumberFirstRequest << "</td></tr>" << std::endl;
			os << "<tr><td>timestamp first request</td><td>" << object->_timestampFirstRequest << "</td></tr>" << std::endl;
			os << "<tr><td>priority first request</td><td>" << object->_priorityFirstRequest << "</td></tr>" << std::endl;

			os << "<tr><td>frame no last request</td><td>" << object->_frameNumberLastRequest << "</td></tr>" << std::endl;
			os << "<tr><td>timestamp last request</td><td>" << object->_timestampLastRequest << "</td></tr>" << std::endl;
			os << "<tr><td>priority last request</td><td>" << object->_priorityLastRequest << "</td></tr>" << std::endl;

			os << "<tr><td>num requests</td><td>" << object->_numOfRequests << "</td></tr>" << std::endl;

			os << "<tr><td>terrain</td><td>" << osg_helpers::getObjectNameAndType(object->_terrain.get(), true) << "</td></tr>" << std::endl;
			os << "<tr><td>group</td><td>" << osg_helpers::getObjectNameAndType(object->_group.get(), true) << "</td></tr>" << std::endl;

			os << "<tr><td>loadedModel</td><td>" << osg_helpers::getObjectNameAndType(object->_loadedModel.get(), true) << "</td></tr>" << std::endl;
			os << "<tr><td>loadOptions</td><td>" << osg_helpers::getObjectNameAndType(object->_loadOptions.get(), true) << "</td></tr>" << std::endl;
			os << "<tr><td>objectCache</td><td>" << osg_helpers::getObjectNameAndType(object->_objectCache.get()) << "</td></tr>" << std::endl;

			os << "<tr><td>ICO compile set</td><td>" << osg_helpers::getObjectNameAndType(object->_compileSet.get()) << "</td></tr>" << std::endl;

			os << "<tr><td>groupExpired</td><td>" << (object->_groupExpired ? "true" : "false") << "</td></tr>" << std::endl;

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


bool writePrettyHTMLImpl<osgDB::DatabasePager::RequestQueue>::process(std::basic_ostream<char>& os)
{
	bool ret = false;
	osgDB::DatabasePager::RequestQueue * object = getObject<osgDB::DatabasePager::RequestQueue, SGIItemOsg>();
	switch (itemType())
	{
	case SGIItemTypeObject:
	{
		if (_table)
			os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

		// add object properties first
		callNextHandler(os);

		os << "<tr><td>size</td><td>" << object->size() << "</td></tr>" << std::endl;
		os << "<tr><td>frame no last pruned</td><td>" << object->_frameNumberLastPruned << "</td></tr>" << std::endl;

		if (_table)
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


bool writePrettyHTMLImpl<osgDB::DatabasePager::ReadQueue>::process(std::basic_ostream<char>& os)
{
	bool ret = false;
	osgDB::DatabasePager::ReadQueue * object = getObject<osgDB::DatabasePager::ReadQueue, SGIItemOsg>();
	switch (itemType())
	{
	case SGIItemTypeObject:
	{
		if (_table)
			os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

		// add object properties first
		callNextHandler(os);

		os << "<tr><td>name</td><td>" << object->_name << "</td></tr>" << std::endl;
		os << "<tr><td>block</td><td>" << osg_helpers::getObjectNameAndType(object->_block.get()) << "</td></tr>" << std::endl;

		if (_table)
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

void writePrettyHTMLImpl_DatabaseRequest(std::basic_ostream<char>& os, const DatabasePagerAccessor::DatabaseRequestAccess * req, bool brief=true)
{
    if (brief)
    {
        os << "<li>";
        if (req)
        {
            os << osg_helpers::getObjectNameAndType(req) << "&nbsp;";
            if (!req->_valid)
                os << "invalid";
            else
            {
                os << req->_fileName << "&nbsp;" << req->_frameNumberLastRequest << '/' << req->_timestampLastRequest << '@' << req->_priorityLastRequest << '#' << req->_numOfRequests;
            }
        }
        else
            os << "(null)";
        os << "</li>";
    }
    else
    {
        os << "<tr>";
        if (req)
        {
            os << "<td>" << osg_helpers::getObjectNameAndType(req) << "</td>";
            if (!req->_valid)
            {
                os << "<td>invalid</td>";
                os << "<td>-1</td>";
                os << "<td>0</td>";
                os << "<td>-1</td>";
                os << "<td>0</td>";
            }
            else
            {
                os << "<td>" << req->_fileName << "</td>";
                os << "<td>" << req->_frameNumberLastRequest << "</td>";
                os << "<td>" << req->_timestampLastRequest << "</td>";
                os << "<td>" << req->_priorityLastRequest << "</td>";
                os << "<td>" << req->_numOfRequests << "</td>";
            }
        }
        else
        {
            os << "<td>(null)</td>";
            os << "<td></td>";
            os << "<td>-1</td>";
            os << "<td>0</td>";
            os << "<td>-1</td>";
            os << "<td>0</td>";
        }
        os << "</tr>";
    }
}

void writePrettyHTMLImpl_DatabaseRequestList(std::basic_ostream<char>& os, const DatabasePagerAccessor::RequestList & requestList, bool brief=true)
{
    if(requestList.empty())
        os << "<i>empty</i>";
    else
    {
        if(brief)
            os << "<ul>";
		else
		{
			os << requestList.size() << "&nbsp;requests<br/>";
			os << "<table border=\'1\' align=\'left\'><tr><th>object</th><th>filename</th><th>frame</th><th>time</th><th>prio</th><th>numReq</th></tr>" << std::endl;
		}
        for(auto it = requestList.begin(); it != requestList.end(); it++)
        {
            const DatabasePagerAccessor::DatabaseRequestAccess * req = static_cast<const DatabasePagerAccessor::DatabaseRequestAccess*>((*it).get());
            writePrettyHTMLImpl_DatabaseRequest(os, req, brief);
        }
        if(brief)
            os << "</ul>" << std::endl;
        else
            os << "</table>" << std::endl;
    }
}

bool writePrettyHTMLImpl<osgDB::DatabasePager>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    DatabasePagerAccessor* object = static_cast<DatabasePagerAccessor*>(getObject<osgDB::DatabasePager,SGIItemOsg>());
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
            os << "<tr><td>num file requests</td><td>" << object->getLocalFileRequestListSize() << "</td></tr>" << std::endl;
            os << "<tr><td>num http requests</td><td>" << object->getHttpRequestListSize() << "</td></tr>" << std::endl;
            os << "<tr><td>num compiles</td><td>" << object->getDataToCompileListSize() << "</td></tr>" << std::endl;
            os << "<tr><td>num merges</td><td>" << object->getDataToMergeListSize() << "</td></tr>" << std::endl;
            os << "<tr><td>requestsInProgress</td><td>" << (object->getRequestsInProgress()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>requiresRedraw</td><td>" << (object->requiresRedraw()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>min/max time to merge tile</td><td>" << object->getMinimumTimeToMergeTile() << "/" << object->getMaximumTimeToMergeTile() << "</td></tr>" << std::endl;
            os << "<tr><td>avg time to merge tile</td><td>" << object->getAverageTimeToMergeTiles() << "</td></tr>" << std::endl;
            os << "<tr><td>require update</td><td>" << (object->requiresUpdateSceneGraph()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>apply PBO to images</td><td>" << (object->getApplyPBOToImages()?"true":"false") << "</td></tr>" << std::endl;
            os << "<tr><td>drawablePolicy</td><td>" << object->getDrawablePolicy() << "</td></tr>" << std::endl;

            os << "<tr><td>active PagedLODs</td><td>" << object->numberOfPagedLODs() << "</td></tr>" << std::endl;

			bool changeAutoUnRef = false, valueAutoUnRef = false;
			object->getUnrefImageDataAfterApplyPolicy(changeAutoUnRef, valueAutoUnRef);
			os << "<tr><td>unref image data after apply</td><td>" << (changeAutoUnRef ? "true" : "false") << "/" << (valueAutoUnRef ? "true" : "false") << "</td></tr>" << std::endl;
			
			bool changeAnisotropy = false;
			float valueAnisotropy = 0;
			object->getMaxAnisotropyPolicy(changeAnisotropy, valueAnisotropy);
			os << "<tr><td>max anisotropy policy</td><td>" << (changeAnisotropy ? "true" : "false") << "/" << valueAnisotropy << "</td></tr>" << std::endl;

            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeThreads:
        {
            if(_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Id</th><th>Name</th><th>Mode</th><th>State</th></tr>" << std::endl;
            for(unsigned n = 0; n < object->getNumDatabaseThreads(); n++)
            {
                DatabaseThreadAccess * th = static_cast<DatabaseThreadAccess*>(object->getDatabaseThread(n));
                os << "<tr><td>#" << n << "/" << th->getThreadId() << "</td>";
                os << "<td>" << th->getName() << "</td>";
                os << "<td>" << th->getMode() << "</td>";
                os << "<td>"
                    << (th->getActive()?"active":"inactive") << ", "
                    << (th->isRunning()?"running":"not running") << ", "
                    << (th->getDone()?"done":"not done")
                    << "</td>";
                os << "</tr>";
            }
            if(_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeActivePagedLODs:
        {
            os << "<ul>";
            osgDB::DatabasePager::PagedLODList * activePagedLODList = object->activePagedLODList();
            const DatabasePagerAccessor::SetBasedPagedLODList * list = static_cast<const DatabasePagerAccessor::SetBasedPagedLODList *>(activePagedLODList);
            for (DatabasePagerAccessor::SetBasedPagedLODList::const_iterator it = list->begin(); it != list->end(); it++)
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
            DatabasePagerAccessor::RequestList requestList;
            object->copyFileRequests(requestList);
            writePrettyHTMLImpl_DatabaseRequestList(os, requestList, false);
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerHttpRequests:
        {
            DatabasePagerAccessor::RequestList requestList;
            object->copyHttpRequests(requestList);
            writePrettyHTMLImpl_DatabaseRequestList(os, requestList, false);
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerDataToCompile:
        {
            DatabasePagerAccessor::RequestList requestList;
            object->copyDataToCompile(requestList);
            writePrettyHTMLImpl_DatabaseRequestList(os, requestList, false);
            ret = true;
        }
        break;
    case SGIItemTypeDBPagerDataToMerge:
        {
            DatabasePagerAccessor::RequestList requestList;
            object->copyDataToMerge(requestList);
            writePrettyHTMLImpl_DatabaseRequestList(os, requestList, false);
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

bool writePrettyHTMLImpl<osgDB::ObjectCache>::process(std::basic_ostream<char>& os)
{
    bool ret = false;
    ObjectCacheAccess * object = static_cast<ObjectCacheAccess *>(getObject<osgDB::ObjectCache, SGIItemOsg>());
    switch (itemType())
    {
    case SGIItemTypeObject:
        {
            if (_table)
                os << "<table border=\'1\' align=\'left\'><tr><th>Field</th><th>Value</th></tr>" << std::endl;

            // add object properties first
            callNextHandler(os);

            os << "<tr><td>num items</td><td>" << object->getNumItems() << "</td></tr>" << std::endl;

            if (_table)
                os << "</table>" << std::endl;
            ret = true;
        }
        break;
    case SGIItemTypeCachedObjects:
        {
            ObjectCacheAccess::ItemList items;
            object->getItems(items);
            if (itemNumber() == ~0u)
            {

                os << "<table border=\'1\' align=\'left\'><tr><th>Name</th><th>Object</th><th>Timestamp</th></tr>" << std::endl;
                for (const auto & item : items)
                {
                    os << "<tr><td>" << item.name;
                    if (item.options.valid())
                        os << "&nbsp;" << osg_helpers::getObjectNameAndType(item.options.get());
                    os << "</td><td>";
                    os << osg_helpers::getObjectNameAndType(item.object.get()) << "</td><td>" << item.timestamp << "</td></tr>" << std::endl;
                }
                os << "</table>" << std::endl;
            }
            else if (itemNumber() < items.size())
            {
                const auto & item = items[itemNumber()];
                os << "<table border=\'1\' align=\'left\'>" << std::endl;
                os << "<tr><td>name</td><td>" << item.name << "</td></tr>" << std::endl;
                os << "<tr><td>object</td><td>" << osg_helpers::getObjectNameAndType(item.object) << "</td></tr>" << std::endl;
                os << "<tr><td>options</td><td>" << osg_helpers::getObjectNameAndType(item.options) << "</td></tr>" << std::endl;
                os << "<tr><td>timestamp</td><td>" << item.timestamp << "</td></tr>" << std::endl;
                os << "</table>" << std::endl;
            }
            else
            {
                os << "Cached object #" << itemNumber() << " does not exist." << std::endl;
            }
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

#if OSG_VERSION_GREATER_OR_EQUAL(3,5,0)
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgDB::ObjectWrapperAssociate & a)
{
    os << "firstVersion " << a._firstVersion << ", lastVersion " << a._lastVersion << " " << a._name;
    return os;
}
#endif

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
#if OSG_VERSION_GREATER_OR_EQUAL(3,5,0)
            const osgDB::ObjectWrapper::RevisionAssociateList & associates = object->getAssociates();
            for(osgDB::ObjectWrapper::RevisionAssociateList::const_iterator it = associates.begin(); it != associates.end(); it++)
            {
                os << "<li>" << *it << "</li>";
            }
#else
            const osgDB::StringList & associates = object->getAssociates();
            for(osgDB::StringList::const_iterator it = associates.begin(); it != associates.end(); it++)
            {
                os << "<li>" << *it << "</li>";
            }
#endif
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

unsigned ObjectCacheAccess::getNumItems()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_objectCacheMutex);
    return _objectCache.size();
}

void ObjectCacheAccess::getItems(ItemList & items)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_objectCacheMutex);
    unsigned num = _objectCache.size();
    items.resize(num);
    unsigned idx = 0;
    for (auto it = _objectCache.begin(); it != _objectCache.end(); ++it, ++idx)
    {
        Item & item = items[idx];
#if OSG_MIN_VERSION_REQUIRED(3,5,0)
        const FileNameOptionsPair & filename_options = it->first;
        item.name = filename_options.first;
        item.options = filename_options.second;
#else
        item.name = it->first;
        item.options = nullptr;
#endif
        item.object = it->second.first;
        item.timestamp = it->second.second;
    }
}


} // namespace osg_plugin
} // namespace sgi

