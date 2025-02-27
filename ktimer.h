/*
 * SPDX-FileCopyrightText: 2001 Stefan Schimanski <1Stein@gmx.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KTIMER_H_INCLUDED
#define KTIMER_H_INCLUDED

#include "ui_prefwidget.h"
#include <QDialog>
#include <QProcess>
#include <QWidget>

class QTreeWidgetItem;
class KConfig;

class KTimerJob : public QObject
{
    Q_OBJECT

public:
    explicit KTimerJob(QObject *parent = nullptr);
    ~KTimerJob() override;

    enum States {
        Stopped,
        Paused,
        Started
    };

    unsigned delay() const;
    QString command() const;
    bool loop() const;
    bool oneInstance() const;
    bool consecutive() const;
    unsigned value() const;
    States state() const;
    void *user();
    void setUser(void *user);

    void load(KConfig *cfg, const QString &grp);
    void save(KConfig *cfg, const QString &grp);
    QString formatTime(int seconds) const;
    int timeToSeconds(int hours, int minutes, int seconds) const;
    void secondsToHMS(int secs, int *hours, int *minutes, int *seconds) const;

public Q_SLOTS:
    void setDelay(unsigned int sec);
    void setDelay(int sec);
    void setCommand(const QString &cmd);
    void setLoop(bool loop);
    void setOneInstance(bool one);
    void setConsecutive(bool consecutive);
    void setValue(unsigned int value);
    void setValue(int value);
    void setState(KTimerJob::States state);

    void pause();
    void stop();
    void start();

Q_SIGNALS:
    void stateChanged(KTimerJob *job, KTimerJob::States state);
    void delayChanged(KTimerJob *job, unsigned int sec);
    void commandChanged(KTimerJob *job, const QString &cmd);
    void loopChanged(KTimerJob *job, bool loop);
    void oneInstanceChanged(KTimerJob *job, bool one);
    void consecutiveChanged(KTimerJob *job, bool consecutive);
    void valueChanged(KTimerJob *job, unsigned int value);

    void changed(KTimerJob *job);
    void fired(KTimerJob *job);
    void finished(KTimerJob *job, bool error);
    void error(KTimerJob *job);

protected Q_SLOTS:
    virtual void fire();

private Q_SLOTS:
    void timeout();
    void processExited(int, QProcess::ExitStatus);

private:
    struct KTimerJobPrivate *d;
};

class KTimerPref : public QDialog, public Ui::PrefWidget
{
    Q_OBJECT
public:
    explicit KTimerPref(QWidget *parent = nullptr);
    ~KTimerPref() override;

public Q_SLOTS:
    void exit();
    void done(int result) Q_DECL_OVERRIDE;

protected Q_SLOTS:
    void add();
    void remove();
    void help();
    void currentChanged(QTreeWidgetItem *, QTreeWidgetItem *);

    void saveJobs(KConfig *cfg);
    void loadJobs(KConfig *cfg);
    void saveAllJobs();

private Q_SLOTS:
    void jobChanged(KTimerJob *job);
    void jobFinished(KTimerJob *job, bool error);
    void delayChanged();

private:
    struct KTimerPrefPrivate *d;
};

#endif
