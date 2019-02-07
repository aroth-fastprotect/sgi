#pragma once

#include <QDialog>
#include <QDockWidget>
#include <QAbstractItemModel>
#include <sgi/plugins/SGISettingsWindowImpl>

QT_BEGIN_NAMESPACE
class Ui_ShaderEditorDialog;
class QTextEdit;
class QTableView;
class QTreeView;
class QTemporaryFile;
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

class ShaderEditorDialog;

class InfoLogDock : public QDockWidget
{
    Q_OBJECT

public:
    InfoLogDock(ShaderEditorDialog * parent=nullptr);
    ~InfoLogDock() override;

    void setInfoLog(const std::string & log);

protected:
    void showEvent(QShowEvent * event) override;
    void hideEvent(QHideEvent * event) override;

    void onTimer();
    void update();

private:
    ShaderEditorDialog * _shaderEditor;
    QTextEdit * _log;
    QTimer * _timer;
    std::string _infoLog;
    QString _logFilename;
    QByteArray _logFileData;
};


class UniformModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    UniformModel(ShaderEditorDialog * parent=nullptr);
    ~UniformModel() override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    enum Section {
        SectionName = 0,
        SectionValue,
        SectionType,
        SectionCount = SectionType
    };

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */) override;
    void reload();

private:
    class Private;
    Private * m_impl;
};

class UniformEditDock : public QDockWidget
{
    Q_OBJECT

public:
    UniformEditDock(ShaderEditorDialog * parent = nullptr);
    ~UniformEditDock() override;

    void reload();

private:
    QTableView * _table;
    UniformModel * _model;
};

class ShaderEditorDialog : public SettingsQMainWindowImpl
{
    friend class InfoLogDock;
    friend class UniformModel;
    Q_OBJECT

public:
    ShaderEditorDialog(QWidget * parent, SGIPluginHostInterface * hostInterface, SGIItemBase * item, ISettingsDialogInfo * info);
    ~ShaderEditorDialog() override;

    const QString &         shaderLogFile() const;

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

    void                    dirty();

private:  // for now
    IHostCallbackPtr                _hostCallback;
    Ui_ShaderEditorDialog* ui;
    InfoLogDock * _infoLogDock;
    UniformEditDock * _uniformEditDock;
    bool _ready;
    int _currentVPFunctionIndex;
    int _currentProgShaderIndex;
    std::string _originalLogFile;
    bool _originalLogFileEnabled;
    QTemporaryFile * _tmpShaderLog;
    QString _shaderLogFile;
};

} // namespace sgi
