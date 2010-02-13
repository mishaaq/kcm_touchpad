/*
   Copyright (C) 2009 by Andrey Borzenkov <arvidjaar at mail.ru>


   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 */

#include <KUniqueApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KDebug>
#include <KLocale>

#include "ksyndaemon.h"

int main(int argc, char **argv)
{
    KAboutData aboutdata("ksyndaemon", "kcm_touchpad", ki18n("KSyndaemon"),
                         "0.1", ki18n("KDE Synaptics touchpad activity monitor"),
                         KAboutData::License_GPL, ki18n("(C) 2009"));
    aboutdata.addAuthor(ki18n("Andrey Borzenkov"),ki18n("Author"),"arvidjaar@main.ru");
    aboutdata.setBugAddress("http://kde-apps.org/content/show.php/kcm_touchpad?content=113335");

    KCmdLineArgs::init( argc, argv, &aboutdata );
    KSyndaemon::addCmdLineOptions();

    // initialize application
    if ( !KSyndaemon::start() ) {
        kDebug() << "Running ksyndaemon found";
        return 0;
    }

    // do not connect to ksmserver at all, ksyndaemon is launched on demand
    // and doesn't need to know about logout
    unsetenv( "SESSION_MANAGER" ); 
    KSyndaemon(app);
    
    // start syndaemon
    // listen to D-Bus reconfiguration events
    app.exec();
}

