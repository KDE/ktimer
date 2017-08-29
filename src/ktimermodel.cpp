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
        case VALUE: return job->formatTime(job->delay());
        case CURRENT: return job->formatTime(job->value());
        case STATE: return job->state();
        case COMMAND: return job->command();
        case LOOP: return job->loop();
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
    return {
        {VALUE, "value"},
        {CURRENT, "current"},
        {STATE, "state"},
        {LOOP, "loop"},
        {COMMAND, "command"}
    };
}

void KTimerModel::loadJobs()
{
    auto cfg = KSharedConfig::openConfig().data();
    const int numJobs = cfg->group("Jobs").readEntry( "Number", 0 );
    beginResetModel();
    qDebug() << "Reading " << numJobs;
    for (int n = 0; n < numJobs; ++n) {
        auto *job = new KTimerJob();
        job->load(cfg, QStringLiteral("Job%1").arg(n));
        m_timerInfos.append(job);
    }
    endResetModel();
}
