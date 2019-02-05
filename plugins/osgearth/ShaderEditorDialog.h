#pragma once

#include <QDialog>
#include <sgi/plugins/SGISettingsDialogImpl>

QT_BEGIN_NAMESPACE
class Ui_ShaderEditorDialog;
QT_END_NAMESPACE
namespace osg {
    class StateSet;
    class Program;
}
namespace osgEarth {
    class VirtualProgram;
    class PolyShader;
}

namespace sgi {

class ShaderEditorDialog : public SettingsQDialogImpl
{
    Q_OBJECT

public:
    ShaderEditorDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info);
    ~ShaderEditorDialog() override;

public slots:

protected slots:
    void                    reset();
    void                    apply();
    void                    load();
    void                    reload();

    void                    vpFunctionChanged(int index);
    void                    vpFunctionAdd(QAction * action);
    void                    vpFunctionRemove();
    void                    vpFunctionDetails();
    void                    vpInheritShaders(bool b);
    void                    vpLogChanged(bool b);
    void                    vpLogFile(const QString & s);

    void                    progShaderChanged(int index);
    void                    progShaderAdd();
    void                    progShaderRemove();

    void                    openItem();

protected:
    osg::StateSet *         getStateSet(bool create = false);
    osgEarth::VirtualProgram * getVirtualProgram(bool create = false);
    osg::Program *          getProgram(bool create = false);
    osgEarth::PolyShader *  getPolyShader(int index);
    bool                    removeVPShader(int index);
    void                    loadInfoLog();
    void                    setInfoLog(const std::string & log);

private:  // for now
    IHostCallbackPtr                _hostCallback;
    Ui_ShaderEditorDialog* ui;
    bool _ready;
    int _currentVPFunctionIndex;
    int _currentProgShaderIndex;
};

} // namespace sgi
