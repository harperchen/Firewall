import QtQuick 2.0
import QtQuick.Controls 1.4

TableView{
    id:natTable
    frameVisible: true
    anchors.topMargin: 10
    model: natTableModel

    TableViewColumn{
        id: before_ip
        role: "Before_IP"
        title: "Before Nat IP"
        width: 140
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id: before_port
        role: "Before_Port"
        title: "Before Port"
        width: 100
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id:after_ip
        role : "After_IP"
        title: "After Nat IP"
        width: 140
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id:after_port
        role : "After_Port"
        title: "After Port"
        width: 100
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id: type
        role : "Type"
        title: "Type"
        width: natTable.viewport.width - before_ip.width - before_port.width - after_ip.width - after_port.width
        horizontalAlignment: Text.AlignHCenter
    }

    onClicked: {
        natTableModel.selected_row = row;
    }
}

