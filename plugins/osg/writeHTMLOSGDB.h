#pragma once

namespace osgDB {
    class Registry;
    class Options;
    class ReaderWriter;
    class ImagePager;
    class DatabasePager;
    class FileCache;
    class DatabaseRevision;
    class DatabaseRevisions;
    class FileList;
    class ObjectWrapper;
    class ObjectWrapperManager;
    class BaseSerializer;
};

namespace sgi {
namespace osg_plugin {

WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::Registry)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::Options)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::ReaderWriter)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::ImagePager)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::DatabasePager)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::FileCache)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::DatabaseRevision)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::DatabaseRevisions)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::FileList)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::ObjectWrapper)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::ObjectWrapperManager)
WRITE_PRETTY_HTML_IMPL_DECLARE(osgDB::BaseSerializer)

} // namespace osg_plugin
} // namespace sgi
