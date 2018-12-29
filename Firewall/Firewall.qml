import QtQuick 2.2
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1

Window{
    height: 600
    width: 700

    TabView {
        currentIndex: 2
        anchors.margins: 10
        anchors.fill: parent
        Tab {
            title: "Filter Table"
            ColumnLayout{
                anchors.fill: parent
                anchors.margins: 10

                FilterInput {
                    id:filterInput
                    anchors.top:parent.top
                    Layout.fillWidth: true
                }

                FilterTable {
                    id:filterTable
                    Layout.fillWidth: true
                    anchors.top: filterInput.bottom
                    anchors.bottom:parent.bottom
                }
            }

        }
        Tab {
            title: "Nat Table"
            ColumnLayout{
                anchors.fill: parent
                anchors.margins: 10

                NatInput {
                    id:natInput
                    anchors.top:parent.top
                    Layout.fillWidth: true
                }

                NatTable{
                    id:natTable
                    Layout.fillWidth: true
                    anchors.top: natInput.bottom
                    anchors.bottom:parent.bottom
                }
            }

        }
        Tab {
            title: "Log Information"
            ColumnLayout{
                anchors.fill:parent
                anchors.margins: 10

                TextField{
                    text: "Application Logs"
                    readOnly: true
                    Layout.fillWidth: true
                    anchors.top:parent.top
                    implicitHeight: 25
                    style: TextFieldStyle {
                            textColor: "black"
                            background: Rectangle {
                                border.color: "gray"
                                radius: 2
                                color:"#f5f5f5"
                            }
                        }
                }

                TextArea{
                    id:logs
                    objectName: "logs"
                    anchors.bottom: parent.bottom
                    Layout.fillWidth: true
                    implicitHeight:parent.height - 25
                }
            }

        }

        Tab {
            title: "Link Table"
            ColumnLayout{
                anchors.fill:parent
                anchors.margins: 10
                spacing: 20

                TextField {
                    id: searchBox
                    placeholderText: "Search Link..."
                    width: 200
                    anchors.right: parent.right
                    anchors.top: parent.top
                    onTextChanged: {
                        linkTableModel.filter(searchBox.text);
                    }
                }


                LinkTable{
                    id:linkTable
                    anchors.topMargin: 10
                    anchors.top: searchBox.bottom
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                }
            }

        }

        style: TabViewStyle {
               frame: Rectangle {
                   color: "#F0F3F4"
                   border.color: "#A6ACAF"
               }
           }
    }

}
