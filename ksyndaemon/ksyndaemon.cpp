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
#include <kdebug.h>
//#include <kglobal.h>
//#include <klocale.h>
//#include <kmessage.h>
//#include <kpassivepopupmessagehandler.h>
//#include <kdefakes.h>

#include "ksyndaemon.h"
#include "ksyndaemonadaptor.h"

KSyndaemon::KSyndaemon(void)
	: KUniqueApplication(false),
	m_interval(1),
	m_cmd("exec syndaemon -R -i "),
	daemon()
{
	new KSyndaemonAdaptor(this);
	QDBusConnection dbus = QDBusConnection::sessionBus();
	dbus.registerObject("/Syndaemon", this);
	dbus.registerService("org.kde.KSyndaemon");
}

KSyndaemon::~KSyndaemon(void)
{
	stopMonitoring();
}

void
KSyndaemon::setInterval(unsigned i)
{
	unsigned old = m_interval;

	m_interval = i;
	if (old != i && daemon.state() == QProcess::Running) {
		stopMonitoring();
		startMonitoring();
	}
}

void
KSyndaemon::startMonitoring(void)
{
	if (daemon.state() != QProcess::NotRunning)
		return;

	daemon.setShellCommand(m_cmd + QString::number(m_interval) + " >/dev/null");
	daemon.start();
}

void
KSyndaemon::stopMonitoring(void)
{
	if (daemon.state() == QProcess::Running) {
		daemon.terminate();
		daemon.waitForFinished();
	}
}

#include "ksyndaemon.moc"
