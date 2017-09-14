import QtQuick 2.1
import org.kde.kirigami 2.1 as Kirigami
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as QC2
import QtQuick.Controls 1.4 as QC1
import kde.ktimer.components 1.0

Kirigami.AbstractListItem {
    id: listItem
    property QtObject jobModel;

    RowLayout {
        //anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing*4

        Kirigami.Label {
            id: labelItem
            verticalAlignment: Text.AlignVCenter
            Layout.fillWidth: true
            color: listItem.checked || listItem.pressed ? listItem.activeTextColor : listItem.textColor
            elide: Text.ElideRight
            font: listItem.font
            text: model.job.formattedValue
        }

        RowLayout {
            //use opacity instead of visible to *not* relayout when things get visible/invisible
            opacity: listItem.hovered || Kirigami.Settings.isMobile ? 1 : 0
            QC1.Button {
                iconName : model.job.state == TimerJob.Started ? "media-playback-pause" : "arrow-right"
                onClicked : {
                    if (model.job.state == TimerJob.Started) {
                        job.pause();
                    } else {
                        job.start();
                    }
                }
            }
            QC1.Button {
                iconName : "media-playback-stop"
                onClicked : {
                    job.stop();
                }
            }
            QC1.Button {
                checkable: true
                iconName : "media-playlist-repeat"
                onClicked : {
                    job.loop = checked
                }
            }
            /* TODO: Play when done
            QC1.Button {
                checkable: true
                iconName : "audio-volume-high"
                onClicked : {

                }
            } */
            QC1.Button {
                iconName : "user-trash"
                onClicked : {
                    jobModel.removeJob(index);
                }
            }
        }
    }
}
