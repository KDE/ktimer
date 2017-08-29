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
    void stateChanged(States state);
    void delayChanged(unsigned int sec);
    void commandChanged(const QString &cmd);
    void loopChanged(bool loop);
    void oneInstanceChanged(bool one);
    void valueChanged(unsigned int value);

    void changed();
    void fired();
    void finished(bool error);
    void error();

 protected slots:
    virtual void fire();

 private slots:
    void timeout();
    void processExited(int, QProcess::ExitStatus);

 private:
    struct KTimerJobPrivate *d;
};

#endif
