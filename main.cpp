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

#include "ktimer.h"
#include <KAboutData>
#include <KDBusService>
#include <KLocalizedString>
#include <Kdelibs4ConfigMigrator>
#include <QApplication>
#include <QCommandLineParser>
#include <config-ktimer.h>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    /**
     * enable high dpi support
     */
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    Kdelibs4ConfigMigrator migrate(QStringLiteral("ktimer"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("ktimerrc"));
    migrate.migrate();
#endif
    KLocalizedString::setApplicationDomain("ktimer");
    KAboutData aboutData(QStringLiteral("ktimer"),
                         i18n("KTimer"),
                         QStringLiteral(KTIMER_VERSION),
                         i18n("KDE Timer"),
                         KAboutLicense::GPL,
                         i18n("(c) 2001, Stefan Schimanski"),
                         QStringLiteral("https://utils.kde.org/projects/ktimer/"));
    aboutData.addAuthor(i18n("Stefan Schimanski"), QString(), QStringLiteral("schimmi@kde.org"));
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    app.setQuitOnLastWindowClosed(false);
    KDBusService service;

    auto timer = new KTimerPref;
    timer->show();

    return app.exec();
}
