/*
 * SPDX-FileCopyrightText: 2001 Stefan Schimanski <1Stein@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "ktimer.h"
using namespace Qt::Literals::StringLiterals;


#include <KConfigGroup>
#include <KGuiItem>
#include <KHelpClient>
#include <KLineEdit>
#include <KSharedConfig>
#include <KStandardAction>
#include <KStandardGuiItem>
#include <KStatusNotifierItem>
#include <QAction>
#include <QTimer>

class KTimerJobItem : public QTreeWidgetItem
{
public:
    KTimerJobItem(KTimerJob *job, QTreeWidget *parent)
        : QTreeWidgetItem()
    {
        parent->addTopLevelItem(this);
        m_job = job;
        m_error = false;
        update();
    }

    KTimerJobItem(KTimerJob *job, QTreeWidget *parent, QTreeWidgetItem *after)
        : QTreeWidgetItem()
    {
        int otherItemIndex = parent->indexOfTopLevelItem(after);
        parent->insertTopLevelItem(otherItemIndex + 1, this);
        m_job = job;
        m_error = false;
        update();
    }

    ~KTimerJobItem() override
    {
        delete m_job;
    }

    KTimerJob *job()
    {
        return m_job;
    }

    void setStatus(bool error)
    {
        m_error = error;
        update();
    }

    void update()
    {
        setText(0, m_job->formatTime(m_job->value()));

        if (m_error)
            setIcon(0, QIcon::fromTheme(u"process-stop"_s));
        else
            setIcon(0, QPixmap());

        setText(1, m_job->formatTime(m_job->delay()));

        switch (m_job->state()) {
        case KTimerJob::Stopped:
            setIcon(2, QIcon::fromTheme(u"media-playback-stop"_s));
            break;
        case KTimerJob::Paused:
            setIcon(2, QIcon::fromTheme(u"media-playback-pause"_s));
            break;
        case KTimerJob::Started:
            setIcon(2, QIcon::fromTheme(u"arrow-right"_s));
            break;
        }

        setText(3, m_job->command());
    }

private:
    bool m_error;
    KTimerJob *m_job;
};

/***************************************************************/

struct KTimerPrefPrivate {
    QList<KTimerJob *> jobs;
};

KTimerPref::KTimerPref(QWidget *parent)
    : QDialog(parent)
{
    d = new KTimerPrefPrivate;

    setupUi(this);

    // set icons
    m_stop->setIcon(QIcon::fromTheme(u"media-playback-stop"_s));
    m_pause->setIcon(QIcon::fromTheme(u"media-playback-pause"_s));
    m_start->setIcon(QIcon::fromTheme(u"arrow-right"_s));

    // create tray icon
    auto tray = new KStatusNotifierItem(this);
    tray->setIconByName(u"ktimer"_s);
    tray->setCategory(KStatusNotifierItem::ApplicationStatus);
    tray->setStatus(KStatusNotifierItem::Active);
    // set help button gui item
    KGuiItem::assign(m_help, KStandardGuiItem::help());

    // Exit
    QAction *exitAction = KStandardAction::quit(this, SLOT(exit()), this);
    addAction(exitAction);

    // connect
    connect(m_add, &QPushButton::clicked, this, &KTimerPref::add);
    connect(m_remove, &QPushButton::clicked, this, &KTimerPref::remove);
    connect(m_help, &QPushButton::clicked, this, &KTimerPref::help);
    connect(m_list, &QTreeWidget::currentItemChanged, this, &KTimerPref::currentChanged);
    loadJobs(KSharedConfig::openConfig().data());

    show();
}

KTimerPref::~KTimerPref()
{
    delete d;
}

void KTimerPref::saveAllJobs()
{
    saveJobs(KSharedConfig::openConfig().data());
}

void KTimerPref::add()
{
    auto job = new KTimerJob;
    auto item = new KTimerJobItem(job, m_list);

    connect(job, &KTimerJob::delayChanged, this, &KTimerPref::jobChanged);
    connect(job, &KTimerJob::valueChanged, this, &KTimerPref::jobChanged);
    connect(job, &KTimerJob::stateChanged, this, &KTimerPref::jobChanged);
    connect(job, &KTimerJob::commandChanged, this, &KTimerPref::jobChanged);
    connect(job, &KTimerJob::finished, this, &KTimerPref::jobFinished);

    job->setUser(item);

    // Qt drops currentChanged signals on first item (bug?)
    if (m_list->topLevelItemCount() == 1)
        currentChanged(item, nullptr);

    m_list->setCurrentItem(item);
    m_list->update();
}

void KTimerPref::remove()
{
    delete m_list->currentItem();
    m_list->update();
}

void KTimerPref::help()
{
    KHelpClient::invokeHelp();
}

// note, don't use old, but added it so we can connect to the new one
void KTimerPref::currentChanged(QTreeWidgetItem *i, QTreeWidgetItem * /* old */)
{
    auto item = static_cast<KTimerJobItem *>(i);
    if (item) {
        KTimerJob *job = item->job();

        m_state->setEnabled(true);
        m_settings->setEnabled(true);
        m_remove->setEnabled(true);
        m_delayH->disconnect();
        m_delayM->disconnect();
        m_delay->disconnect();
        m_loop->disconnect();
        m_one->disconnect();
        m_consecutive->disconnect();
        m_start->disconnect();
        m_pause->disconnect();
        m_stop->disconnect();
        m_counter->disconnect();
        m_slider->disconnect();
        m_commandLine->disconnect();
        m_commandLine->lineEdit()->disconnect();

        // Set hour, minute and second QSpinBoxes before we connect to signals.
        int h, m, s;
        job->secondsToHMS(job->delay(), &h, &m, &s);
        m_delayH->setValue(h);
        m_delayM->setValue(m);
        m_delay->setValue(s);

        m_commandLine->lineEdit()->setText(job->command());
        m_loop->setChecked(job->loop());
        m_one->setChecked(job->oneInstance());
        m_consecutive->setChecked(job->consecutive());
        m_counter->display((int)job->value());
        m_slider->setMaximum(job->delay());
        m_slider->setValue(job->value());

        connect(m_commandLine->lineEdit(), &QLineEdit::textChanged, job, &KTimerJob::setCommand);
        connect(m_delayH, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &KTimerPref::delayChanged);
        connect(m_delayM, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &KTimerPref::delayChanged);
        connect(m_delay, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &KTimerPref::delayChanged);
        connect(m_loop, &QCheckBox::toggled, job, &KTimerJob::setLoop);
        connect(m_one, &QCheckBox::toggled, job, &KTimerJob::setOneInstance);
        connect(m_consecutive, &QCheckBox::toggled, job, &KTimerJob::setConsecutive);
        connect(m_stop, &QToolButton::clicked, job, &KTimerJob::stop);
        connect(m_pause, &QToolButton::clicked, job, &KTimerJob::pause);
        connect(m_start, &QToolButton::clicked, job, &KTimerJob::start);
        connect(m_slider, SIGNAL(valueChanged(int)), job, SLOT(setValue(int)));

    } else {
        m_state->setEnabled(false);
        m_settings->setEnabled(false);
        m_remove->setEnabled(false);
    }
}

void KTimerPref::jobChanged(KTimerJob *job)
{
    auto item = static_cast<KTimerJobItem *>(job->user());
    if (item) {
        item->update();
        m_list->update();

        if (item == m_list->currentItem()) {
            // XXX optimize
            m_slider->setMaximum(job->delay());
            m_slider->setValue(job->value());
            m_counter->display((int)job->value());
        }
    }
}

void KTimerPref::jobFinished(KTimerJob *job, bool error)
{
    auto item = static_cast<KTimerJobItem *>(job->user());
    item->setStatus(error);
    if (m_list->itemBelow(m_list->currentItem()) != nullptr && (static_cast<KTimerJobItem *>(m_list->itemBelow(m_list->currentItem())))->job()->consecutive()) {
        m_list->setCurrentItem(m_list->itemBelow(m_list->currentItem()));
        (static_cast<KTimerJobItem *>(m_list->currentItem()))->job()->start();
    }
    m_list->update();
}

/* Hour/Minute/Second was changed. This slot calculates the seconds until we start
    the job and inform the current job */
void KTimerPref::delayChanged()
{
    auto item = static_cast<KTimerJobItem *>(m_list->currentItem());
    if (item) {
        KTimerJob *job = item->job();
        int time_sec = job->timeToSeconds(m_delayH->value(), m_delayM->value(), m_delay->value());
        job->setDelay(time_sec);
    }
}

// Really quits the application
void KTimerPref::exit()
{
    done(0);
    qApp->quit();
}

void KTimerPref::done(int result)
{
    saveAllJobs();
    QDialog::done(result);
}

void KTimerPref::saveJobs(KConfig *cfg)
{
    const int nbList = m_list->topLevelItemCount();
    for (int num = 0; num < nbList; ++num) {
        auto item = static_cast<KTimerJobItem *>(m_list->topLevelItem(num));
        item->job()->save(cfg, u"Job%1"_s.arg(num));
    }

    KConfigGroup jobscfg = cfg->group(u"Jobs"_s);
    jobscfg.writeEntry("Number", m_list->topLevelItemCount());

    jobscfg.sync();
}

void KTimerPref::loadJobs(KConfig *cfg)
{
    const int num = cfg->group(u"Jobs"_s).readEntry("Number", 0);
    for (int n = 0; n < num; n++) {
        auto job = new KTimerJob;
        auto item = new KTimerJobItem(job, m_list);

        connect(job, &KTimerJob::delayChanged, this, &KTimerPref::jobChanged);
        connect(job, &KTimerJob::valueChanged, this, &KTimerPref::jobChanged);
        connect(job, &KTimerJob::stateChanged, this, &KTimerPref::jobChanged);
        connect(job, &KTimerJob::commandChanged, this, &KTimerPref::jobChanged);
        connect(job, &KTimerJob::finished, this, &KTimerPref::jobFinished);

        job->load(cfg, u"Job%1"_s.arg(n));

        job->setUser(item);
        jobChanged(job);
    }

    m_list->update();
}

/*********************************************************************/

struct KTimerJobPrivate {
    unsigned delay;
    QString command;
    bool loop;
    bool oneInstance;
    bool consecutive;
    unsigned value;
    KTimerJob::States state;
    QList<QProcess *> processes;
    void *user;

    QTimer *timer;
};

KTimerJob::KTimerJob(QObject *parent)
    : QObject(parent)
{
    d = new KTimerJobPrivate;

    d->delay = 100;
    d->loop = false;
    d->oneInstance = true;
    d->consecutive = false;
    d->value = 100;
    d->state = Stopped;
    d->user = nullptr;

    d->timer = new QTimer(this);
    connect(d->timer, &QTimer::timeout, this, &KTimerJob::timeout);
}

KTimerJob::~KTimerJob()
{
    delete d;
}

void KTimerJob::save(KConfig *cfg, const QString &grp)
{
    KConfigGroup groupcfg = cfg->group(grp);
    groupcfg.writeEntry("Delay", d->delay);
    groupcfg.writePathEntry("Command", d->command);
    groupcfg.writeEntry("Loop", d->loop);
    groupcfg.writeEntry("OneInstance", d->oneInstance);
    groupcfg.writeEntry("Consecutive", d->consecutive);
    groupcfg.writeEntry("State", (int)d->state);
}

void KTimerJob::load(KConfig *cfg, const QString &grp)
{
    KConfigGroup groupcfg = cfg->group(grp);
    setDelay(groupcfg.readEntry("Delay", 100));
    setCommand(groupcfg.readPathEntry("Command", QString()));
    setLoop(groupcfg.readEntry("Loop", false));
    setOneInstance(groupcfg.readEntry("OneInstance", d->oneInstance));
    setConsecutive(groupcfg.readEntry("Consecutive", d->consecutive));
    setState((States)groupcfg.readEntry("State", (int)Stopped));
}

// Format given seconds to hour:minute:seconds and return QString
QString KTimerJob::formatTime(int seconds) const
{
    int h, m, s;
    secondsToHMS(seconds, &h, &m, &s);
    return u"%1:%2:%3"_s.arg(h).arg(m, 2, 10, u'0').arg(s, 2, 10, u'0');
}

// calculate seconds from hour, minute and seconds, returns seconds
int KTimerJob::timeToSeconds(int hours, int minutes, int seconds) const
{
    return hours * 3600 + minutes * 60 + seconds;
}

// calculates hours, minutes and seconds from given secs.
void KTimerJob::secondsToHMS(int secs, int *hours, int *minutes, int *seconds) const
{
    int s = secs;
    (*hours) = s / 3600;
    s = s % 3600;
    (*minutes) = s / 60;
    (*seconds) = s % 60;
}

void *KTimerJob::user()
{
    return d->user;
}

void KTimerJob::setUser(void *user)
{
    d->user = user;
}

unsigned KTimerJob::delay() const
{
    return d->delay;
}

void KTimerJob::pause()
{
    setState(Paused);
}

void KTimerJob::stop()
{
    setState(Stopped);
}

void KTimerJob::start()
{
    setState(Started);
}

void KTimerJob::setDelay(int sec)
{
    setDelay((unsigned)sec);
}

void KTimerJob::setValue(int value)
{
    setValue((unsigned)value);
}

void KTimerJob::setDelay(unsigned int sec)
{
    if (d->delay != sec) {
        d->delay = sec;

        if (d->state == Stopped)
            setValue(sec);

        Q_EMIT delayChanged(this, sec);
        Q_EMIT changed(this);
    }
}

QString KTimerJob::command() const
{
    return d->command;
}

void KTimerJob::setCommand(const QString &cmd)
{
    if (d->command != cmd) {
        d->command = cmd;
        Q_EMIT commandChanged(this, cmd);
        Q_EMIT changed(this);
    }
}

bool KTimerJob::loop() const
{
    return d->loop;
}

void KTimerJob::setLoop(bool loop)
{
    if (d->loop != loop) {
        d->loop = loop;
        Q_EMIT loopChanged(this, loop);
        Q_EMIT changed(this);
    }
}

bool KTimerJob::oneInstance() const
{
    return d->oneInstance;
}

void KTimerJob::setOneInstance(bool one)
{
    if (d->oneInstance != one) {
        d->oneInstance = one;
        Q_EMIT oneInstanceChanged(this, one);
        Q_EMIT changed(this);
    }
}

bool KTimerJob::consecutive() const
{
    return d->consecutive;
}

void KTimerJob::setConsecutive(bool consecutive)
{
    if (d->consecutive != consecutive) {
        d->consecutive = consecutive;
        Q_EMIT consecutiveChanged(this, consecutive);
        Q_EMIT changed(this);
    }
}

unsigned KTimerJob::value() const
{
    return d->value;
}

void KTimerJob::setValue(unsigned int value)
{
    if (d->value != value) {
        d->value = value;
        Q_EMIT valueChanged(this, value);
        Q_EMIT changed(this);
    }
}

KTimerJob::States KTimerJob::state() const
{
    return d->state;
}

void KTimerJob::setState(KTimerJob::States state)
{
    if (d->state != state) {
        if (state == Started)
            d->timer->start(1000);
        else
            d->timer->stop();

        if (state == Stopped)
            setValue(d->delay);

        d->state = state;
        Q_EMIT stateChanged(this, state);
        Q_EMIT changed(this);
    }
}

void KTimerJob::timeout()
{
    if (d->state == Started && d->value != 0) {
        setValue(d->value - 1);
        if (d->value == 0) {
            fire();
            if (d->loop)
                setValue(d->delay);
            else
                stop();
        }
    }
}

void KTimerJob::processExited(int, QProcess::ExitStatus status)
{
    auto proc = static_cast<QProcess *>(sender());
    const bool ok = status == 0;
    const int i = d->processes.indexOf(proc);
    if (i != -1)
        delete d->processes.takeAt(i);

    if (!ok)
        Q_EMIT error(this);
    Q_EMIT finished(this, !ok);
}

void KTimerJob::fire()
{
    if (!d->oneInstance || d->processes.isEmpty()) {
        auto proc = new QProcess;
        d->processes.append(proc);
        connect(proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &KTimerJob::processExited);
        if (!d->command.simplified().isEmpty()) {
            QStringList splitArguments = QProcess::splitCommand(d->command);
            if (!splitArguments.isEmpty()) {
                const QString prog = splitArguments.takeFirst();
                proc->start(prog, splitArguments);
            }
            Q_EMIT fired(this);
        }
        if (proc->state() == QProcess::NotRunning) {
            const int i = d->processes.indexOf(proc);
            if (i != -1)
                delete d->processes.takeAt(i);
            Q_EMIT error(this);
            Q_EMIT finished(this, true);
        }
    }
}

#include "moc_ktimer.cpp"
