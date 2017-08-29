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

    auto job = m_timerInfos.at(idx.row());
    switch(role) {
        case JOB: return QVariant::fromValue<QObject*>(job);
    }
    return {};
}

bool KTimerModel::insertRows(int row, int count, const QModelIndex& parent)
{
    // We ignore everything because we are always inserting at the end.
    Q_UNUSED(row);
    Q_UNUSED(count);
    Q_UNUSED(parent);

    auto currTotal = rowCount(parent);
    beginInsertRows(parent, currTotal, currTotal);
    auto *job = new KTimerJob();
    m_timerInfos.append(job);
    endInsertRows();
    return false;
}

void KTimerModel::createJob()
{
    insertRows(1, 1, QModelIndex());
}

bool KTimerModel::removeRows(int first, int last, const QModelIndex& parent)
{
    if (last < first)
        return false;
    if (first >= m_timerInfos.count() || last >= m_timerInfos.count())
        return false;

    beginRemoveRows(parent, first, last);
        auto job = m_timerInfos.takeAt(first);
        job->deleteLater();
    endRemoveRows();
    return true;
}

void KTimerModel::removeJob(int row)
{
    removeRows(row, row, QModelIndex());
}

int KTimerModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_timerInfos.count();
}

QHash<int, QByteArray> KTimerModel::roleNames() const
{
    return {
        {JOB, "job"},
    };
}

void KTimerModel::loadJobs()
{
    auto cfg = KSharedConfig::openConfig().data();
    const int numJobs = cfg->group("Jobs").readEntry( "Number", 0 );
    beginResetModel();
    for (int n = 0; n < numJobs; ++n) {
        auto *job = new KTimerJob();
        job->load(cfg, QStringLiteral("Job%1").arg(n));
        m_timerInfos.append(job);
    }
    endResetModel();
}

void KTimerModel::saveJobs()
{
    auto cfg = KSharedConfig::openConfig().data();
    for(int i = 0, end = m_timerInfos.count(); i < end; ++i)
    {
        const auto job = m_timerInfos.at(i);
        job->save( cfg, QStringLiteral( "Job%1" ).arg( i ) );
    }

    KConfigGroup jobscfg = cfg->group("Jobs");
    jobscfg.writeEntry( "Number", m_timerInfos.count());
    jobscfg.sync();
}
