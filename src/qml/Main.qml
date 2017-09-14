/*
 *   Copyright 2016 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 2.1
import org.kde.kirigami 2.1 as Kirigami
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as QC2
import QtQuick.Controls 1.4 as QC1
import kde.ktimer.components 1.0

Kirigami.ApplicationWindow {
    id: root
    property QtObject currentTimer

    header: Kirigami.ApplicationHeader {}

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    pageStack.initialPage: mainPageComponent


    Component {
        id: mainPageComponent
        Kirigami.ScrollablePage {
            title: "Timers"
            actions.main: Kirigami.Action {
                iconName: "list-add"
                text: i18n("Add Timer")
                onTriggered : {
                    timerModel.createJob();
                }
            }
            background: Rectangle {
                color: Kirigami.Theme.viewBackgroundColor
            }
            ListView {
                id: timerView
                model: timerModel
                delegate : TimerJobDelegate {
                    jobModel : timerModel
                    onClicked : {
                        currentTimer = model.job
                        bottomDrawer.open()
                    }
                }
             }
        }
    }

    TimerModel {
        id: timerModel
        Component.onCompleted : {
            timerModel.loadJobs()
        }
        Component.onDestruction : {
            timerModel.saveJobs()
        }
    }

    Kirigami.OverlayDrawer {
        id: bottomDrawer
        edge: Qt.BottomEdge
        contentItem: Item {
            implicitHeight: childrenRect.height + Kirigami.Units.gridUnit
            ColumnLayout {
                anchors.centerIn: parent
                QC2.TextField {
                    placeholderText: "Name"
                    text: currentTimer ? currentTimer.name : ""
                    onEditingFinished : {
                        currentTimer.name = text
                    }
                }
                QC2.TextField { //TODO: Use a clock here.
                    placeholderText: "Duration"
                    text: currentTimer ? currentTimer.formattedValue : ""
                    onEditingFinished : {
                        currentTimer.setDelayFromString(text)
                    }
                }
                QC2.TextField {
                    placeholderText: "Run when finished"
                    text: currentTimer ? currentTimer.command : ""
                    onEditingFinished : {
                        currentTimer.command = text
                    }
                }
                Item {
                    Layout.minimumHeight: Kirigami.Units.gridUnit * 4
                }
            }
        }
    }


    onClosing : {
        close.accepted = false
        hide()
    }
}
