
#include "log4cplus_model.h"

#include <QDebug>
#include <QMimeType>
#include <log4cplus/hierarchy.h>

using namespace sgi;

Log4cplusModel::Log4cplusModel(QObject *parent)
    : QStandardItemModel(parent)
    , m_modelFilled(false)
{
}

Log4cplusModel::~Log4cplusModel()
{
}

QVariant Log4cplusModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && role == Qt::DecorationRole && index.column() == 3) {
        QStandardItem *item = itemFromIndex(index);
        if (!item)
            return QVariant();

        // on-demand lookup of icons, too slow to do that all in one go
        const QVariant v = item->data(Qt::DecorationRole);
        if (v.isNull()) {
            const QString iconName = item->data(IconNameRole).toString();
            const QString genericIconName = item->data(GenericIconNameRole).toString();
            if (iconName.isEmpty() && genericIconName.isEmpty())
                return QVariant();

            QIcon icon = QIcon::fromTheme(item->data(IconNameRole).toString());
            if (icon.isNull())
                icon = QIcon::fromTheme(item->data(GenericIconNameRole).toString());
            const_cast<Log4cplusModel *>(this)->blockSignals(true);
            item->setIcon(icon);
            item->setData(QVariant(), IconNameRole); // reset to avoid trying to look up an icon multiple times
            item->setData(QVariant(), GenericIconNameRole);
            const_cast<Log4cplusModel *>(this)->blockSignals(false);
            return icon;
        } else {
            return v;
        }
    }
    return QStandardItemModel::data(index, role);
}

Qt::ItemFlags Log4cplusModel::flags(const QModelIndex &index) const
{
    return QStandardItemModel::flags(index) & ~Qt::ItemIsEditable;
}

int Log4cplusModel::rowCount(const QModelIndex &parent) const
{
    const_cast<Log4cplusModel *>(this)->fillModel();
    return QStandardItemModel::rowCount(parent);
}

QVector<QStandardItem *> Log4cplusModel::itemsForLogger(const log4cplus::Logger &logger)
{
    auto it = m_loggerItems.find(QString::fromStdString(logger.getName()));
    if(it != m_loggerItems.end())
        return it.value();

    makeItemsForLogger(logger);
    return m_loggerItems.value(QString::fromStdString(logger.getName()));
}

void Log4cplusModel::makeItemsForLogger(const log4cplus::Logger &logger)
{
    QString loggerName = QString::fromStdString(logger.getName());

    if(logger.getParent().getName() == log4cplus::Logger::getRoot().getName())
    {
        const QList<QStandardItem *> row = makeRowForLogger(logger);
        appendRow(row);
        m_loggerItems[loggerName].push_back(row.first());
    } else {
        foreach (QStandardItem *parentItem, itemsForLogger(logger.getParent()))
        {
            const QList<QStandardItem *> row = makeRowForLogger(logger);
            parentItem->appendRow(row);
            m_loggerItems[loggerName].push_back(row.first());
        }
    }
}

QList<QStandardItem *> Log4cplusModel::makeRowForLogger(const log4cplus::Logger &logger)
{
    QList<QStandardItem *> row;
    auto *item = new QStandardItem;
    item->setText(QString::fromStdString(logger.getName()));
    row.push_back(item);

    item = new QStandardItem;
    item->setText(QString::number(logger.getLogLevel()));
    row.push_back(item);

    return row;
}

void Log4cplusModel::fillModel()
{
    if (m_modelFilled)
        return;
    m_modelFilled = true;

    blockSignals(true);
    setHorizontalHeaderLabels(QStringList() << tr("Name")
                                            << tr("LogLevel")
                                            );

    log4cplus::LoggerList allLoggers = log4cplus::Logger::getCurrentLoggers();
    for(log4cplus::LoggerList::const_iterator it = allLoggers.begin(); it != allLoggers.end(); it++)
    {
        const log4cplus::Logger & child = *it;
        if(!m_loggerItems.contains(QString::fromStdString(child.getName())))
            makeItemsForLogger(child);
    }

    m_loggerItems.clear();
    blockSignals(false);
}
