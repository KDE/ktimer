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

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
    }

    pageStack.initialPage: mainPageComponent

    Component {
        id: mainPageComponent
        Kirigami.Page {
            title: "Hello"
            ColumnLayout {
                anchors.fill: parent
                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: timerModel
                    delegate : Kirigami.AbstractListItem {
                        id: listItem

                        RowLayout {
                            anchors.fill: listItem
                            spacing: Kirigami.Units.smallSpacing*2
                            Kirigami.Label {
                                id: iconItem
                                Layout.minimumHeight: Kirigami.Units.iconSizes.smallMedium
                                Layout.maximumHeight: Layout.minimumHeight
                                Layout.minimumWidth: height
                                text: model.job.delay
                            }

                            Kirigami.Label {
                                id: labelItem
                                Layout.fillWidth: true
                                color: listItem.checked || listItem.pressed ? listItem.activeTextColor : listItem.textColor
                                elide: Text.ElideRight
                                font: listItem.font
                                text: model.job.value
                            }

                            QC1.Button {
                                iconName : "arrow-right"
                                onClicked : {
                                    model.job.start();
                                }
                            }
                            QC1.Button {
                                iconName : "media-playback-pause"
                                onClicked : {
                                    model.job.pause();
                                }
                            }
                            QC1.Button {
                                iconName : "media-playback-stop"
                                onClicked : {
                                    model.job.stop();
                                }
                            }
                        }
                    }
                }
                RowLayout {
                    QC2.Button {
                        text: "+"
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
    }
}
