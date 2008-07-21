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

#include <kapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "ktimer.h"

static const char description[] =
        I18N_NOOP("KDE Timer");

static const char version[] = "v0.2";

int main( int argc, char **argv )
{
    KAboutData aboutData( "ktimer", 0, ki18n("KTimer"),
                          version, ki18n(description), KAboutData::License_GPL,
                          ki18n("(c) 2001, Stefan Schimanski"), KLocalizedString(),
                          "http://utils.kde.org/projects/ktimer");
    aboutData.addAuthor(ki18n("Stefan Schimanski"),KLocalizedString(), "schimmi@kde.org");
    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication app;

    KTimerPref *timer = new KTimerPref;
    app.setTopWidget( timer );

    return app.exec();
}
