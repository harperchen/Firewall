import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import "utils.js" as Funcs

RowLayout{
    anchors.left: parent.left
    anchors.right: parent.right
    spacing: 10

    Utils{
        id:dialogs
        visible: false
    }

    ColumnLayout{
        anchors.left: parent.left
        spacing: 10
        RowLayout{
            anchors.left: parent.left
            Label{
                text: "Before Nat : "
            }

            TextField{
                id:before_ip
                placeholderText: "Before Nat IP"
                implicitWidth: 115
            }

            Label{
                text:"/"
            }

            TextField{
                id:before_mask
                placeholderText: "Mask"
                implicitWidth: 45
                text: static_nat.checked ? "N/A" : ""
                enabled: static_nat.checked ? false : true
            }
            Label{
                text:":"
            }

            TextField{
                id:before_port
                placeholderText: "Port"
                implicitWidth: 50
                text: "ANY"
                enabled: false
            }
        }

        RowLayout{
            anchors.left: parent.left
            anchors.right: parent.right

            RowLayout{
                anchors.left: parent.left

                Label{
                    text: "After Nat : "
                }

                TextField{
                    id:after_ip
                    Layout.leftMargin: 10
                    placeholderText: "After Nat IP"
                    implicitWidth: 115
                }

                Label{
                    text:"/"
                }

                TextField{
                    id:after_mask
                    placeholderText: "Mask"
                    implicitWidth: 45
                    text: static_nat.checked || pat.checked ? "N/A" : ""
                    enabled: static_nat.checked || pat.checked ? false : true
                }
                Label{
                    text:":"
                }

                TextField{
                    id:after_port
                    placeholderText: "Port"
                    implicitWidth: 50
                    text: static_nat.checked || dynamic_nat.checked ? "ANY" : ""
                    enabled: static_nat.checked || dynamic_nat.checked ? false : true
                }
            }
        }
    }

    Label{
        Layout.leftMargin: 10
        text:"Nat Type :"
    }

    GroupBox{
        ColumnLayout {
            spacing: 15
            ExclusiveGroup {
                id: nat_type
            }
            RowLayout{
                RadioButton {
                    id:static_nat
                    text: "Static Nat"
                    checked: true
                    exclusiveGroup: nat_type
                }
                RadioButton {
                    id:pat
                    text: "PAT"
                    exclusiveGroup: nat_type
                }
            }
            RadioButton {
                id:dynamic_nat
                text: "Dynamic Nat"
                exclusiveGroup: nat_type
            }
        }
    }
    ColumnLayout{
        anchors.right: parent.right
        spacing: 10
        Button{
            id: add
            text: "Add"
            implicitWidth: 70
            onClicked: {
                var info;
                info = Funcs.check("before", "ip", before_ip.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }
                info = Funcs.check("before", "mask", before_mask.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }
                info = Funcs.check("before", "port", before_port.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }

                info = Funcs.check("after", "ip", after_ip.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }

                info = Funcs.check("after", "mask", after_mask.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }

                info = Funcs.check("after", "port", after_port.text);
                if(info){
                    dialogs.switchs = info[0];  dialogs.str = info[1];
                    return;
                }

                if(static_nat.checked){
                    qnat.nat_type = "Static Nat";
                }else if(dynamic_nat.checked){
                    qnat.nat_type = "Dynamic Nat";
                }else{
                    qnat.nat_type = "PAT";
                }

                natTableModel.addNatItem(qnat);

            }
        }
        Button{
            id:remove
            text:"Delete"
            implicitWidth: 70

            onClicked: {
                natTableModel.delNatItem();

            }
        }
    }

}
