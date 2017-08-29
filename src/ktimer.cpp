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
#include "ktimerjob.h"

#include <KConfig>
#include <KConfigGroup>
#include <KGuiItem>
#include <KHelpClient>
#include <KLineEdit>
#include <KSharedConfig>
#include <KStandardAction>
#include <KStandardGuiItem>
#include <KStatusNotifierItem>
#include <KToolInvocation>

#include <QAction>
#include <QProcess>
#include <QTimer>
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
            setIcon( 0, QIcon::fromTheme( QStringLiteral( "process-stop" )) );
        else
            setIcon( 0, QPixmap() );

        setText( 1, m_job->formatTime(m_job->delay()) );

        switch( m_job->state() ) {
            case KTimerJob::Stopped: setIcon( 2, QIcon::fromTheme( QStringLiteral( "media-playback-stop" )) ); break;
            case KTimerJob::Paused: setIcon( 2, QIcon::fromTheme( QStringLiteral( "media-playback-pause" )) ); break;
            case KTimerJob::Started: setIcon( 2, QIcon::fromTheme( QStringLiteral( "arrow-right" )) ); break;
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
    m_stop->setIcon( QIcon::fromTheme( QStringLiteral( "media-playback-stop" )) );
    m_pause->setIcon( QIcon::fromTheme( QStringLiteral( "media-playback-pause" )) );
    m_start->setIcon( QIcon::fromTheme( QStringLiteral( "arrow-right" )) );

    // create tray icon
    KStatusNotifierItem *tray = new KStatusNotifierItem(this);
    tray->setIconByName(QStringLiteral( "ktimer" ));
    tray->setCategory(KStatusNotifierItem::ApplicationStatus);
    tray->setStatus(KStatusNotifierItem::Active);
    // set help button gui item
    KGuiItem::assign(m_help,KStandardGuiItem::help());

    // Exit
    QAction *exit = KStandardAction::quit(this, SLOT(exit()), this);
    addAction(exit);

    // connect
    connect(m_add, &QPushButton::clicked, this, &KTimerPref::add);
    connect(m_remove, &QPushButton::clicked, this, &KTimerPref::remove);
    connect(m_help, &QPushButton::clicked, this, &KTimerPref::help);
    connect(m_list, &QTreeWidget::currentItemChanged, this, &KTimerPref::currentChanged);
    loadJobs( KSharedConfig::openConfig().data() );

    show();
}


KTimerPref::~KTimerPref()
{
    delete d;
}

void KTimerPref::saveAllJobs() {
    saveJobs( KSharedConfig::openConfig().data() );
}


void KTimerPref::add()
{
    KTimerJob *job = new KTimerJob;
    KTimerJobItem *item = new KTimerJobItem( job, m_list );

    connect(job, &KTimerJob::delayChanged, this, &KTimerPref::jobChanged);
    connect(job, &KTimerJob::valueChanged, this, &KTimerPref::jobChanged);
    connect(job, &KTimerJob::stateChanged, this, &KTimerPref::jobChanged);
    connect(job, &KTimerJob::commandChanged, this, &KTimerPref::jobChanged);
    connect(job, &KTimerJob::finished, this, &KTimerPref::jobFinished);

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
    KHelpClient::invokeHelp();
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
        job->secondsToHMS( job->delay(), &h, &m, &s );
        m_delayH->setValue( h );
        m_delayM->setValue( m );
        m_delay->setValue( s );

        connect( m_commandLine->lineEdit(), SIGNAL(textChanged(QString)),
                 job, SLOT(setCommand(QString)) );
        connect(m_delayH, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &KTimerPref::delayChanged);
        connect(m_delayM, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &KTimerPref::delayChanged);
        connect(m_delay, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &KTimerPref::delayChanged);
        connect(m_loop, &QCheckBox::toggled, job, &KTimerJob::setLoop);
        connect(m_one, &QCheckBox::toggled, job, &KTimerJob::setOneInstance);
        connect(m_consecutive, &QCheckBox::toggled, job, &KTimerJob::setConsecutive);
        connect(m_stop, &QToolButton::clicked, job, &KTimerJob::stop);
        connect(m_pause, &QToolButton::clicked, job, &KTimerJob::pause);
        connect(m_start, &QToolButton::clicked, job, &KTimerJob::start);
        connect( m_slider, SIGNAL(valueChanged(int)),
                 job, SLOT(setValue(int)) );


        m_commandLine->lineEdit()->setText( job->command() );
        m_loop->setChecked( job->loop() );
        m_one->setChecked( job->oneInstance() );
        m_consecutive->setChecked( job->consecutive() );
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
    if( m_list->itemBelow(m_list->currentItem())!=nullptr && (static_cast<KTimerJobItem*>(m_list->itemBelow( m_list->currentItem() )))->job()->consecutive() ) {
        m_list->setCurrentItem( m_list->itemBelow( m_list->currentItem() ) );
        (static_cast<KTimerJobItem*>(m_list->currentItem()))->job()->start();
    }
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
        item->job()->save( cfg, QStringLiteral( "Job%1" ).arg( num ) );

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

            connect(job, &KTimerJob::delayChanged, this, &KTimerPref::jobChanged);
            connect(job, &KTimerJob::valueChanged, this, &KTimerPref::jobChanged);
            connect(job, &KTimerJob::stateChanged, this, &KTimerPref::jobChanged);
            connect(job, &KTimerJob::commandChanged, this, &KTimerPref::jobChanged);
            connect(job, &KTimerJob::finished, this, &KTimerPref::jobFinished);

            job->load( cfg, QStringLiteral( "Job%1" ).arg(n) );

            job->setUser( item );
            jobChanged ( job);
    }

    m_list->update();
}
