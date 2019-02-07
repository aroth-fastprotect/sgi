#include "stdafx.h"
#include "ShaderEditorDialog.h"

#include <sgi/plugins/SGISettingsDialogImpl>
#include <sgi/plugins/SGIItemOsg>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/helpers/qt>
#include <sgi/helpers/osg>

#include <QTemporaryFile>
#include <QPushButton>
#include <QTextStream>
#include <QTimer>
#include <QTableView>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include <ui_ShaderEditorDialog.h>

#include <osgEarth/VirtualProgram>
#include <osg/Node>
#include "osgearth_accessor.h"

#include "ShaderFunctionDetails.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

namespace osgearth_plugin {
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ShaderComp::FunctionLocation & t);
} // namespace osgearth_plugin

using namespace osgearth_plugin;

namespace {
    QVector2D convertToQt(const osg::Vec2f & v)
    {
        return QVector2D(v.x(), v.y());
    }
    QVector2D convertToQt(const osg::Vec2d & v)
    {
        return QVector2D(v.x(), v.y());
    }
    QVector3D convertToQt(const osg::Vec3f & v)
    {
        return QVector3D(v.x(), v.y(), v.z());
    }
    QVector3D convertToQt(const osg::Vec3d & v)
    {
        return QVector3D(v.x(), v.y(), v.z());
    }
    QVector4D convertToQt(const osg::Vec4f & v)
    {
        return QVector4D(v.x(), v.y(), v.z(), v.w());
    }
    QVector4D convertToQt(const osg::Vec4d & v)
    {
        return QVector4D(v.x(), v.y(), v.z(), v.w());
    }
}

InfoLogDock::InfoLogDock(ShaderEditorDialog * parent)
    : QDockWidget (parent)
    , _shaderEditor(parent)
    , _log(nullptr)
    , _timer(nullptr)
{
    setWindowTitle(tr("Info log"));
    _log = new QTextEdit(this);
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &InfoLogDock::onTimer);
    _timer->setInterval(2000);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setWidget(_log);
    update();
}

InfoLogDock::~InfoLogDock()
{
    delete _timer;
}

void InfoLogDock::showEvent(QShowEvent * event)
{
    QDockWidget::showEvent(event);
    _timer->start();
}

void InfoLogDock::hideEvent(QHideEvent * event)
{
    _timer->stop();
    QDockWidget::hideEvent(event);
}

void InfoLogDock::update()
{
    QString str;
    QTextStream ts(&str);
    bool hasWarning = false;
    ts << "Info log:<br/>";
    if(!_infoLog.empty())
    {
        ts << "<pre>" << QString::fromStdString(_infoLog) << "</pre>";
        hasWarning = true;
    }
    else
        ts << QStringLiteral("<i>empty</i>");
    ts << "<br/>";
    ts << "Shader log file:&nbsp;" << _logFilename << "<br/>";
    if(!_logFileData.isEmpty())
    {
        ts << "<pre>" << _logFileData << "</pre>";
    }
    else
        ts << QStringLiteral("<i>empty</i>");
    if(_logFileData.isEmpty() && _infoLog.empty())

    setWindowIcon(hasWarning ? QIcon::fromTheme("dialog-warning") : QIcon::fromTheme("dialog-ok") );
    _log->setHtml(str);
}

void InfoLogDock::onTimer()
{
    _logFilename = _shaderEditor->shaderLogFile();
    bool hasUpdate = false;
    if(_logFilename.isEmpty())
    {
        if(!_logFileData.isEmpty())
        {
            _logFileData.clear();
            hasUpdate = true;
        }
    }
    else
    {
        QFile f(_logFilename);
        if(f.open(QFile::ReadOnly))
        {
            QByteArray data = f.readAll();
            if(_logFileData.size() != data.size())
            {
                _logFileData = data;
                hasUpdate = true;
            }
            f.close();
        }
    }
    if(hasUpdate)
        update();
}

void InfoLogDock::setInfoLog(const std::string & log)
{
    if(_infoLog.size() != log.size())
    {
        _infoLog = log;
        update();
    }
}

#define uniformToQVariant_Data(__gl_type, __c_type) \
    case osg::Uniform::__gl_type: \
        { \
            __c_type value = __c_type(); \
            if (object->get(value)) \
                ret = value; \
        } \
        break

#define uniformToQVariant_Vector(__gl_type, __c_type, __q_type) \
    case osg::Uniform::__gl_type: \
        { \
            __c_type value = __c_type(); \
            if (object->get(value)) { \
                __q_type qvalue = convertToQt(value); \
                ret = qvalue; \
            }\
        } \
        break

#define uniformToQVariant_Sampler(__gl_type) \
    case osg::Uniform::__gl_type: \
        { \
        } \
        break

namespace {
    QVariant uniformToQVariant(const osg::Uniform * object)
    {
        QVariant ret;
        switch (object->getType())
        {
        case osg::Uniform::UNDEFINED:
            break;
        uniformToQVariant_Data(BOOL, bool);
        uniformToQVariant_Data(FLOAT, float);
        uniformToQVariant_Vector(FLOAT_VEC2, osg::Vec2f, QVector2D);
        uniformToQVariant_Vector(FLOAT_VEC3, osg::Vec3f, QVector3D);
        uniformToQVariant_Vector(FLOAT_VEC4, osg::Vec4f, QVector4D);
        uniformToQVariant_Data(DOUBLE, double);
        uniformToQVariant_Vector(DOUBLE_VEC2, osg::Vec2d, QVector2D);
        uniformToQVariant_Vector(DOUBLE_VEC3, osg::Vec3d, QVector3D);
        uniformToQVariant_Vector(DOUBLE_VEC4, osg::Vec4d, QVector4D);
        uniformToQVariant_Data(INT, int);
        //uniformToQVariant_Data(INT_VEC2, osg::Vec2i);
        //uniformToQVariant_Data(INT_VEC3, osg::Vec3i);
        //uniformToQVariant_Data(INT_VEC4, osg::Vec4i);
        uniformToQVariant_Data(UNSIGNED_INT, unsigned int);
        //uniformToQVariant_Data(UNSIGNED_INT_VEC2, osg::Vec2ui);
        //uniformToQVariant_Data(UNSIGNED_INT_VEC3, osg::Vec3ui);
        //uniformToQVariant_Data(UNSIGNED_INT_VEC4, osg::Vec4ui);
        uniformToQVariant_Sampler(SAMPLER_1D);
        uniformToQVariant_Sampler(SAMPLER_2D);
        uniformToQVariant_Sampler(SAMPLER_3D);
        uniformToQVariant_Sampler(SAMPLER_CUBE);
        uniformToQVariant_Sampler(SAMPLER_1D_SHADOW);
        uniformToQVariant_Sampler(SAMPLER_2D_SHADOW);
        uniformToQVariant_Sampler(SAMPLER_1D_ARRAY);
        uniformToQVariant_Sampler(SAMPLER_2D_ARRAY);
        uniformToQVariant_Sampler(SAMPLER_CUBE_MAP_ARRAY);
        uniformToQVariant_Sampler(SAMPLER_1D_ARRAY_SHADOW);
        uniformToQVariant_Sampler(SAMPLER_2D_ARRAY_SHADOW);
        uniformToQVariant_Sampler(SAMPLER_2D_MULTISAMPLE);
        uniformToQVariant_Sampler(SAMPLER_2D_MULTISAMPLE_ARRAY);
        uniformToQVariant_Sampler(SAMPLER_CUBE_SHADOW);
        uniformToQVariant_Sampler(SAMPLER_CUBE_MAP_ARRAY_SHADOW);
        uniformToQVariant_Sampler(SAMPLER_BUFFER);
        uniformToQVariant_Sampler(SAMPLER_2D_RECT);
        uniformToQVariant_Sampler(SAMPLER_2D_RECT_SHADOW);
        default:
            break;
        }
        return ret;
    }
} // namespace

class UniformModel::Private
{
public:
    ShaderEditorDialog * dialog;
    struct Item
    {
        Item()
        {
        }

        unsigned index;
        QString name;
        QString type;
        QString value;
        osg::StateAttribute::OverrideValue overrideValue;
        osg::observer_ptr<osg::Uniform> uniform;
    };
    std::vector<Item> items;
    void add(unsigned i, const std::string & s, osg::StateSet::RefUniformPair & pair)
    {
        osg::Uniform * u = pair.first.get();
        Item item;
        item.index = i;
        item.name = QString::fromStdString(s);
        item.overrideValue = pair.second;
        item.type = QString::fromStdString(osg::Uniform::getTypename(u->getType()));
        item.value = QString::fromStdString(osg_helpers::uniformToString(u));
        item.uniform = u;
        items.push_back(item);
    }
    Private(ShaderEditorDialog * d)
        : dialog(d) {}
};

UniformModel::UniformModel(ShaderEditorDialog * parent)
    : QAbstractItemModel(parent)
    , m_impl(new Private(parent))
{
}

UniformModel::~UniformModel()
{
    delete m_impl;
}

void UniformModel::reload()
{
    beginResetModel();
    m_impl->items.clear();
    if (m_impl->dialog)
    {
        osg::StateSet * stateSet = m_impl->dialog->getStateSet();
        if (stateSet)
        {
            osg_helpers::UniformList uniformList;
            osg_helpers::collectUniformList(stateSet, uniformList);

            unsigned index = 0;
            for (auto it : uniformList)
                m_impl->add(index++, it.first, it.second);
        }
    }
    endResetModel();
}

Qt::ItemFlags UniformModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags ret = 0;
    switch ((Section)index.column())
    {
    case SectionName:
    case SectionType:
        ret = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        break;
    case SectionValue: 
        ret = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
        break;
    }

    return ret;
}

QModelIndex UniformModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex ret;
    if (!parent.isValid())
    {
        const Private::Item * item = (row >= 0 && row < (int)m_impl->items.size()) ? &m_impl->items[row] : NULL;
        if (item)
            ret = createIndex(row, column, (void*)item);
    }
    return ret;
}

QModelIndex UniformModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int UniformModel::rowCount(const QModelIndex &parent) const
{
    int ret = 0;
    if (parent.column() > 0)
        ret = 0;
    else if (!parent.isValid())
        ret = m_impl->items.size();
    else
        ret = 0;
    return ret;
}

int UniformModel::columnCount(const QModelIndex &parent) const
{
    return SectionCount + 1;
}


QVariant UniformModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant ret;
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case SectionName: ret = tr("Name"); break;
        case SectionType: ret = tr("Type"); break;
        case SectionValue: ret = tr("Value"); break;
        }
    }
    return ret;
}

QVariant UniformModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;
    const Private::Item * item = (const Private::Item *)index.internalPointer();
    if (item)
    {
        switch (role)
        {
        case Qt::DisplayRole:
            switch ((Section)index.column())
            {
            case SectionName: ret = item->name; break;
            case SectionValue: ret = item->value; break;
            case SectionType: ret = item->type; break;
            }
            break;
        case Qt::EditRole:
            switch ((Section)index.column())
            {
            case SectionName: ret = item->name; break;
            case SectionValue: ret = item->value; break;
            case SectionType: ret = item->type; break;
            }
            break;
        }
    }
    return ret;
}

bool UniformModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ret = false;;
    Private::Item * item = (Private::Item *)index.internalPointer();
    if (item)
    {
        switch (role)
        {
        case Qt::EditRole:
            switch ((Section)index.column())
            {
            case SectionValue: 
                {
                    osg::ref_ptr<osg::Uniform> uniform;
                    if (item->uniform.lock(uniform))
                    {
                        std::string s = value.toString().toStdString();
                        ret = osg_helpers::stringToUniform(s, uniform.get());
                        if (ret)
                        {
                            item->value = QString::fromStdString(osg_helpers::uniformToString(uniform.get()));
                        }
                    }
                }
                break;
            }
            break;
        }
    }
    return ret;
}

UniformEditDock::UniformEditDock(ShaderEditorDialog * parent)
    : QDockWidget(parent)
    , _table(nullptr)
    , _model(nullptr)
{
    setWindowTitle(tr("Uniforms"));
    _model = new UniformModel(parent);
    _table = new QTableView(this);
    _table->setSortingEnabled(false);
    _table->setModel(_model);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setWidget(_table);
}

UniformEditDock::~UniformEditDock()
{
}

void UniformEditDock::reload()
{
    _model->reload();
    _table->resizeColumnsToContents();
}

namespace  {
    static const char * default_vertex_shader =
            "#version " GLSL_VERSION_STR "\n"
#ifdef OSG_GLES2_AVAILABLE
            "precision mediump float; \n"
#endif
            "void my_vertex(inout vec4 VertexView) \n"
            "{ \n"
            "} \n";
    static const char * default_fragment_shader =
            "#version " GLSL_VERSION_STR "\n"
#ifdef OSG_GLES2_AVAILABLE
           "precision mediump float; \n"
#endif
           "#pragma import_defines(OE_IS_PICK_CAMERA) \n"
           "void my_fragment( inout vec4 color ) \n"
           "{ \n"
           "#ifndef OE_IS_PICK_CAMERA \n"
           "    color.r *= 0.5; \n"
           "    color.g *= 0.2; \n"
           "    color.b *= 0.2; \n"
           "#endif \n"
           "} \n";

}

namespace  {
    class ShaderAccess : public osg::Shader {
    public:
        /** Query InfoLog from a glProgram */
        bool getGlProgramInfoLog(unsigned int contextID, std::string& log) const
        {
            for (unsigned i = 0; i < _pcsList.size(); ++i)
            {
                const osg::Shader::ShaderObjects * obj = _pcsList[i].get();

                if(obj && obj->_contextID == contextID)
                {
                    const osg::Shader::PerContextShader * pcs = obj->_perContextShaders[contextID].get();
                    if(pcs)
                        return pcs->getInfoLog(log);
                    break;
                }

            }
            return false;
        }
    };
}

unsigned hashFunctionName(const osgEarth::ShaderComp::FunctionLocation & loc, float order, const std::string & name)
{
    return qHash((int)loc) ^ qHash(order) ^ qHashBits(name.c_str(), name.size());
}

#define AFM_ACTION(__def) { \
    QAction * a = new QAction(#__def, this); \
    a->setData(QVariant::fromValue((int)osgEarth::ShaderComp::__def)); \
    a->setActionGroup(addFunctionGroup); \
    addFunctionMenu->addAction(a);\
     }

ShaderEditorDialog::ShaderEditorDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info)
    : SettingsQMainWindowImpl(parent, hostInterface, item, info)
    , _hostCallback(hostInterface->hostCallback())
    , ui(new Ui_ShaderEditorDialog)
    , _infoLogDock(nullptr)
    , _uniformEditDock(nullptr)
    , _ready(false)
    , _currentVPFunctionIndex(-1)
    , _currentProgShaderIndex(-1)
    , _originalLogFile()
    , _originalLogFileEnabled(false)
    , _tmpShaderLog(nullptr)
{
    ui->setupUi( this );

    _infoLogDock = new InfoLogDock(this);
    _uniformEditDock = new UniformEditDock(this);
    addDockWidget(Qt::RightDockWidgetArea, _infoLogDock);
    addDockWidget(Qt::RightDockWidgetArea, _uniformEditDock);
    tabifyDockWidget(_infoLogDock, _uniformEditDock);

    connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &ShaderEditorDialog::close);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ShaderEditorDialog::apply);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &ShaderEditorDialog::reset);
    ui->buttonBox->button(QDialogButtonBox::Reset)->setText(tr("Create Default"));
    connect(ui->buttonBox->button(QDialogButtonBox::Discard), &QPushButton::clicked, this, &ShaderEditorDialog::reload);
    ui->buttonBox->button(QDialogButtonBox::Discard)->setText(tr("Reload"));

    QMenu * addFunctionMenu = new QMenu(this);
    QActionGroup * addFunctionGroup = new QActionGroup(this);
    AFM_ACTION(LOCATION_VERTEX_MODEL);
    AFM_ACTION(LOCATION_VERTEX_VIEW);
    AFM_ACTION(LOCATION_VERTEX_CLIP);
    AFM_ACTION(LOCATION_TESS_CONTROL);
    AFM_ACTION(LOCATION_TESS_EVALUATION);
    AFM_ACTION(LOCATION_GEOMETRY);
    AFM_ACTION(LOCATION_FRAGMENT_COLORING);
    AFM_ACTION(LOCATION_FRAGMENT_LIGHTING);
    AFM_ACTION(LOCATION_FRAGMENT_OUTPUT);
    connect(addFunctionGroup, &QActionGroup::triggered, this, &ShaderEditorDialog::vpFunctionAdd);
    ui->vpFunctionAdd->setMenu(addFunctionMenu);

    std::string name;
    _hostInterface->getObjectDisplayName(name, _item);
    qt_helpers::QtSGIItem data(_item.get());
    ui->objectComboBox->addItem(qt_helpers::fromUtf8(name), QVariant::fromValue(data));

    VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram());
    if(vp)
    {
        _originalLogFile = vp->getShaderLogFile();
        _originalLogFileEnabled = vp->getShaderLogging();
        if(!_originalLogFileEnabled || _originalLogFile.empty())
        {
            _tmpShaderLog = new QTemporaryFile();
            // open and then close the temp file; it remains viable until the dtor of QTemporaryFile is called
            if(_tmpShaderLog->open())
            {
                _shaderLogFile = _tmpShaderLog->fileName();
                vp->setShaderLogging(true, _shaderLogFile.toStdString());
                dirty();
                _tmpShaderLog->close();
            }
        }
        else {
            _shaderLogFile = QString::fromStdString(_originalLogFile);
        }
    }

    load();
    _ready = true;
}

ShaderEditorDialog::~ShaderEditorDialog()
{
    VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram());
    if(vp)
    {
        vp->setShaderLogging(_originalLogFileEnabled, _originalLogFile);
    }
    delete _tmpShaderLog;
    delete ui;
    ui = nullptr;
}

osg::StateSet * ShaderEditorDialog::getStateSet(bool create)
{
    osg::StateSet * stateSet = getObject<osg::StateSet, SGIItemOsg, DynamicCaster>();
    osg::StateAttribute * stateAttr = getObject<osg::StateAttribute, SGIItemOsg, DynamicCaster>();
    osg::Node * node = getObject<osg::Node, SGIItemOsg, DynamicCaster>();
    if(!stateSet && stateAttr)
    {
        stateSet = stateAttr->getParent(0);
    }
    else if (!stateSet && node)
    {
        if (create)
            stateSet = node->getOrCreateStateSet();
        else
            stateSet = node->getStateSet();
    }
    return stateSet;
}

osgEarth::VirtualProgram * ShaderEditorDialog::getVirtualProgram(bool create)
{
    osgEarth::VirtualProgram * ret = nullptr;
    osg::StateAttribute * stateAttr = getObject<osg::StateAttribute, SGIItemOsg, DynamicCaster>();
    if(stateAttr)
    {
        ret = dynamic_cast<osgEarth::VirtualProgram*>(stateAttr);
    }
    else
    {
        osg::StateSet * stateSet = getStateSet(create);
        if (stateSet)
        {
            if(create)
                ret = osgEarth::VirtualProgram::getOrCreate(stateSet);
            else
                ret = osgEarth::VirtualProgram::get(stateSet);
        }
    }
    return ret;
}

osg::Program * ShaderEditorDialog::getProgram(bool create)
{
    osg::Program * ret = nullptr;
    osg::StateAttribute * stateAttr = getObject<osg::StateAttribute, SGIItemOsg, DynamicCaster>();
    if(stateAttr)
    {
        ret = dynamic_cast<osg::Program*>(stateAttr);
    }
    else
    {
        osg::StateSet * stateSet = getStateSet(create);
        if (stateSet)
        {
            osg::StateAttribute * p = stateSet->getAttribute(osg::StateAttribute::PROGRAM);
            ret = dynamic_cast<osg::Program*>(p);
            if(create)
            {
                if(!p)
                {
                    ret = new osg::Program;
                    stateSet->setAttribute(ret, osg::StateAttribute::ON);
                }
            }
        }
    }
    return ret;
}

osgEarth::PolyShader * ShaderEditorDialog::getPolyShader(int index)
{
    osgEarth::PolyShader * ret = nullptr;
    uint hash = ui->vpFunction->itemData(index).toUInt();
    osgEarth::VirtualProgram * vp_ = getVirtualProgram(false);
    if (vp_)
    {
        VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(vp_);
        osgEarth::ShaderComp::FunctionLocationMap funcs;
        vp->getFunctions(funcs);

        for (auto it = funcs.begin(); it != funcs.end(); ++it)
        {
            const osgEarth::ShaderComp::FunctionLocation & loc = it->first;
            const osgEarth::ShaderComp::OrderedFunctionMap & map = it->second;
            for (auto it = map.begin(); it != map.end(); ++it)
            {
                const float & order = it->first;
                const osgEarth::ShaderComp::Function & func = it->second;
                unsigned h = hashFunctionName(loc, order, func._name);
                if (h == hash)
                {
                    ret = vp->getPolyShader(func._name);
                }
            }
        }
    }
    return ret;
}

const QString & ShaderEditorDialog::shaderLogFile() const
{
    return _shaderLogFile;
}

bool ShaderEditorDialog::removeVPShader(int index)
{
    bool ret = false;
    uint hash = ui->vpFunction->itemData(index).toUInt();
    osgEarth::VirtualProgram * vp_ = getVirtualProgram(false);
    if (vp_)
    {
        VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(vp_);
        osgEarth::ShaderComp::FunctionLocationMap funcs;
        vp->getFunctions(funcs);

        for (auto it = funcs.begin(); it != funcs.end(); ++it)
        {
            const osgEarth::ShaderComp::FunctionLocation & loc = it->first;
            const osgEarth::ShaderComp::OrderedFunctionMap & map = it->second;
            for (auto it = map.begin(); it != map.end(); ++it)
            {
                const float & order = it->first;
                const osgEarth::ShaderComp::Function & func = it->second;
                unsigned h = hashFunctionName(loc, order, func._name);
                if (h == hash)
                {
                    vp->removeShader(func._name);
                    ret = true;
                }
            }
        }
    }
    return ret;
}

void ShaderEditorDialog::apply()
{
    if (!_ready)
        return;

    osgEarth::VirtualProgram * vp_ = getVirtualProgram(true);
    if (vp_ && _currentVPFunctionIndex >= 0)
    {
        VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(vp_);
        uint hash = ui->vpFunction->itemData(_currentVPFunctionIndex).toUInt();
        if (vp_)
        {
            osgEarth::ShaderComp::FunctionLocationMap funcs;
            vp->getFunctions(funcs);

            for (auto it = funcs.begin(); it != funcs.end(); ++it)
            {
                const osgEarth::ShaderComp::FunctionLocation & loc = it->first;
                const osgEarth::ShaderComp::OrderedFunctionMap & map = it->second;
                for (auto it = map.begin(); it != map.end(); ++it)
                {
                    const float & order = it->first;
                    const osgEarth::ShaderComp::Function & func = it->second;
                    unsigned h = hashFunctionName(loc, order, func._name);
                    if (h == hash)
                    {
                        std::string source = qt_helpers::toUtf8(ui->vpShaderCode->toPlainText());
                        vp->setFunction(func._name, source, loc, order);
                        break;
                    }
                }
            }
        }
    }
    loadInfoLog();
}

void ShaderEditorDialog::reset()
{
    osg::StateSet * stateSet = getStateSet(true);

    if(stateSet)
    {
        stateSet->removeAttribute(osg::StateAttribute::VERTEXPROGRAM);
        stateSet->removeAttribute(osg::StateAttribute::FRAGMENTPROGRAM);
        stateSet->removeAttribute(osg::StateAttribute::PROGRAM);

        osgEarth::VirtualProgram * vp = osgEarth::VirtualProgram::getOrCreate(stateSet);
        vp->setInheritShaders(true);
        vp->setFunction("my_vertex", default_vertex_shader, osgEarth::ShaderComp::LOCATION_VERTEX_MODEL);
        vp->setFunction("my_fragment", default_fragment_shader, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING, 2.0f);
    }

    load();
}

void ShaderEditorDialog::load()
{
    bool foundShader = false;
    _ready = false;
    VirtualProgramAccessor* vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
    if(vp)
    {
        ui->vpInheritShaders->blockSignals(true);
        ui->vpInheritShaders->setChecked(vp->getInheritShaders());
        ui->vpInheritShaders->blockSignals(false);
        ui->vpLog->blockSignals(true);
        ui->vpLog->setChecked(vp->getShaderLogging());
        ui->vpLog->blockSignals(false);
        ui->vpLogFile->blockSignals(true);
        ui->vpLogFile->setText(qt_helpers::fromLocal8Bit(vp->getShaderLogFile()));
        ui->vpLogFile->blockSignals(false);

        ui->tabWidget->setCurrentWidget(ui->tabVirtualProgram);

        osgEarth::ShaderComp::FunctionLocationMap funcs;
        vp->getFunctions(funcs);

        ui->vpFunction->blockSignals(true);
        ui->vpFunction->clear();
        int index = -1;
        int currentIndex = -1;
        for (auto itfunc = funcs.begin(); itfunc != funcs.end(); ++itfunc)
        {
            const osgEarth::ShaderComp::FunctionLocation & loc = itfunc->first;
            const osgEarth::ShaderComp::OrderedFunctionMap & map = itfunc->second;
            for (auto itmap = map.begin(); itmap != map.end(); ++itmap)
            {
                const float & order = itmap->first;
                const osgEarth::ShaderComp::Function & func = itmap->second;
                unsigned h = hashFunctionName(loc, order, func._name);
                std::stringstream ss;
                ss << loc << ':' << order << ' ' << func._name;
                QString name = QString::fromStdString(ss.str());
                ui->vpFunction->insertItem(++index, name, h);
                switch (loc)
                {
                case osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING:
                case osgEarth::ShaderComp::LOCATION_FRAGMENT_LIGHTING:
                case osgEarth::ShaderComp::LOCATION_FRAGMENT_OUTPUT:
                    currentIndex = index;
                    break;
                default:
                    if(currentIndex < 0)
                        currentIndex = index;
                    break;
                }
            }
        }
        ui->vpFunction->blockSignals(false);
        ui->vpFunction->setCurrentIndex(currentIndex);
        foundShader = true;
    }
    else if(osg::Program * p = getProgram())
    {
        ui->tabWidget->setCurrentWidget(ui->tabProgram);

        unsigned contextID = osg_helpers::findContextID(p);

        ui->progShader->blockSignals(true);
        ui->progShader->clear();

        int index = -1;
        int currentIndex = -1;

        for(unsigned i = 0; i < p->getNumShaders(); ++i)
        {
            osg::Shader * shader = p->getShader(i);
            if(shader)
            {
                std::stringstream ss;
                ss << shader->getTypename() << ':' << shader->getName();
                QString name = QString::fromStdString(ss.str());
                ui->progShader->insertItem(++index, name, i);

                switch(shader->getType())
                {
                case osg::Shader::FRAGMENT:
                    currentIndex = index;
                    break;
                default:
                    if (currentIndex < 0)
                        currentIndex = index;
                    break;
                }
            }
        }
        ui->progShader->blockSignals(false);
        ui->progShader->setCurrentIndex(currentIndex);

        std::string log;
        p->getGlProgramInfoLog(contextID, log);
        _infoLogDock->setInfoLog(log);
        foundShader = true;
    }
    _uniformEditDock->reload();
    _ready = true;

    if(!foundShader)
    {
        ui->tabWidget->setCurrentWidget(ui->tabNoShader);
    }
}

void ShaderEditorDialog::reload()
{
    load();
}

void ShaderEditorDialog::loadInfoLog()
{
    VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
    if (vp)
    {
        unsigned contextID = osg_helpers::findContextID(vp);
        osgEarth::PolyShader * sh = getPolyShader(_currentVPFunctionIndex);
        std::string log;
        if (sh)
        {
            osg::Shader * shader = sh->getNominalShader();
            if(shader)
            {
                static_cast<ShaderAccess*>(shader)->getGlProgramInfoLog(contextID, log);
            }
            std::string src = sh->getShaderSource();
            ui->vpShaderCode->setPlainText(qt_helpers::fromUtf8(src));
        }
        _infoLogDock->setInfoLog(log);

    }
    else if(osg::Program * p = getProgram())
    {
        unsigned contextID = osg_helpers::findContextID(p);
        std::string log;
        p->getGlProgramInfoLog(contextID, log);
        _infoLogDock->setInfoLog(log);
    }
}

void ShaderEditorDialog::dirty()
{
    VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
    if (vp)
    {
        VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
        unsigned contextID = osg_helpers::findContextID(vp);
        vp->dirty(contextID);
    }
}

void ShaderEditorDialog::vpInheritShaders(bool b)
{
    osgEarth::VirtualProgram * vp = getVirtualProgram(false);
    if(vp)
        vp->setInheritShaders(b);
}

void ShaderEditorDialog::vpLogChanged(bool b)
{
    VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
    if (vp)
        vp->setShaderLogging(b);
}

void ShaderEditorDialog::vpLogFile(const QString & s)
{
    VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
    if (vp)
        vp->setShaderLogging(vp->getShaderLogging(), qt_helpers::toLocal8Bit(s));
}

void ShaderEditorDialog::vpFunctionChanged(int index)
{
    if (_ready)
    {
        // save current changes first
        apply();
    }

    VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
    unsigned contextID = osg_helpers::findContextID(vp);
    osgEarth::PolyShader * sh = getPolyShader(index);
    std::string log;
    if (sh)
    {
        osg::Shader * shader = sh->getNominalShader();
        if(shader)
        {
            static_cast<ShaderAccess*>(shader)->getGlProgramInfoLog(contextID, log);
        }
        std::string src = sh->getShaderSource();
        ui->vpShaderCode->setPlainText(qt_helpers::fromUtf8(src));
    }
    _infoLogDock->setInfoLog(log);
    _currentVPFunctionIndex = index;
}

void ShaderEditorDialog::vpFunctionAdd(QAction * action)
{
    VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
    if (vp)
    {
        osgEarth::ShaderComp::FunctionLocation location = (osgEarth::ShaderComp::FunctionLocation)action->data().toInt();
        std::string title;
        {
            std::stringstream ss;
            ss << "Add new " << location << " function";
            title = ss.str();
        }
        std::string functionName;
        if (_hostInterface->inputDialogString(this, functionName, "Function name:", title, SGIPluginHostInterface::InputDialogStringEncodingSystem))
        {
            std::stringstream ss;
            switch (location)
            {
            case osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING:
            case osgEarth::ShaderComp::LOCATION_FRAGMENT_LIGHTING:
            case osgEarth::ShaderComp::LOCATION_FRAGMENT_OUTPUT:
                ss << "#version " GLSL_VERSION_STR "\n"
                    << "void " << functionName << "(inout vec4 color)\n"
                    << "{\n"
                    << "}\n";
                break;
            case osgEarth::ShaderComp::LOCATION_VERTEX_MODEL:
            case osgEarth::ShaderComp::LOCATION_VERTEX_VIEW:
            case osgEarth::ShaderComp::LOCATION_VERTEX_CLIP:
                ss << "#version " GLSL_VERSION_STR "\n"
                    << "void " << functionName << "(inout vec4 VertexView)\n"
                    << "{\n"
                    << "}\n";
                break;
            default:
                ss << "#version " GLSL_VERSION_STR "\n"
                    << "void " << functionName << "()\n"
                    << "{\n"
                    << "}\n";
                break;
            }
            vp->setFunction(functionName, ss.str(), location);
            load();
        }
    }
}

void ShaderEditorDialog::vpFunctionRemove()
{
    int index = ui->vpFunction->currentIndex();
    removeVPShader(index);
    ui->vpFunction->removeItem(index);
    _currentVPFunctionIndex = -1;

}

void ShaderEditorDialog::vpFunctionDetails()
{
    bool found = false;
    int index = ui->vpFunction->currentIndex();
    if (index < 0)
        return;
    uint hash = ui->vpFunction->itemData(index).toUInt();
    osgEarth::VirtualProgram * vp_ = getVirtualProgram(false);
    if (vp_)
    {
        ShaderFunctionDetails * details = new ShaderFunctionDetails(vp_, hash, this);
        details->exec();
        if (details->hasModifications())
        {
            // reload the shader because the function has changed
            load();
        }
        delete details;
    }
}

void ShaderEditorDialog::progShaderChanged(int index)
{
    if (_ready)
    {
        // save current changes first
        apply();
    }

    osg::Program * p = getProgram(false);
    unsigned contextID = osg_helpers::findContextID(p);
    osg::Shader * shader = p->getShader(index);
    std::string log;
    if (shader)
    {
        if (shader)
        {
            static_cast<ShaderAccess*>(shader)->getGlProgramInfoLog(contextID, log);
        }
        std::string src = shader->getShaderSource();
        ui->progShaderCode->setPlainText(qt_helpers::fromUtf8(src));
    }
    _infoLogDock->setInfoLog(log);
    _currentProgShaderIndex = index;
}

void ShaderEditorDialog::progShaderAdd()
{
}

void ShaderEditorDialog::progShaderRemove()
{
}

void ShaderEditorDialog::openItem()
{
    if (_item.valid())
    {
        ISceneGraphDialogPtr dialog;
        if (_hostCallback.valid())
            dialog = _hostCallback->showSceneGraphDialog(this, _item.get());
        else
            dialog = _hostInterface->showSceneGraphDialog(this, _item.get(), _hostCallback.get());
        if (dialog.valid())
        {
            dialog->show();
        }
    }
}

} // namespace sgi
