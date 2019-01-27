#include "stdafx.h"
#include "ShaderEditorDialog.h"

#include <sgi/plugins/SGISettingsDialogImpl>
#include <sgi/plugins/SGIItemOsg>

#include <QPushButton>

#include <ui_ShaderEditorDialog.h>

#include <osgEarth/VirtualProgram>
#include <osg/Node>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

namespace osgearth_plugin {
extern std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const osgEarth::ShaderComp::FunctionLocation & t);
} // namespace osgearth_plugin

using namespace osgearth_plugin;

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

ShaderEditorDialog::ShaderEditorDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info)
    : SettingsQDialogImpl(parent, hostInterface, item, info)
    , ui(new Ui_ShaderEditorDialog)
    , _ready(false)
 {
    ui->setupUi( this );

    connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &ShaderEditorDialog::close);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ShaderEditorDialog::apply);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &ShaderEditorDialog::reset);
    ui->buttonBox->button(QDialogButtonBox::Reset)->setText(tr("Create Default"));
    _ready = true;
}

ShaderEditorDialog::~ShaderEditorDialog()
{
    delete ui;
    ui = nullptr;
}

void ShaderEditorDialog::apply()
{
    if (!_ready)
        return;

    osg::StateSet * stateSet = getObject<osg::StateSet, SGIItemOsg>();
    osg::Node * node = getObject<osg::Node, SGIItemOsg>();
    if(!stateSet && node)
        stateSet = node->getOrCreateStateSet();

}

void ShaderEditorDialog::reset()
{

    osg::StateSet * stateSet = getObject<osg::StateSet, SGIItemOsg>();
    osg::Node * node = getObject<osg::Node, SGIItemOsg>();
    if(!stateSet && node)
        stateSet = node->getOrCreateStateSet();

    if(stateSet)
    {
        stateSet->removeAttribute(osg::StateAttribute::VERTEXPROGRAM);
        stateSet->removeAttribute(osg::StateAttribute::FRAGMENTPROGRAM);
        stateSet->removeAttribute(osg::StateAttribute::PROGRAM);

        osgEarth::VirtualProgram * vp = osgEarth::VirtualProgram::getOrCreate(stateSet);
        vp->setInheritShaders(true);
        vp->setFunction("my_vertex", default_vertex_shader, osgEarth::ShaderComp::LOCATION_VERTEX_MODEL);
        vp->setFunction("my_frament", default_fragment_shader, osgEarth::ShaderComp::LOCATION_FRAGMENT_COLORING);
    }
}

void ShaderEditorDialog::load()
{
    osg::StateSet * stateSet = getObject<osg::StateSet, SGIItemOsg>();
    osg::Node * node = getObject<osg::Node, SGIItemOsg>();

    if(stateSet)
    {
        osgEarth::VirtualProgram * vp = osgEarth::VirtualProgram::get(stateSet);
        if(vp)
        {
            ui->vpInheritShaders->setChecked(vp->getInheritShaders());

            osgEarth::ShaderComp::FunctionLocationMap funcs;
            vp->getFunctions(funcs);

            for(auto it = funcs.begin(); it != funcs.end(); ++it)
            {
                const osgEarth::ShaderComp::FunctionLocation & loc = it->first;
                const osgEarth::ShaderComp::OrderedFunctionMap & map = it->second;
                for(auto it = map.begin(); it != map.end(); ++it)
                {
                    const float & order = it->first;
                    const osgEarth::ShaderComp::Function & func = it->second;
                    std::stringstream ss;
                    ss << loc << ':' << order << ' ' << func._name;
                    QString name = QString::fromStdString(ss.str());
                    ui->vpFunction->addItem(name, qHash(name));
                }
            }
        }
        else
        {
            osg::Program * p = dynamic_cast<osg::Program*>(stateSet->getAttribute(osg::StateAttribute::PROGRAM));
            if(p)
            {
                for(unsigned i = 0; i < p->getNumShaders(); ++i)
                {
                    osg::Shader * shader = p->getShader(i);
                    if(shader)
                    {
                        switch(shader->getType())
                        {
                        case osg::Shader::FRAGMENT:
                            ui->shaderFragment->setText(QString::fromStdString(shader->getShaderSource()));
                            break;
                        case osg::Shader::VERTEX:
                            ui->shaderVertex->setText(QString::fromStdString(shader->getShaderSource()));
                            break;
                        }
                    }
                }
            }
        }
    }
}

void ShaderEditorDialog::vpInheritShaders(bool b)
{
    osg::StateSet * stateSet = getObject<osg::StateSet, SGIItemOsg>();
    if(stateSet)
    {
        osgEarth::VirtualProgram * vp = osgEarth::VirtualProgram::get(stateSet);
        if(vp)
            vp->setInheritShaders(b);
    }
}

void ShaderEditorDialog::vpFunctionChanged(int index)
{
    uint hash = ui->vpFunction->itemData(index).toUInt();

    osg::StateSet * stateSet = getObject<osg::StateSet, SGIItemOsg>();
    if(stateSet)
    {
        osgEarth::VirtualProgram * vp = osgEarth::VirtualProgram::get(stateSet);
        if(vp)
        {
            osgEarth::ShaderComp::FunctionLocationMap funcs;
            vp->getFunctions(funcs);

            for(auto it = funcs.begin(); it != funcs.end(); ++it)
            {
                const osgEarth::ShaderComp::FunctionLocation & loc = it->first;
                const osgEarth::ShaderComp::OrderedFunctionMap & map = it->second;
                for(auto it = map.begin(); it != map.end(); ++it)
                {
                    const float & order = it->first;
                    const osgEarth::ShaderComp::Function & func = it->second;
                    std::stringstream ss;
                    ss << loc << ':' << order << ' ' << func._name;
                    QString name = QString::fromStdString(ss.str());
                    if(qHash(name) == hash)
                    {

                    }
                }
            }
        }
    }
}

void ShaderEditorDialog::vpFunctionAdd()
{
}

void ShaderEditorDialog::vpFunctionRemove()
{
}

} // namespace sgi
