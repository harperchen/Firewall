import QtQuick 2.5
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

ApplicationWindow {
    id:windows
    visible: true
    title: "Ubuntu Firewall"
    minimumWidth: 350
    minimumHeight: 250
    x:300
    y:150

    property var str

    menuBar:MenuBar {
        style:MenuBarStyle{
            background: Rectangle{
                implicitHeight: 20
                color:"#EFEBE7"
            }
        }

        Menu {
            title: "Save"
            MenuItem {
                text: "Save Filter Table"
                onTriggered: {
                    savefileDialog.open();
                    str = "filter";
                }


            }
            MenuItem{
                text: "Save Nat Table"
                onTriggered: {
                    savefileDialog.open();
                    str = "nat";
                }
            }
        }

        Menu {
            title: "Open"
            MenuItem {
                text: "Open Filter Table"
                onTriggered: {
                    openfileDialog.open();
                    str = "filter";
                }


            }
            MenuItem{
                text: "Open Nat Table"
                onTriggered: {
                    openfileDialog.open();
                    str = "nat";
                }
            }
        }

        Menu{
            title: "Window"
            MenuItem {
                text: "Close Window"
                onTriggered: Qt.quit()
            }
        }

    }
    ColumnLayout{
        anchors.fill:parent
        RowLayout{
            id:labels
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            anchors.bottom: bottons.top
            anchors.top: parent.top
            Layout.fillWidth: true
            Label{
                text: "Ubuntu Firewall"
                anchors.centerIn: parent
                font.pixelSize: 22
            }

        }

        RowLayout{
            id:bottons
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            anchors.bottom: parent.bottom
            anchors.margins: 30
            spacing: 20

            Label{
                text:"Default Policy :"
                color:"red"
            }
            ComboBox{
                id:default_policy
                objectName: "defaultpolicy"
                model:["Accept", "Drop"]
            }

            Button{
                text: "Tables"
                onClicked: {
                    firewall.visible = true;
                }
            }
        }
        Firewall{
            id:firewall
        }

    }

    onClosing: {
        chrDev.closeDev();
    }

    FileDialog {
        id: openfileDialog
        title: "Please choose a configure file"
        folder: shortcuts.home
        selectFolder: false
        nameFilters: [ "Configure files (*.cfg)"]
        onAccepted: {
            if(str === "filter"){
                filterTableModel.openFilter(openfileDialog.fileUrl);
            }
            if(str === "nat"){
                natTableModel.openNat(openfileDialog.fileUrl);
            }
        }
    }

    FileDialog{
        id:savefileDialog
        title:"Save configure as"
        folder: shortcuts.home
        selectExisting: false
        nameFilters: [ "Configure files (*.cfg)"]
        selectedNameFilter: "Configure files (*.cfg)"
        onAccepted: {
            if(str === "filter"){
                filterTableModel.saveFilter(savefileDialog.fileUrl);
            }
            if(str === "nat"){
                natTableModel.saveNat(savefileDialog.fileUrl);
            }
        }
    }
}
