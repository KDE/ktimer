/*
 * Copyright 2001 Stefan Schimanski <1Stein@gmx.de>
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

#ifndef KTIMER_H_INCLUDED
#define KTIMER_H_INCLUDED

#include <QDialog>
#include <QWidget>
#include <QProcess>
#include "ui_prefwidget.h"

class QTreeWidgetItem;
class KConfig;

class KTimerJob;

class KTimerPref : public QDialog, public Ui::PrefWidget
{
    Q_OBJECT
 public:
    explicit KTimerPref( QWidget *parent=nullptr);
    virtual ~KTimerPref();

 public Q_SLOTS:
    void exit();
    void done(int result) Q_DECL_OVERRIDE;

 protected Q_SLOTS:
    void add();
    void remove();
    void help();
    void currentChanged( QTreeWidgetItem * , QTreeWidgetItem *);

    void saveJobs( KConfig *cfg );
    void loadJobs( KConfig *cfg );
    void saveAllJobs();

 private Q_SLOTS:
     /* The jobs are the input connection, retrieve with sender(). */
    void jobChanged();
    void jobFinished(bool error );
    void delayChanged();

 private:
    struct KTimerPrefPrivate *d;
};

#endif
