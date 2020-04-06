#include "ktimerjob.h"

#include <QProcess>
#include <QTimer>

#include <KConfig>
#include <KConfigGroup>

struct KTimerJobPrivate {
    int delay;
    QString command;
    bool loop;
    bool oneInstance;
    bool consecutive;
    int value;
    KTimerJob::States state;
    QList<QProcess *> processes;
    QString formattedValue;
    QString name;
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
    d->formattedValue = formatTime(d->value);
    d->timer = new QTimer( this );
    connect(d->timer, &QTimer::timeout, this, &KTimerJob::timeout);
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
    return QStringLiteral( "%1:%2:%3" ).arg( h, 2, 10, QLatin1Char('0'))
                                       .arg( m, 2, 10, QLatin1Char('0'))
                                       .arg( s, 2, 10, QLatin1Char('0'));
}

// calculate seconds from hour, minute and seconds, returns seconds
int KTimerJob::timeToSeconds( int hours, int minutes, int seconds ) const
{
    return hours * 3600 + minutes * 60 + seconds;
}

void KTimerJob::setDelayFromString(const QString& value)
{
    auto t = QTime::fromString(value, QStringLiteral("hh:mm:ss"));
    setDelay(t.hour() * 3600 + t.minute() * 60 + t.second());
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

int KTimerJob::delay() const
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
    if( d->delay!=sec ) {
        d->delay = sec;

        if( d->state==Stopped )
            setValue( sec );

        emit delayChanged(sec);
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
        emit commandChanged(cmd);
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
        emit loopChanged(loop);
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
        emit oneInstanceChanged(one);
    }
}

int KTimerJob::value() const
{
    return d->value;
}

void KTimerJob::setValue( int value )
{
    if( d->value!=value ) {
        d->value = value;
        setFormattedValue(formatTime(value));
        emit valueChanged(value);
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
        emit stateChanged(state);
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

    if( !ok ) emit error();
    emit finished(!ok);
}

bool KTimerJob::consecutive() const
{
    return d->consecutive;
}

void KTimerJob::setConsecutive(bool value)
{
    if (d->consecutive == value) {
        return;
    }

    d->consecutive = value;
    Q_EMIT consecutiveChanged(value);
}

void KTimerJob::fire()
{
    if( !d->oneInstance || d->processes.isEmpty() ) {
        QProcess *proc = new QProcess;
        d->processes.append( proc );
        connect(proc, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &KTimerJob::processExited);
        if (!d->command.simplified ().isEmpty()) {
	        proc->start(d->command);
	        emit fired();
        }
        if(proc->state() == QProcess::NotRunning) {
            const int i = d->processes.indexOf( proc);
            if (i != -1)
                delete d->processes.takeAt(i);
            emit error();
            emit finished(true);
        }
    }
}


QString KTimerJob::formattedValue() const
{
    return d->formattedValue;
}

void KTimerJob::setFormattedValue(const QString& value)
{
    if (d->formattedValue != value) {
        d->formattedValue = value;
        emit formattedValueChanged(value);
    }
}

QString KTimerJob::name() const
{
    return d->name;
}

void KTimerJob::setName(const QString& name)
{
    if (name != d->name) {
        d->name = name;
        emit nameChanged(d->name);
    }
}
