#include "stdafx.h"
#include "ProxySettingsDialog.h"

#include "ui_ProxySettingsDialog.h"

#include <osgEarth/Registry>
#include <osgEarth/HTTPClient>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace terra3d {

ProxySettingsDialog::ProxySettingsDialog(Terra3DView * view)
	: QDialog(view)
	, m_view(view)
	, m_ready(false)
{
	ui = new Ui_ProxySettingsDialog;
	ui->setupUi( this );

	connect(ui->buttonBox->button(QDialogButtonBox::Save), SIGNAL(clicked()), this, SLOT(save()));
	connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
	connect(ui->buttonBox->button(QDialogButtonBox::Close), SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), SIGNAL(clicked()), this, SLOT(restoreDefaults()));

	load();
	m_ready = true;
}

ProxySettingsDialog::~ProxySettingsDialog()
{
	if (ui)
	{
		delete ui;
		ui = NULL;
	}
}

void ProxySettingsDialog::showEvent( QShowEvent * event )
{
	load();
}

void ProxySettingsDialog::load()
{
    const osgEarth::optional<osgEarth::ProxySettings> & proxy = osgEarth::HTTPClient::getProxySettings();
    ui->enabled->setChecked(proxy.isSet() && !proxy->hostName().empty());
    ui->serverName->setText(QString::fromStdString(proxy->hostName()));
    ui->port->setValue(proxy->port());
    ui->username->setText(QString::fromStdString(proxy->userName()));
    ui->password->setText(QString::fromStdString(proxy->password()));
}

void ProxySettingsDialog::restoreDefaults()
{
	load();
}

// buttons
void ProxySettingsDialog::save()
{
	apply(true);
	accept();
}

void ProxySettingsDialog::apply()
{
	apply(false);
}

void ProxySettingsDialog::apply(bool save)
{
	if(!m_ready)
		return;

    osgEarth::ProxySettings proxy;
    if(ui->enabled->isChecked())
    {
        proxy.hostName() = ui->serverName->text().toStdString();
        proxy.port() = ui->port->value();
        proxy.userName() = ui->username->text().toStdString();
        proxy.password() = ui->password->text().toStdString();
    }
    osgEarth::HTTPClient::setProxySettings(proxy);

	m_view->repaint();
}

void ProxySettingsDialog::proxyEnabled(bool enable)
{
    ui->serverName->setEnabled(enable);
    ui->port->setEnabled(enable);
    ui->username->setEnabled(enable);
    ui->password->setEnabled(enable);
}

void ProxySettingsDialog::reject()
{
	QDialog::reject();
}

} // namespace terra3d
