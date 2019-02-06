#include "stdafx.h"
#include "ShaderFunctionDetails.h"

#include <ui_ShaderFunctionDetails.h>

#include <QPushButton>

#include <osgEarth/VirtualProgram>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

extern unsigned hashFunctionName(const osgEarth::ShaderComp::FunctionLocation & loc, float order, const std::string & name);

class ShaderFunctionDetailsPrivate
{
public:
    ShaderFunctionDetailsPrivate(osgEarth::VirtualProgram * vp, unsigned functionHash)
        : _ready(false)
        , _vp(vp)
        , _functionHash(functionHash)
        , _originalOrder(0)
        , _originalLocation(osgEarth::ShaderComp::LOCATION_UNDEFINED)
        , _currentOrder(0)
        , _currentLocation(osgEarth::ShaderComp::LOCATION_UNDEFINED)
    {
    }
    bool _ready;
    osg::ref_ptr<osgEarth::VirtualProgram> _vp;
    unsigned _functionHash;
    float _originalOrder;
    osgEarth::ShaderComp::FunctionLocation _originalLocation;
    float _currentOrder;
    osgEarth::ShaderComp::FunctionLocation _currentLocation;
};


#define AFM_ACTION(__def) \
    ui->location->addItem(#__def, QVariant::fromValue((int)osgEarth::ShaderComp::__def));

ShaderFunctionDetails::ShaderFunctionDetails(osgEarth::VirtualProgram * vp, unsigned functionHash, QWidget * parent)
    : QDialog(parent)
    , ui(new Ui_ShaderFunctionDetails)
    , d_ptr(new ShaderFunctionDetailsPrivate(vp, functionHash))
{
    ui->setupUi( this );

    ui->location->blockSignals(true);
    AFM_ACTION(LOCATION_VERTEX_MODEL);
    AFM_ACTION(LOCATION_VERTEX_VIEW);
    AFM_ACTION(LOCATION_VERTEX_CLIP);
    AFM_ACTION(LOCATION_TESS_CONTROL);
    AFM_ACTION(LOCATION_TESS_EVALUATION);
    AFM_ACTION(LOCATION_GEOMETRY);
    AFM_ACTION(LOCATION_FRAGMENT_COLORING);
    AFM_ACTION(LOCATION_FRAGMENT_LIGHTING);
    AFM_ACTION(LOCATION_FRAGMENT_OUTPUT);
    ui->location->blockSignals(false);

    connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &ShaderFunctionDetails::close);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &ShaderFunctionDetails::reset);
    load();
    d_ptr->_ready = true;
}

ShaderFunctionDetails::~ShaderFunctionDetails()
{
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
    delete d_ptr;
}

bool ShaderFunctionDetails::hasModifications() const
{
    Q_D(const ShaderFunctionDetails);
    return d->_originalLocation != d->_currentLocation || d->_originalOrder != d->_currentOrder;
}

void ShaderFunctionDetails::load()
{
    Q_D(ShaderFunctionDetails);
    std::string functionName;
    osgEarth::ShaderComp::FunctionLocation location = osgEarth::ShaderComp::LOCATION_UNDEFINED;
    float functionOrder = 0;
    bool found = false;

    if (d->_vp.valid())
    {
        osgEarth::ShaderComp::FunctionLocationMap funcs;
        d->_vp->getFunctions(funcs);

        for (auto itfunc = funcs.begin(); itfunc != funcs.end(); ++itfunc)
        {
            const osgEarth::ShaderComp::FunctionLocation & loc = itfunc->first;
            const osgEarth::ShaderComp::OrderedFunctionMap & map = itfunc->second;
            for (auto itmap = map.begin(); itmap != map.end(); ++itmap)
            {
                const float & order = itmap->first;
                const osgEarth::ShaderComp::Function & func = itmap->second;
                unsigned h = hashFunctionName(loc, order, func._name);
                if (h == d->_functionHash)
                {
                    functionOrder = order;
                    functionName = func._name;
                    location = loc;

                    found = true;
                }
            }
        }
    }

    if (found)
    {
        d->_originalOrder = functionOrder;
        d->_originalLocation = location;
        d->_currentOrder = d->_originalOrder;
        d->_currentLocation = d->_originalLocation;
    }

    int index = -1;
    setWindowTitle(tr("Function %1").arg(QString::fromStdString(functionName)));
    ui->functionName->setText(QString::fromStdString(functionName));
    ui->orderSpinBox->blockSignals(true);
    ui->orderSpinBox->setValue(functionOrder);
    ui->orderSpinBox->blockSignals(false);
    ui->location->blockSignals(true);
    index = ui->location->findData(location);
    ui->location->setCurrentIndex(index);
    ui->location->blockSignals(false);

}

void ShaderFunctionDetails::reset()
{
    Q_D(ShaderFunctionDetails);
    applyChanges(d->_originalLocation, d->_originalOrder);
    d->_currentOrder = d->_originalOrder;
    d->_currentLocation = d->_originalLocation;
    load();
}

bool ShaderFunctionDetails::applyChanges(int location, float order)
{
    bool found = false;
    Q_D(ShaderFunctionDetails);
    if (d->_vp.valid())
    {
        osgEarth::ShaderComp::FunctionLocationMap funcs;
        d->_vp->getFunctions(funcs);

        std::string functionName;
        osgEarth::ShaderComp::FunctionLocation oldLocation = osgEarth::ShaderComp::LOCATION_UNDEFINED;
        std::string source;
        float oldOrder = 0;

        for (auto it = funcs.begin(); it != funcs.end(); ++it)
        {
            const osgEarth::ShaderComp::FunctionLocation & loc = it->first;
            const osgEarth::ShaderComp::OrderedFunctionMap & map = it->second;
            for (auto it = map.begin(); it != map.end(); ++it)
            {
                const float & order = it->first;
                const osgEarth::ShaderComp::Function & func = it->second;
                unsigned h = hashFunctionName(loc, order, func._name);
                if (h == d->_functionHash)
                {
                    oldOrder = order;
                    functionName = func._name;
                    oldLocation = loc;
                    osgEarth::PolyShader * sh = d->_vp->getPolyShader(functionName);
                    if (sh)
                        source = sh->getShaderSource();
                    found = true;
                }
            }
        }
        if (found)
        {
            d->_vp->removeShader(functionName);
            d->_vp->setFunction(functionName, source, (osgEarth::ShaderComp::FunctionLocation)location, order);
        }
    }
    return found;
}

void ShaderFunctionDetails::orderChanged(double v)
{
    Q_D(ShaderFunctionDetails);
    applyChanges(d->_currentLocation, (float)v);
    d->_currentOrder = v;
}

void ShaderFunctionDetails::locationChanged(int index)
{
    Q_D(ShaderFunctionDetails);
    osgEarth::ShaderComp::FunctionLocation location = (osgEarth::ShaderComp::FunctionLocation)ui->location->itemData(index).toInt();
    applyChanges(location, d->_currentOrder);
    d->_currentLocation = location;
}



} // namespace sgi
