#ifndef KTIMERJOB_H
#define KTIMERJOB_H

#include <QObject>
#include <QString>
#include <QProcess>

class KConfig;
class KTimerJob : public QObject {
 Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int delay READ delay WRITE setDelay NOTIFY delayChanged)
    Q_PROPERTY(QString command READ command WRITE setCommand NOTIFY commandChanged)
    Q_PROPERTY(bool loop READ loop WRITE setLoop NOTIFY loopChanged)
    Q_PROPERTY(bool oneInstance READ oneInstance WRITE setOneInstance NOTIFY oneInstanceChanged)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString formattedValue READ formattedValue WRITE setFormattedValue NOTIFY formattedValueChanged)
    Q_PROPERTY(States state READ state WRITE setState NOTIFY stateChanged)

public:
    KTimerJob( QObject *parent=nullptr);
    virtual ~KTimerJob();

    enum States { Stopped, Paused, Started };
    Q_ENUM(States)

    int delay() const;
    QString command() const;
    bool loop() const;
    bool oneInstance() const;
    int value() const;
    States state() const;
    QString formattedValue() const;
    QString name() const;
    void load( KConfig *cfg, const QString& grp );
    void save( KConfig *cfg, const QString& grp );
    QString formatTime( int seconds ) const;
    int timeToSeconds( int hours, int minutes, int seconds ) const;
    void secondsToHMS( int secs, int *hours, int *minutes, int *seconds ) const;

 public Q_SLOTS:
    void setDelay( int sec );
    void setCommand( const QString &cmd );
    void setLoop( bool loop );
    void setOneInstance( bool one );
    void setValue( int value );
    void setState( States state );
    void setFormattedValue(const QString& value);
    void setName(const QString& name);
    void setDelayFromString(const QString& value);

    void pause();
    void stop();
    void start();

 Q_SIGNALS:
    // formats the value in hh:mm:ss
    void formattedValueChanged(const QString& value);
    void stateChanged(States state);
    void delayChanged(int sec);
    void commandChanged(const QString &cmd);
    void loopChanged(bool loop);
    void oneInstanceChanged(bool one);
    void valueChanged(unsigned int value);
    void nameChanged(const QString& name);
    void changed();
    void fired();
    void finished(bool error);
    void error();

 protected Q_SLOTS:
    virtual void fire();

 private Q_SLOTS:
    void timeout();
    void processExited(int, QProcess::ExitStatus);

 private:
    struct KTimerJobPrivate *d;
};

#endif
