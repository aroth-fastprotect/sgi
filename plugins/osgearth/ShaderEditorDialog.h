#pragma once

#include <QDialog>
#include <QDockWidget>
#include <QAbstractItemModel>
#include <sgi/plugins/SGISettingsWindowImpl>
#include <osg/ref_ptr>

QT_BEGIN_NAMESPACE
class Ui_ShaderEditorDialog;
class QTextEdit;
class QTableView;
class QTreeView;
class QTemporaryFile;
class QComboBox;
class QPushButton;
QT_END_NAMESPACE
namespace osg {
    class StateSet;
    class Program;
    class Uniform;
    class Texture;
    class Camera;
}
namespace osgEarth {
    class VirtualProgram;
    class PolyShader;
}

namespace sgi {

class IContextMenu;
typedef sgi::details::ref_ptr<IContextMenu> IContextMenuPtr;

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
    void onContextMenuRequested(const QPoint &pos);
    void onRefresh();

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

    osg::Uniform * getUniform(const QModelIndex &index) const;

private:
    class Private;
    Private * m_impl;
};

class UniformEditDock : public QDockWidget
{
    Q_OBJECT

public:
    UniformEditDock(ISettingsDialogInfo * info, ShaderEditorDialog * parent = nullptr);
    ~UniformEditDock() override;

    void reload();

    void onContextMenuRequested(const QPoint &pos);

private:
    QTableView * _table;
    UniformModel * _model;
    IContextMenuPtr _contextMenu;
    ISettingsDialogInfo * _info;
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
    void                    createEmptyShader();
    void                    debugTools(bool on);
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
    void                    setupShaderLogFile();
    void                    loadInfoLog();

    void                    dirty();
    void                    selectItemFromPath();
    void                    activateDebugTools(bool on);

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
    QComboBox * _comboBoxPath;
    QPushButton * _openItemButton;
    osg::ref_ptr<osg::Texture> _dbgVec4;
    osg::ref_ptr<osg::Camera> _dbgCamera;
};

} // namespace sgi
