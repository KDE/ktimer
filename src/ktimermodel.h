#ifndef KTIMERMODEL_H
#define KTIMERMODEL_H

#include <QAbstractListModel>
#include <QList>

class KTimerInfo;

class KTimerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    KTimerModel(QObject *parent = 0);
    QVariant data(const QModelIndex& idx, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void load();
private:
    QList<KTimerInfo*> m_timerInfos;

};

#endif
