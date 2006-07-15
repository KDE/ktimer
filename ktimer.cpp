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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QTimer>
#include <qtoolbutton.h>
#include <q3groupbox.h>
#include <q3listview.h>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <qslider.h>
#include <qlcdnumber.h>
//Added by qt3to4:
#include <QPixmap>
#include <QList>
#include <kurlrequester.h>
#include <klineedit.h>

#include <kiconloader.h>
#include <kapplication.h>
#include <ksystemtray.h>
#include <kfiledialog.h>

#include "ktimer.h"
#include <QPushButton>
#include <kglobal.h>


class KTimerJobItem : public Q3ListViewItem {
public:
    KTimerJobItem( KTimerJob *job, Q3ListView *parent )
        : Q3ListViewItem( parent ) {
        m_job = job;
        m_error = false;
        update();
    };

    KTimerJobItem( KTimerJob *job, Q3ListView *parent, Q3ListViewItem *after )
        : Q3ListViewItem( parent, after ) {
        m_job = job;
        m_error = false;
        update();
    };

    virtual ~KTimerJobItem() {
        delete m_job;
    };

    KTimerJob *job() { return m_job; };

    void setStatus( bool error ) {
        m_error = error;
        update();
    }

    void update() {
        setText( 0, QString::number(m_job->value()) );

        if( m_error )
            setPixmap( 0, SmallIcon("stop") );
        else
            setPixmap( 0, QPixmap() );

        setText( 1, QString::number(m_job->delay()) );

        switch( m_job->state() ) {
            case KTimerJob::Stopped: setPixmap( 2, SmallIcon("player_stop") ); break;
            case KTimerJob::Paused: setPixmap( 2, SmallIcon("player_pause") ); break;
            case KTimerJob::Started: setPixmap( 2, SmallIcon("1rightarrow") ); break;
        }

        setText( 3, m_job->command() );
    };

private:
    bool m_error;
    KTimerJob *m_job;
};


/***************************************************************/


struct KTimerPrefPrivate
{
    QList<KTimerJob *> jobs;
};

KTimerPref::KTimerPref( QWidget *parent, const char *name )
    : QDialog( parent, name )
{
    d = new KTimerPrefPrivate;

    setupUi(this);

    // set icons
    m_stop->setIcon( SmallIconSet("player_stop") );
    m_pause->setIcon( SmallIconSet("player_pause") );
    m_start->setIcon( SmallIconSet("1rightarrow") );

    // create tray icon
    KSystemTray *tray = new KSystemTray( this );
    tray->show();
    tray->setIcon( SmallIcon( "ktimer" ) );

    // connect
    connect( m_add, SIGNAL(clicked()), SLOT(add()) );
    connect( m_remove, SIGNAL(clicked()), SLOT(remove()) );
    connect( m_list, SIGNAL(currentChanged(Q3ListViewItem*)),
             SLOT(currentChanged(Q3ListViewItem*)) );
    loadJobs( KGlobal::config() );

    show();
}


KTimerPref::~KTimerPref()
{
    saveJobs( KGlobal::config() );
    delete d;
}


void KTimerPref::add()
{
    KTimerJob *job = new KTimerJob;
    KTimerJobItem *item = new KTimerJobItem( job, m_list );

    connect( job, SIGNAL(delayChanged(KTimerJob*,unsigned int)),
             SLOT(jobChanged(KTimerJob*)) );
    connect( job, SIGNAL(valueChanged(KTimerJob*,unsigned int)),
             SLOT(jobChanged(KTimerJob*)) );
    connect( job, SIGNAL(stateChanged(KTimerJob*,States)),
             SLOT(jobChanged(KTimerJob*)) );
    connect( job, SIGNAL(commandChanged(KTimerJob*,const QString&)),
             SLOT(jobChanged(KTimerJob*)) );
    connect( job, SIGNAL(finished(KTimerJob*,bool)),
             SLOT(jobFinished(KTimerJob*,bool)) );

    job->setUser( item );

    // Qt drops currentChanged signals on first item (bug?)
    if( m_list->childCount()==1 )
      currentChanged( item );

    m_list->setCurrentItem( item );
    m_list->triggerUpdate();
}


void KTimerPref::remove()
{
    delete m_list->currentItem();
    m_list->triggerUpdate();
}


void KTimerPref::currentChanged( Q3ListViewItem *i )
{
    KTimerJobItem *item = static_cast<KTimerJobItem*>(i);
    if( item ) {
        KTimerJob *job = item->job();

        m_state->setEnabled( true );
        m_settings->setEnabled( true );
        m_remove->setEnabled( true );

        m_delay->disconnect();
        m_loop->disconnect();
        m_one->disconnect();
        m_start->disconnect();
        m_pause->disconnect();
        m_stop->disconnect();
        m_counter->disconnect();
        m_slider->disconnect();

        connect( m_commandLine->lineEdit(), SIGNAL(textChanged(const QString &)),
                 job, SLOT(setCommand(const QString &)) );
        connect( m_delay, SIGNAL(valueChanged(int)),
                 job, SLOT(setDelay(int)) );
        connect( m_loop, SIGNAL(toggled(bool)),
                 job, SLOT(setLoop(bool)) );
        connect( m_one, SIGNAL(toggled(bool)),
                 job, SLOT(setOneInstance(bool)) );
        connect( m_stop, SIGNAL(clicked()),
                 job, SLOT(stop()) );
        connect( m_pause, SIGNAL(clicked()),
                 job, SLOT(pause()) );
        connect( m_start, SIGNAL(clicked()),
                 job, SLOT(start()) );
        connect( m_slider, SIGNAL(valueChanged(int)),
                 job, SLOT(setValue(int)) );

        m_commandLine->lineEdit()->setText( job->command() );
        m_delay->setValue( job->delay() );
        m_loop->setChecked( job->loop() );
        m_one->setChecked( job->oneInstance() );
        m_counter->display( (int)job->value() );
        m_slider->setMaximum( job->delay() );
        m_slider->setValue( job->value() );

    } else {
        m_state->setEnabled( false );
        m_settings->setEnabled( false );
        m_remove->setEnabled( false );
    }
}


void KTimerPref::jobChanged( KTimerJob *job )
{
    KTimerJobItem *item = static_cast<KTimerJobItem*>(job->user());
    if( item ) {
        item->update();
        m_list->triggerUpdate();

        if( item==m_list->currentItem() ) {

            // XXX optimize
            m_slider->setMaximum( job->delay() );
            m_slider->setValue( job->value() );
            m_counter->display( (int)job->value() );
        }
    }
}


void KTimerPref::jobFinished( KTimerJob *job, bool error )
{
    KTimerJobItem *item = static_cast<KTimerJobItem*>(job->user());
    item->setStatus( error );
    m_list->triggerUpdate();
}


void KTimerPref::saveJobs( KConfig *cfg )
{
    int num = 0;
    KTimerJobItem *item = static_cast<KTimerJobItem*>(m_list->firstChild());
    while( item ) {
        item->job()->save( cfg, QString("Job%1").arg( num ) );
        item = static_cast<KTimerJobItem*>(item->nextSibling());
        num++;
    }

    cfg->setGroup( "Jobs" );
    cfg->writeEntry( "Number", num );

    cfg->sync();
}


void KTimerPref::loadJobs( KConfig *cfg )
{
    cfg->setGroup( "Jobs" );
    int num = cfg->readEntry( "Number", 0 );
    for( int n=0; n<num; n++ ) {
            KTimerJob *job = new KTimerJob;
            KTimerJobItem *item = new KTimerJobItem( job, m_list );

            connect( job, SIGNAL(delayChanged(KTimerJob*,unsigned int)),
                     SLOT(jobChanged(KTimerJob*)) );
            connect( job, SIGNAL(valueChanged(KTimerJob*,unsigned int)),
                     SLOT(jobChanged(KTimerJob*)) );
            connect( job, SIGNAL(stateChanged(KTimerJob*,States)),
                     SLOT(jobChanged(KTimerJob*)) );
            connect( job, SIGNAL(commandChanged(KTimerJob*,const QString&)),
                     SLOT(jobChanged(KTimerJob*)) );
            connect( job, SIGNAL(finished(KTimerJob*,bool)),
                     SLOT(jobFinished(KTimerJob*,bool)) );

            job->load( cfg, QString( "Job%1" ).arg(n) );

            job->setUser( item );
    }

    m_list->triggerUpdate();
}


/*********************************************************************/


struct KTimerJobPrivate {
    unsigned delay;
    QString command;
    bool loop;
    bool oneInstance;
    unsigned value;
    KTimerJob::States state;
    QList<KProcess *> processes;
    void *user;

    QTimer *timer;
};


KTimerJob::KTimerJob( QObject *parent, const char *name )
    : QObject( parent, name )
{
    d = new KTimerJobPrivate;

    d->delay = 100;
    d->loop = false;
    d->oneInstance = true;
    d->value = 100;
    d->state = Stopped;
    d->user = 0;

    d->timer = new QTimer( this );
    connect( d->timer, SIGNAL(timeout()), SLOT(timeout()) );
}


KTimerJob::~KTimerJob()
{
    delete d;
}


void KTimerJob::load( KConfig *cfg, const QString& grp )
{
    cfg->setGroup( grp );
    cfg->writeEntry( "Delay", d->delay );
    cfg->writePathEntry( "Command", d->command );
    cfg->writeEntry( "Loop", d->loop );
    cfg->writeEntry( "OneInstance", d->oneInstance );
    cfg->writeEntry( "State", (int)d->state );
}


void KTimerJob::save( KConfig *cfg, const QString& grp )
{
    cfg->setGroup( grp );
    setDelay( cfg->readEntry( "Delay", 100 ) );
    setCommand( cfg->readPathEntry( "Command" ) );
    setLoop( cfg->readEntry( "Loop", false ) );
    setOneInstance( cfg->readEntry( "OneInstance", d->oneInstance ) );
    setState( (States)cfg->readEntry( "State", (int)Stopped ) );
}


void *KTimerJob::user()
{
    return d->user;
}


void KTimerJob::setUser( void *user )
{
    d->user = user;
}


unsigned KTimerJob::delay() const
{
    return d->delay;
}


void KTimerJob::pause()
{
    setState( Paused );
}

void KTimerJob::stop()
{
    setState( Stopped );
}

void KTimerJob::start()
{
    setState( Started );
}

void KTimerJob::setDelay( int sec )
{
    setDelay( (unsigned)sec );
}

void KTimerJob::setValue( int value )
{
    setValue( (unsigned)value );
}

void KTimerJob::setDelay( unsigned sec )
{
    if( d->delay!=sec ) {
        d->delay = sec;

        if( d->state==Stopped )
            setValue( sec );

        emit delayChanged( this, sec );
        emit changed( this );
    }
}


QString KTimerJob::command() const
{
    return d->command;
}


void KTimerJob::setCommand( const QString &cmd )
{
    if( d->command!=cmd ) {
        d->command = cmd;
        emit commandChanged( this, cmd );
        emit changed( this );
    }
}


bool KTimerJob::loop() const
{
    return d->loop;
}


void KTimerJob::setLoop( bool loop )
{
    if( d->loop!=loop ) {
        d->loop = loop;
        emit loopChanged( this, loop );
        emit changed( this );
    }
}


bool KTimerJob::oneInstance() const
{
    return d->oneInstance;
}


void KTimerJob::setOneInstance( bool one )
{
    if( d->oneInstance!=one ) {
        d->oneInstance = one;
        emit oneInstanceChanged( this, one );
        emit changed( this );
    }
}


unsigned KTimerJob::value() const
{
    return d->value;
}


void KTimerJob::setValue( unsigned value )
{
    if( d->value!=value ) {
        d->value = value;
        emit valueChanged( this, value );
        emit changed( this );
    }
}


KTimerJob::States KTimerJob::state() const
{
    return d->state;
}


void KTimerJob::setState( KTimerJob::States state )
{
    if( d->state!=state ) {
        if( state==Started )
            d->timer->start( 1000 );
        else
            d->timer->stop();

        if( state==Stopped )
            setValue( d->delay );

        d->state = state;
        emit stateChanged( this, state );
        emit changed( this );
    }
}


void KTimerJob::timeout()
{
    if( d->state==Started && d->value!=0 ) {
        setValue( d->value-1 );
        if( d->value==0 ) {
            fire();
            if( d->loop )
                setValue( d->delay );
            else
                stop();
        }
    }
}


void KTimerJob::processExited(KProcess *proc)
{
    bool ok = proc->exitStatus()==0;
    int i = d->processes.indexOf( proc);
    if (i != -1)
        delete d->processes.takeAt(i);

    if( !ok ) emit error( this );
    emit finished( this, !ok );
}


void KTimerJob::fire()
{
    if( !d->oneInstance || d->processes.isEmpty() ) {
        KShellProcess *proc = new KShellProcess;
        (*proc) << d->command;
        d->processes.append( proc );
        connect( proc, SIGNAL(processExited(KProcess*)),
                 SLOT(processExited(KProcess*)) );
        bool ok = proc->start( KProcess::NotifyOnExit );
        emit fired( this );
        if( !ok ) {
            int i = d->processes.indexOf( proc);
            if (i != -1)
                delete d->processes.takeAt(i);
            emit error( this );
            emit finished( this, true );
        }
    }
}
#include "ktimer.moc"
