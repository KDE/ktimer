/*
 * (c) 2001 Stefan Schimanski <1Stein@gmx.de>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef KTIMER_H_INCLUDED
#define KTIMER_H_INCLUDED

#include <qdialog.h>
#include <qwidget.h>
#include <kprocess.h>
#include <kconfig.h>

#include "prefwidget.h"


class KTimerJob : public QObject {
 Q_OBJECT

 public:
    KTimerJob( QObject *parent=0, const char *name=0 );
    virtual ~KTimerJob();

    enum States { Stopped, Paused, Started };

    unsigned delay();
    QString command();
    bool loop();
    bool oneInstance();
    unsigned value();
    States state();
    void *user();    
    void setUser( void *user );

    void load( KConfig *cfg, const QString& grp );
    void save( KConfig *cfg, const QString& grp );

 public slots:
    void setDelay( unsigned sec );
    void setDelay( int sec ) { setDelay( (unsigned)sec ); };
    void setCommand( const QString &cmd );
    void setLoop( bool loop );
    void setOneInstance( bool one );
    void setValue( unsigned value );
    void setValue( int value ) { setValue( (unsigned)value ); };
    void setState( States state ); 

    void pause() { setState( Paused ); }
    void stop() { setState( Stopped ); }
    void start() { setState( Started ); }    

 signals:
    void stateChanged( KTimerJob *job, States state );
    void delayChanged( KTimerJob *job, unsigned sec );
    void commandChanged( KTimerJob *job, const QString &cmd );
    void loopChanged( KTimerJob *job, bool loop );
    void oneInstanceChanged( KTimerJob *job, bool one );
    void valueChanged( KTimerJob *job, unsigned value );

    void changed( KTimerJob *job );
    void fired( KTimerJob *job );
    void finished( KTimerJob *job, bool error );
    void error( KTimerJob *job );

 protected slots:
    virtual void fire();

 private slots:
    void timeout(); 
    void processExited(KProcess *proc);

 private:
    struct KTimerJobPrivate *d;
};


class KTimerPref : public PrefWidget
{
    Q_OBJECT
 public:
    KTimerPref( QWidget *parent=0, const char *name = 0 );
    virtual ~KTimerPref();

 protected slots:
    void add();
    void remove();
    void currentChanged( QListViewItem * );    
    void browse();

    void saveJobs( KConfig *cfg );
    void loadJobs( KConfig *cfg );

 private slots:
    void jobChanged( KTimerJob *job );
    void jobFinished( KTimerJob *job, bool error );
   
 private:
    struct KTimerPrefPrivate *d;
};

#endif
