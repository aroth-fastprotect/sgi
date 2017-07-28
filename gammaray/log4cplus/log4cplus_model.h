#pragma once
#include "common/modelroles.h"

#include <QMimeDatabase>
#include <QStandardItemModel>

namespace log4cplus {
    class Logger;
}

namespace sgi {
class Log4cplusModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum Roles {
        IconNameRole = Qt::UserRole + 1,
        GenericIconNameRole
    };

    explicit Log4cplusModel(QObject *parent = nullptr);
    ~Log4cplusModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    void fillModel();
    QVector<QStandardItem *> itemsForLogger(const log4cplus::Logger & logger);
    void makeItemsForLogger(const log4cplus::Logger & logger);
    static QList<QStandardItem *> makeRowForLogger(const log4cplus::Logger & logger);

    QHash<QString, QVector<QStandardItem *> > m_loggerItems;
    QMimeDatabase m_db;
    bool m_modelFilled;
};
}
