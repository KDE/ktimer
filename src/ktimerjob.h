#ifndef KTIMERJOB_H
#define KTIMERJOB_H

#include <QObject>
#include <QString>
#include <QProcess>

class KConfig;

class KTimerJob : public QObject {
 Q_OBJECT

 public:
    KTimerJob( QObject *parent=0);
    virtual ~KTimerJob();

    enum States { Stopped, Paused, Started };

    unsigned delay() const;
    QString command() const;
    bool loop() const;
    bool oneInstance() const;
    unsigned value() const;
    States state() const;
    void *user();
    void setUser( void *user );

    void load( KConfig *cfg, const QString& grp );
    void save( KConfig *cfg, const QString& grp );
    QString formatTime( int seconds ) const;
    int timeToSeconds( int hours, int minutes, int seconds ) const;
    void secondsToHMS( int secs, int *hours, int *minutes, int *seconds ) const;


 public slots:
    void setDelay( unsigned int sec );
    void setDelay( int sec );
    void setCommand( const QString &cmd );
    void setLoop( bool loop );
    void setOneInstance( bool one );
    void setValue( unsigned int value );
    void setValue( int value );
    void setState( States state );

    void pause();
    void stop();
    void start();

 signals:
    void stateChanged( KTimerJob *job, States state );
    void delayChanged( KTimerJob *job, unsigned int sec );
    void commandChanged( KTimerJob *job, const QString &cmd );
    void loopChanged( KTimerJob *job, bool loop );
    void oneInstanceChanged( KTimerJob *job, bool one );
    void valueChanged( KTimerJob *job, unsigned int value );

    void changed( KTimerJob *job );
    void fired( KTimerJob *job );
    void finished( KTimerJob *job, bool error );
    void error( KTimerJob *job );

 protected slots:
    virtual void fire();

 private slots:
    void timeout();
    void processExited(int, QProcess::ExitStatus);

 private:
    struct KTimerJobPrivate *d;
};

#endif
