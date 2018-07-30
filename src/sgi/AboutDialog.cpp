#include "stdafx.h"
#include "AboutDialog.h"

#include <ui_AboutDialog.h>
#include <sgi/SGIItemInternal>

#include <QPushButton>
#include <QTextStream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {
namespace internal_plugin {


AboutDialog::AboutDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info)
    : SettingsQDialogImpl(parent, hostInterface, item, info)
{
	ui = new Ui_AboutDialog;
	ui->setupUi( this );

	connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &AboutDialog::reject);

    QString str;
    QTextStream ts(&str);
    ts << sgiGetVersion() << " (" << sgiGetSOVersion() << ")";

    ui->version->setText(str);

    std::string name;
    _hostInterface->getObjectDisplayName(name, _item.get());
    setWindowTitle(QString::fromStdString(name));
}

AboutDialog::~AboutDialog()
{
	if (ui)
	{
		delete ui;
		ui = NULL;
	}
}

} // namespace internal_plugin
} // namespace sgi
