import QtQuick 2.1
import org.kde.kirigami 2.1 as Kirigami
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as QC2
import QtQuick.Controls 1.4 as QC1
import kde.ktimer.components 1.0

Kirigami.SwipeListItem {
    id: listItem
    property QtObject jobModel;

    RowLayout {
        //anchors.fill: parent
        spacing: Kirigami.Units.smallSpacing*4

        Kirigami.Label {
            id: labelItem
            verticalAlignment: Text.AlignVCenter
            color: listItem.checked || listItem.pressed ? listItem.activeTextColor : listItem.textColor
            elide: Text.ElideRight
            font: listItem.font
            text: model.job.formattedValue
        }
        Column {
            Layout.fillWidth: true
            Kirigami.Label {
                id: nameItem
                color: listItem.checked || listItem.pressed ? listItem.activeTextColor : listItem.textColor
                elide: Text.ElideRight
                font: listItem.font
                text: model.job.name
            }
            Kirigami.Label {
                id: commandItem
                color: listItem.checked || listItem.pressed ? listItem.activeTextColor : listItem.textColor
                elide: Text.ElideRight
                font: listItem.font
                text: model.job.command ? "Run " + model.job.command : ""
            }
        }
    }
    actions:[
        Kirigami.Action {
            id:m_start
            iconName: model.job.state == TimerJob.Started ? "media-playback-pause" : "arrow-right"
            onTriggered: {
                if (model.job.state == TimerJob.Started) {
                    job.pause();
                } else {
                    job.start();
                }
            }
        },
        Kirigami.Action {
            id:m_stop
            iconName: "media-playback-stop"
            tooltip: "Stop a countdown"
            onTriggered: job.stop();
        },
        Kirigami.Action {
            iconName: "media-playlist-repeat"
            onTriggered: job.loop = checked
        },
        Kirigami.Action {
            iconName: "edit-delete"
            onTriggered: jobModel.removeJob(index);
        }
    ]
}
