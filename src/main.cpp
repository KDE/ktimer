/*
 * Copyright 2001 Stefan Schimanski <schimmi@kde.org>
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



#include <KAboutData>
#include <QApplication>
#include <KLocalizedString>
#include <KStatusNotifierItem>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QWindow>

#include <QCommandLineParser>
#include <Kdelibs4ConfigMigrator>
#include <KDBusService>

#include <config-ktimer.h>
#include "ktimermodel.h"
#include "ktimerjob.h"

static const char description[] =
        I18N_NOOP("KDE Timer");

int main( int argc, char **argv )
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(qml);

    /**
     * enable high dpi support
     */
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    Kdelibs4ConfigMigrator migrate(QStringLiteral("ktimer"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("ktimerrc"));
    migrate.migrate();

    KLocalizedString::setApplicationDomain("ktimer");
    KAboutData aboutData( QStringLiteral("ktimer"), i18n("KTimer"),
                          QString::fromLatin1(KTIMER_VERSION), i18n(description), KAboutLicense::GPL,
                          i18n("(c) 2001, Stefan Schimanski"),
                          QStringLiteral("https://utils.kde.org/projects/ktimer/"));
    aboutData.addAuthor(i18n("Stefan Schimanski"),QString(), QStringLiteral("schimmi@kde.org"));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    app.setQuitOnLastWindowClosed( false );
    KDBusService service;

    qmlRegisterType<KTimerModel>("kde.ktimer.components", 1, 0, "TimerModel");
    qmlRegisterType<KTimerJob>("kde.ktimer.components", 1, 0, "TimerJob");

    auto *engine = new QQmlApplicationEngine("qrc:/KTimer/Main.qml");
    KStatusNotifierItem *statusNotifier = nullptr;
    for(auto obj : engine->rootObjects()) {
        if (auto window = qobject_cast<QWindow*>(obj)) {
            //TODO: find a way for this to work.
            statusNotifier = new KStatusNotifierItem();
            statusNotifier->setIconByName(QStringLiteral( "ktimer" ));
            statusNotifier->setCategory(KStatusNotifierItem::ApplicationStatus);
            statusNotifier->setStatus(KStatusNotifierItem::Active);
            QObject::connect(statusNotifier, &KStatusNotifierItem::activateRequested, [window]{
                window->show();
            });
            break;
        }
    }

    return app.exec();
}
