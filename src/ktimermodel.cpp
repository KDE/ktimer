#include "ktimermodel.h"
#include <QDebug>

KTimerModel::KTimerModel(QObject* parent) : QAbstractListModel(parent)
{
}

QVariant KTimerModel::data(const QModelIndex& idx, int role) const
{
    if (! idx.isValid())
        return {};

    switch(role) {
        case Qt::DisplayRole : return "Something";
    }
    return {};
}

bool KTimerModel::insertRows(int row, int count, const QModelIndex& parent)
{
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);
    return false;
}

int KTimerModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_timerInfos.count();
}

QHash<int, QByteArray> KTimerModel::roleNames() const
{
    return {};
}
