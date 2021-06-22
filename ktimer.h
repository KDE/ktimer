/*
 * Copyright 2001 Stefan Schimanski <1Stein@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KTIMER_H_INCLUDED
#define KTIMER_H_INCLUDED

#include <QDialog>
#include <QWidget>
#include <QProcess>
#include "ui_prefwidget.h"

class QTreeWidgetItem;
class KConfig;

class KTimerJob : public QObject {
 Q_OBJECT

 public:
    explicit KTimerJob( QObject *parent=nullptr);
    virtual ~KTimerJob();

    enum States { Stopped, Paused, Started };

    unsigned delay() const;
    QString command() const;
    bool loop() const;
    bool oneInstance() const;
    bool consecutive() const;
    unsigned value() const;
    States state() const;
    void *user();
    void setUser( void *user );

    void load( KConfig *cfg, const QString& grp );
    void save( KConfig *cfg, const QString& grp );
    QString formatTime( int seconds ) const;
    int timeToSeconds( int hours, int minutes, int seconds ) const;
    void secondsToHMS( int secs, int *hours, int *minutes, int *seconds ) const;


 public Q_SLOTS:
    void setDelay( unsigned int sec );
    void setDelay( int sec );
    void setCommand( const QString &cmd );
    void setLoop( bool loop );
    void setOneInstance( bool one );
    void setConsecutive( bool consecutive );
    void setValue( unsigned int value );
    void setValue( int value );
    void setState( KTimerJob::States state );

    void pause();
    void stop();
    void start();

 Q_SIGNALS:
    void stateChanged( KTimerJob *job, KTimerJob::States state );
    void delayChanged( KTimerJob *job, unsigned int sec );
    void commandChanged( KTimerJob *job, const QString &cmd );
    void loopChanged( KTimerJob *job, bool loop );
    void oneInstanceChanged( KTimerJob *job, bool one );
    void consecutiveChanged( KTimerJob *job, bool consecutive );
    void valueChanged( KTimerJob *job, unsigned int value );

    void changed( KTimerJob *job );
    void fired( KTimerJob *job );
    void finished( KTimerJob *job, bool error );
    void error( KTimerJob *job );

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
    explicit KTimerPref( QWidget *parent=nullptr);
    virtual ~KTimerPref();

 public Q_SLOTS:
    void exit();
    void done(int result) Q_DECL_OVERRIDE;

 protected Q_SLOTS:
    void add();
    void remove();
    void help();
    void currentChanged( QTreeWidgetItem * , QTreeWidgetItem *);

    void saveJobs( KConfig *cfg );
    void loadJobs( KConfig *cfg );
    void saveAllJobs();

 private Q_SLOTS:
    void jobChanged( KTimerJob *job );
    void jobFinished( KTimerJob *job, bool error );
    void delayChanged();

 private:
    struct KTimerPrefPrivate *d;
};

#endif
