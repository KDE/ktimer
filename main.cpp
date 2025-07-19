/*
 * SPDX-FileCopyrightText: 2001 Stefan Schimanski <schimmi@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "ktimer.h"
using namespace Qt::Literals::StringLiterals;

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
    KAboutData aboutData(u"ktimer"_s,
                         i18n("KTimer"),
                         QStringLiteral(KTIMER_VERSION),
                         i18n("KDE Timer"),
                         KAboutLicense::GPL,
                         i18n("(c) 2001, Stefan Schimanski"),
                         u"https://apps.kde.org/ktimer"_s);
    aboutData.addAuthor(i18n("Stefan Schimanski"), QString(), u"schimmi@kde.org"_s);
    QApplication::setWindowIcon(QIcon::fromTheme(u"ktimer"_s));
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
