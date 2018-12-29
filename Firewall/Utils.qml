import QtQuick 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

ColumnLayout{

    property var str
    property var switchs

    Dialog {
        id: isNull
        title:"Alert"
        width: 300
        height: 100
        Label {
            text: "<b>WARNING</b> : " + str + " is NULL !"
            font.pixelSize: 13
            anchors.centerIn: parent
        }
        visible:switchs === 1? true: false
        standardButtons: StandardButton.Ok
    }

    Dialog {
        id: isInvalid
        title:"Alert"
        width: 300
        height: 100
        Label {
            text: "<b>WARNING</b> : " + str + " is Invalid !"
            font.pixelSize: 13
            anchors.centerIn: parent
        }
        visible:switchs === 2? true: false
        standardButtons: StandardButton.Ok
    }
}
