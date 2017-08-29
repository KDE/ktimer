#include "ktimermodel.h"
#include "ktimerjob.h"

#include <QDebug>
#include <KSharedConfig>
#include <KConfigGroup>


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

void KTimerModel::load()
{
    auto cfg = KSharedConfig::openConfig().data();
    const int numJobs = cfg->group("Jobs").readEntry( "Number", 0 );
    beginResetModel();
    for (int n = 0; n < numJobs; ++n) {
        auto *job = new KTimerJob();
        job->load(cfg, QStringLiteral("Job%1").arg(n));
    }
    endResetModel();
}
