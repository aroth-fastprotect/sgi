#include "stdafx.h"
#include "ShaderEditorDialog.h"

#include <sgi/plugins/SGISettingsDialogImpl>
#include <sgi/plugins/SGIItemOsg>

#include <QPushButton>

#include <ui_ShaderEditorDialog.h>

#include <osgEarth/NodeUtils>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

ShaderEditorDialog::ShaderEditorDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info)
    : SettingsQDialogImpl(parent, hostInterface, item, info)
    , ui(new Ui_ShaderEditorDialog)
    , _ready(false)
 {
    ui->setupUi( this );

    connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &ShaderEditorDialog::close);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &ShaderEditorDialog::apply);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &ShaderEditorDialog::reset);
    _ready = true;
}

ShaderEditorDialog::~ShaderEditorDialog()
{
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
}

void ShaderEditorDialog::apply()
{
    if (!_ready)
        return;
}

void ShaderEditorDialog::reset()
{
}

} // namespace sgi
