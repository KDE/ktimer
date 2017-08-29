import QtQuick 2.1
import org.kde.kirigami 2.1 as Kirigami
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2 as QC2
import QtQuick.Controls 1.4 as QC1
import kde.ktimer.components 1.0

Kirigami.AbstractListItem {
    id: listItem
    property QtObject job;

    RowLayout {
        anchors.fill: listItem
        spacing: Kirigami.Units.smallSpacing*2
        Kirigami.Label {
            id: iconItem
            Layout.minimumHeight: Kirigami.Units.iconSizes.smallMedium
            Layout.maximumHeight: Layout.minimumHeight
            Layout.minimumWidth: height
            text: job.delay
        }

        Kirigami.Label {
            id: labelItem
            Layout.fillWidth: true
            color: listItem.checked || listItem.pressed ? listItem.activeTextColor : listItem.textColor
            elide: Text.ElideRight
            font: listItem.font
            text: job.value
        }

        QC1.Button {
            iconName : "arrow-right"
            onClicked : {
                job.start();
            }
            visible : job.state != TimerJob.Started
        }
        QC1.Button {
            iconName : "media-playback-pause"
            onClicked : {
                job.pause();
            }
            visible : job.state == TimerJob.Started
        }
        QC1.Button {
            iconName : "media-playback-stop"
            onClicked : {
                job.stop();
            }
        }
    }
}
