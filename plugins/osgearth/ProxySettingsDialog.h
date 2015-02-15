#ifndef PROXYSETTINGSDIALOG_H
#define PROXYSETTINGSDIALOG_H

QT_BEGIN_NAMESPACE
class Ui_ProxySettingsDialog;
QT_END_NAMESPACE

namespace terra3d {

class Terra3DView;

class ProxySettingsDialog : public QDialog
{
	Q_OBJECT

public:
						ProxySettingsDialog(Terra3DView * view);
	virtual				~ProxySettingsDialog();

public slots:
	void				save();
	void				apply();
	virtual void		reject();
	void				load();
	void				restoreDefaults();
    void                proxyEnabled(bool enable);

protected:
	void				apply(bool save);

	virtual void 		showEvent  ( QShowEvent * event );

private:
	Ui_ProxySettingsDialog *	ui;
	Terra3DView *		m_view;
	bool				m_ready;
};

} // namespace terra3d

#endif // PROXYSETTINGSDIALOG_H
