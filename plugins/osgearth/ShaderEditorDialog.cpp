#include "stdafx.h"
#include "ShaderEditorDialog.h"

#include <sgi/plugins/SGISettingsDialogImpl>
#include <sgi/plugins/SGIItemOsg>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/helpers/qt>
#include <sgi/helpers/osg>

#include <QPushButton>

#include <ui_ShaderEditorDialog.h>

#include <osgEarth/VirtualProgram>
#include <osg/Node>
#include "osgearth_accessor.h"

#include "ShaderFunctionDetails.h"

#include <QDebug>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

namespace osgearth_plugin {
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ShaderComp::FunctionLocation & t);
} // namespace osgearth_plugin

using namespace osgearth_plugin;

InfoLogDock::InfoLogDock(ShaderEditorDialog * parent)
    : QDockWidget (parent)
    , _log(nullptr)
{
    setWindowTitle(tr("Info log"));
    _log = new QTextEdit(this);
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setWidget(_log);
}

InfoLogDock::~InfoLogDock()
{
}

void InfoLogDock::setInfoLog(const std::string & log)
{
    if (!log.empty())
    {
        setWindowIcon(QIcon::fromTheme("dialog-warning"));
        _log->setPlainText(QString::fromStdString(log));
    }
    else
    {
        setWindowIcon(QIcon::fromTheme("dialog-ok"));
        _log->setHtml(QStringLiteral("<i>empty</i>"));
    }
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
    , _ready(false)
    , _currentVPFunctionIndex(-1)
    , _currentProgShaderIndex(-1)
 {
    ui->setupUi( this );

    for(int i = 0; i < ui->tabWidget->count(); ++i)
        ui->tabWidget->widget(i)->setWindowTitle(ui->tabWidget->tabText(i));

    _infoLogDock = new InfoLogDock(this);
    addDockWidget(Qt::RightDockWidgetArea, _infoLogDock);

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

    load();
    _ready = true;
}

ShaderEditorDialog::~ShaderEditorDialog()
{
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

class PolyShaderAccess : public osgEarth::PolyShader
{
public:
    void resetShaders()
    {
        _nominalShader = nullptr;
        _geomShader = nullptr;
        _tessevalShader = nullptr;
        _dirty = true;
    }
};

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

namespace {
    void showHideTab(QTabWidget * tab, QWidget * w, bool visible)
    {
        if(visible)
        {
            int index = tab->indexOf(w);
            if (index < 0)
                tab->addTab(w, w->windowTitle());
        }
        else {
            tab->removeTab(tab->indexOf(w));
        }
    }
    void setTabIcon(QTabWidget * tab, QWidget * w, const QIcon & ico)
    {
        int index = tab->indexOf(w);
        if (index >= 0)
            tab->setTabIcon(index, ico);
    }
}

void ShaderEditorDialog::load()
{
    bool foundShader = false;
    _ready = false;
    VirtualProgramAccessor* vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
    if(vp)
    {
        ui->vpInheritShaders->setChecked(vp->getInheritShaders());
        ui->vpLog->setChecked(vp->getShaderLogging());
        ui->vpLogFile->setText(qt_helpers::fromLocal8Bit(vp->getShaderLogFile()));

        showHideTab(ui->tabWidget,ui->tabVirtualProgram, true);
        showHideTab(ui->tabWidget,ui->tabProgram, false);
        showHideTab(ui->tabWidget,ui->tabNoShader, false);
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
                qDebug() << loc << order << QString::fromStdString(func._name) << h;
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
        showHideTab(ui->tabWidget,ui->tabVirtualProgram, false);
        showHideTab(ui->tabWidget,ui->tabProgram, true);
        showHideTab(ui->tabWidget,ui->tabNoShader, false);
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
    _ready = true;

    if(!foundShader)
    {
        showHideTab(ui->tabWidget,ui->tabVirtualProgram, false);
        showHideTab(ui->tabWidget,ui->tabProgram, false);
        showHideTab(ui->tabWidget,ui->tabNoShader, true);
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
        VirtualProgramAccessor * vp = static_cast<VirtualProgramAccessor*>(getVirtualProgram(false));
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
    qDebug() << __FUNCTION__ << hash;
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
