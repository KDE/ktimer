/*
 * SPDX-FileCopyrightText: 2001 Stefan Schimanski <schimmi@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "ktimer.h"

#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <config-ktimer.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("ktimer"));
    KAboutData aboutData(QStringLiteral("ktimer"),
                         i18n("KTimer"),
                         QStringLiteral(KTIMER_VERSION),
                         i18n("KDE Timer"),
                         KAboutLicense::GPL,
                         i18n("(c) 2001, Stefan Schimanski"),
                         QStringLiteral("https://apps.kde.org/ktimer"));
    aboutData.addAuthor(i18n("Stefan Schimanski"), QString(), QStringLiteral("schimmi@kde.org"));
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("ktimer")));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KCrash::initialize();

    app.setQuitOnLastWindowClosed(false);
    KDBusService service;

    auto timer = new KTimerPref;
    timer->show();

    return app.exec();
}
