/*
 * (c) 2001 Stefan Schimanski <1Stein@gmx.de>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <kapp.h>
#include <klocale.h>
#include <kstdaccel.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "ktimer.h"

static const char *description =
        I18N_NOOP("KDE Timer");

static const char *version = "v0.1";

int main( int argc, char **argv )
{
    KAboutData aboutData( "ktimer", I18N_NOOP("KTimer"),
                          version, description, KAboutData::License_GPL,
                          "(c) 2001, Stefan Schimanski");
    aboutData.addAuthor("Stefan Schimanski",0, "1Stein@gmx.de");
    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication app;

    KTimerPref *timer = new KTimerPref;
    app.setMainWidget( timer );
    app.setTopWidget( timer );

    return app.exec();
}
