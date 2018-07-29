#pragma once

#include <QDialog>
#include <sgi/plugins/SGISettingsDialogImpl>

QT_BEGIN_NAMESPACE
class Ui_ShaderEditorDialog;
QT_END_NAMESPACE

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

    void                    vpFunctionChanged(int index);
    void                    vpFunctionAdd();
    void                    vpFunctionRemove();
    void                    vpInheritShaders(bool b);

private:  // for now
    Ui_ShaderEditorDialog* ui;
    bool _ready;
};

} // namespace sgi
