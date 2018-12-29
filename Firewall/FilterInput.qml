import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import "utils.js" as Funcs

ColumnLayout{
    anchors.left: parent.left
    anchors.right: parent.right
    spacing: 10

    Utils{
        id:dialogs
        visible: false
    }

    RowLayout{
        anchors.left: parent.left
        anchors.right: parent.right

        RowLayout{
            anchors.left: parent.left
            Label{
                text: "Source : "
            }

            TextField{
                id:source_ip
                implicitWidth: 115
                placeholderText: "Source IP"
            }

            Label{
                text:"/"
            }

            TextField{
                id:source_mask
                placeholderText: "Mask"
                enabled: source_ip.text === "ANY" ? false : true
                text:source_ip.text === "ANY" ? "N/A" : ""
                implicitWidth: 45
            }
            Label{
                text:":"
            }

            TextField{
                id:source_port
                placeholderText: "Port"
                enabled: protocol.currentIndex === 2 ? false: true
                text:protocol.currentIndex === 2 ? "N/A" : ""
                implicitWidth: 50
            }
        }

        RowLayout{
            anchors.right: parent.right
            Label{
                text: "Destination : "
            }

            TextField{
                id:dest_ip
                implicitWidth: 115
                placeholderText: "Dest IP"
            }

            Label{
                text:"/"
            }

            TextField{
                id:dest_mask
                placeholderText: "Mask"
                implicitWidth: 45
                enabled: dest_ip.text === "ANY" ? false : true
                text:dest_ip.text === "ANY" ? "N/A" : ""
            }
            Label{
                text:":"
            }

            TextField{
                id:dest_port
                placeholderText: "Port"
                enabled: protocol.currentIndex === 2 ? false: true
                text:protocol.currentIndex === 2 ? "N/A" : ""
                implicitWidth: 50
            }
        }
    }

    RowLayout{
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: parent.width / 35

        RowLayout{
            anchors.left: parent.left
            Label{
                text:"Prot :"
            }
            ComboBox{
                id:protocol
                model: ["TCP", "UDP", "ICMP", "ANY"]
                implicitWidth: 65
            }
        }


        RowLayout{

            Label{
                text:"Chain :"
            }
            ComboBox{
                id:chain
                model: ["INPUT", "OUTPUT", "FORWARD"]
                implicitWidth: 95
            }
        }

        RowLayout{

            Label{
                text:"Action :"
            }
            ComboBox{
                id:action
                model: ["Y", "N"]
                implicitWidth:40
            }
        }

        RowLayout{

            Label{
                text:"Log :"
            }
            ComboBox{
                id:log
                model: ["Y", "N"]
                implicitWidth:40
            }
        }

        Button{
            id: add
            text: "Add"
            implicitWidth: 70
            Layout.leftMargin: 5
            onClicked: {
                var info;
                info = Funcs.check("source", "ip", source_ip.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }
                info = Funcs.check("source", "mask", source_mask.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }
                info = Funcs.check("source", "port", source_port.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }

                info = Funcs.check("dest", "ip", dest_ip.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }

                info = Funcs.check("dest", "mask", dest_mask.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }

                info = Funcs.check("dest", "port", dest_port.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }

                qrule.protocol = protocol.currentText;
                qrule.chain = chain.currentText;
                qrule.action = action.currentText;
                qrule.log = log.currentText;

                filterTableModel.addFilterItem(qrule);
                console.log("add filter time from qml" + Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz ddd"));
            }
        }

        Button{
            id: deletes
            text: "Delete"
            implicitWidth: 70
            anchors.right: parent.right
            onClicked: {
                filterTableModel.delFilterItem();
            }
        }

    }

}

