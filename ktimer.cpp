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

#include "ktimer.h"

#include <QProcess>
#include <QTimer>

#include <klineedit.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <ktoolinvocation.h>
#include <kstandardguiitem.h>
#include <kaction.h>
#include <kstandardaction.h>
#include "kstatusnotifieritem.h"

class KTimerJobItem : public QTreeWidgetItem {
public:
    KTimerJobItem( KTimerJob *job, QTreeWidget *parent )
        : QTreeWidgetItem() {
			parent->addTopLevelItem(this);
        m_job = job;
        m_error = false;
        update();
    }

    KTimerJobItem( KTimerJob *job, QTreeWidget * parent, QTreeWidgetItem *after )
        : QTreeWidgetItem() {
			int otherItemIndex = parent->indexOfTopLevelItem(after);
			parent->insertTopLevelItem(otherItemIndex + 1, this);
        m_job = job;
        m_error = false;
        update();
    }

    virtual ~KTimerJobItem() {
        delete m_job;
    }

    KTimerJob *job() { return m_job; }

    void setStatus( bool error ) {
        m_error = error;
        update();
    }

    void update() {
        setText( 0, m_job->formatTime(m_job->value()) );

        if( m_error )
            setIcon( 0, KIcon( QLatin1String( "process-stop" )) );
        else
            setIcon( 0, QPixmap() );

        setText( 1, m_job->formatTime(m_job->delay()) );

        switch( m_job->state() ) {
            case KTimerJob::Stopped: setIcon( 2, KIcon( QLatin1String( "media-playback-stop" )) ); break;
            case KTimerJob::Paused: setIcon( 2, KIcon( QLatin1String( "media-playback-pause" )) ); break;
            case KTimerJob::Started: setIcon( 2, KIcon( QLatin1String( "arrow-right" )) ); break;
        }

        setText( 3, m_job->command() );
    }

private:
    bool m_error;
    KTimerJob *m_job;
};


/***************************************************************/


struct KTimerPrefPrivate
{
    QList<KTimerJob *> jobs;
};

KTimerPref::KTimerPref( QWidget *parent)
    : QDialog( parent )
{
    d = new KTimerPrefPrivate;

    setupUi(this);

    // set icons
    m_stop->setIcon( KIcon( QLatin1String( "media-playback-stop" )) );
    m_pause->setIcon( KIcon( QLatin1String( "media-playback-pause" )) );
    m_start->setIcon( KIcon( QLatin1String( "arrow-right" )) );

    // create tray icon
    KStatusNotifierItem *tray = new KStatusNotifierItem(this);
    tray->setIconByName(QLatin1String( "ktimer" ));
    tray->setCategory(KStatusNotifierItem::ApplicationStatus);
    tray->setStatus(KStatusNotifierItem::Active);
    // set help button gui item
    m_help->setGuiItem(KStandardGuiItem::help());

    // Exit
    KAction *exit = KStandardAction::quit(this, SLOT(exit()), this);
    addAction(exit);

    // connect
    connect( m_add, SIGNAL(clicked()), SLOT(add()) );
    connect( m_remove, SIGNAL(clicked()), SLOT(remove()) );
    connect( m_help, SIGNAL(clicked()), SLOT(help()) );
    connect( m_list, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
             SLOT(currentChanged(QTreeWidgetItem*,QTreeWidgetItem*)) );
    loadJobs( KGlobal::config().data() );

    show();
}


KTimerPref::~KTimerPref()
{
    delete d;
}

void KTimerPref::saveAllJobs() {
    saveJobs( KGlobal::config().data() );
}


void KTimerPref::add()
{
    KTimerJob *job = new KTimerJob;
    KTimerJobItem *item = new KTimerJobItem( job, m_list );

    connect( job, SIGNAL(delayChanged(KTimerJob*,uint)),
             SLOT(jobChanged(KTimerJob*)) );
    connect( job, SIGNAL(valueChanged(KTimerJob*,uint)),
             SLOT(jobChanged(KTimerJob*)) );
    connect( job, SIGNAL(stateChanged(KTimerJob*,States)),
             SLOT(jobChanged(KTimerJob*)) );
    connect( job, SIGNAL(commandChanged(KTimerJob*,QString)),
             SLOT(jobChanged(KTimerJob*)) );
    connect( job, SIGNAL(finished(KTimerJob*,bool)),
             SLOT(jobFinished(KTimerJob*,bool)) );

    job->setUser( item );

    // Qt drops currentChanged signals on first item (bug?)
    if( m_list->topLevelItemCount()==1 )
      currentChanged( item , NULL);

    m_list->setCurrentItem( item );
    m_list->update();
}


void KTimerPref::remove()
{
    delete m_list->currentItem();
    m_list->update();
}

void KTimerPref::help()
{
    KToolInvocation::invokeHelp();
}

// note, don't use old, but added it so we can connect to the new one
void KTimerPref::currentChanged( QTreeWidgetItem *i , QTreeWidgetItem * /* old */)
{
    KTimerJobItem *item = static_cast<KTimerJobItem*>(i);
    if( item ) {
        KTimerJob *job = item->job();

        m_state->setEnabled( true );
        m_settings->setEnabled( true );
        m_remove->setEnabled( true );
        m_delayH->disconnect();
        m_delayM->disconnect();
        m_delay->disconnect();
        m_loop->disconnect();
        m_one->disconnect();
        m_start->disconnect();
        m_pause->disconnect();
        m_stop->disconnect();
        m_counter->disconnect();
        m_slider->disconnect();
		m_commandLine->disconnect();
		m_commandLine->lineEdit()->disconnect();

        // Set hour, minute and second QSpinBoxes before we connect to signals.
        int h, m, s;
        job->secondsToHMS( job->delay(), &h, &m, &s );
        m_delayH->setValue( h );
        m_delayM->setValue( m );
        m_delay->setValue( s );

        connect( m_commandLine->lineEdit(), SIGNAL(textChanged(QString)),
                 job, SLOT(setCommand(QString)) );
        connect( m_delayH, SIGNAL(valueChanged(int)),
                 SLOT(delayChanged()) );
        connect( m_delayM, SIGNAL(valueChanged(int)),
                 SLOT(delayChanged()) );
        connect( m_delay, SIGNAL(valueChanged(int)),
                 SLOT(delayChanged()) );
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
        m_list->update();

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
    m_list->update();
}

/* Hour/Minute/Second was changed. This slot calculates the seconds until we start
    the job and inform the current job */
void KTimerPref::delayChanged()
{
    KTimerJobItem *item = static_cast<KTimerJobItem*>(m_list->currentItem());
    if ( item ) {
        KTimerJob *job = item->job();
        int time_sec = job->timeToSeconds( m_delayH->value(), m_delayM->value(), m_delay->value() );
        job->setDelay( time_sec );
    }
}

// Really quits the application
void KTimerPref::exit() {
    done(0);
    qApp->quit();
}

void KTimerPref::done(int result) {
    saveAllJobs();
    QDialog::done(result);
}

void KTimerPref::saveJobs( KConfig *cfg )
{
	const int nbList=m_list->topLevelItemCount();
	for (int num = 0; num < nbList; ++num)
	{
		KTimerJobItem *item = static_cast<KTimerJobItem*>(m_list->topLevelItem(num));
        item->job()->save( cfg, QString(QLatin1String( "Job%1" )).arg( num ) );

	}

	KConfigGroup jobscfg = cfg->group("Jobs");
    jobscfg.writeEntry( "Number", m_list->topLevelItemCount());

    jobscfg.sync();
}


void KTimerPref::loadJobs( KConfig *cfg )
{
    const int num = cfg->group("Jobs").readEntry( "Number", 0 );
    for( int n=0; n<num; n++ ) {
            KTimerJob *job = new KTimerJob;
            KTimerJobItem *item = new KTimerJobItem( job, m_list );

            connect( job, SIGNAL(delayChanged(KTimerJob*,uint)),
                     SLOT(jobChanged(KTimerJob*)) );
            connect( job, SIGNAL(valueChanged(KTimerJob*,uint)),
                     SLOT(jobChanged(KTimerJob*)) );
            connect( job, SIGNAL(stateChanged(KTimerJob*,States)),
                     SLOT(jobChanged(KTimerJob*)) );
            connect( job, SIGNAL(commandChanged(KTimerJob*,QString)),
                     SLOT(jobChanged(KTimerJob*)) );
            connect( job, SIGNAL(finished(KTimerJob*,bool)),
                     SLOT(jobFinished(KTimerJob*,bool)) );

            job->load( cfg, QString( QLatin1String( "Job%1" ) ).arg(n) );

            job->setUser( item );
            jobChanged ( job);
    }

    m_list->update();
}


/*********************************************************************/


struct KTimerJobPrivate {
    unsigned delay;
    QString command;
    bool loop;
    bool oneInstance;
    unsigned value;
    KTimerJob::States state;
    QList<QProcess *> processes;
    void *user;

    QTimer *timer;
};


KTimerJob::KTimerJob( QObject *parent)
    : QObject( parent )
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


void KTimerJob::save( KConfig *cfg, const QString& grp )
{
	KConfigGroup groupcfg = cfg->group(grp);
    groupcfg.writeEntry( "Delay", d->delay );
    groupcfg.writePathEntry( "Command", d->command );
    groupcfg.writeEntry( "Loop", d->loop );
    groupcfg.writeEntry( "OneInstance", d->oneInstance );
    groupcfg.writeEntry( "State", (int)d->state );
}


void KTimerJob::load( KConfig *cfg, const QString& grp )
{
	KConfigGroup groupcfg = cfg->group(grp);
    setDelay( groupcfg.readEntry( "Delay", 100 ) );
    setCommand( groupcfg.readPathEntry( "Command", QString() ) );
    setLoop( groupcfg.readEntry( "Loop", false ) );
    setOneInstance( groupcfg.readEntry( "OneInstance", d->oneInstance ) );
    setState( (States)groupcfg.readEntry( "State", (int)Stopped ) );
}


// Format given seconds to hour:minute:seconds and return QString
QString KTimerJob::formatTime( int seconds ) const
{
    int h, m, s;
    secondsToHMS( seconds, &h, &m, &s );
    return QString( QLatin1String( "%1:%2:%3" ) ).arg( h ).arg( m, 2, 10, QLatin1Char( '0' ) ).arg( s,2, 10, QLatin1Char( '0' ) );
}


// calculate seconds from hour, minute and seconds, returns seconds
int KTimerJob::timeToSeconds( int hours, int minutes, int seconds ) const
{
    return hours * 3600 + minutes * 60 + seconds;
}


// calculates hours, minutes and seconds from given secs.
void KTimerJob::secondsToHMS( int secs, int *hours, int *minutes, int *seconds ) const
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


void KTimerJob::processExited(int, QProcess::ExitStatus status)
{
	QProcess * proc = static_cast<QProcess*>(sender());
    const bool ok = status==0;
    const int i = d->processes.indexOf( proc);
    if (i != -1)
        delete d->processes.takeAt(i);

    if( !ok ) emit error( this );
    emit finished( this, !ok );
}




void KTimerJob::fire()
{
    if( !d->oneInstance || d->processes.isEmpty() ) {
        QProcess *proc = new QProcess;
        d->processes.append( proc );
        connect( proc, SIGNAL(finished(int,QProcess::ExitStatus)),
                 SLOT(processExited(int,QProcess::ExitStatus)) );
        if (!d->command.simplified ().isEmpty()) {
	        proc->start(d->command);
	        emit fired( this );
        }
        if(proc->state() == QProcess::NotRunning) {
            const int i = d->processes.indexOf( proc);
            if (i != -1)
                delete d->processes.takeAt(i);
            emit error( this );
            emit finished( this, true );
        }
    }
}
#include "ktimer.moc"
