#pragma once
#include "common/modelroles.h"

#include <QMimeDatabase>
#include <QStandardItemModel>

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
    QVector<QStandardItem *> itemsForType(const QString &mimeTypeName);
    void makeItemsForType(const QString &mimeTypeName);
    static QList<QStandardItem *> makeRowForType(const QMimeType &mt);
    QSet<QString> normalizedMimeTypeNames(const QStringList &typeNames) const;

    QHash<QString, QVector<QStandardItem *> > m_mimeTypeNodes;
    QMimeDatabase m_db;
    bool m_modelFilled;
};
}
