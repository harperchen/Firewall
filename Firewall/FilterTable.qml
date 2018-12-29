import QtQuick 2.0
import QtQuick.Controls 1.4

TableView{
    id:filterTable
    frameVisible: true
    anchors.topMargin: 10
    model: filterTableModel

    TableViewColumn{
        id: source_ip
        role: "Source_IP"
        title: "Source IP"
        width: 120
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id: source_port
        role: "Source_Port"
        title: "Port"
        width: 50
        horizontalAlignment: Text.AlignHCenter
    }


    TableViewColumn{
        id:dest_ip
        role : "Dest_IP"
        title: "Dest IP"
        width: 120
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id:dest_port
        role : "Dest_Port"
        title: "Port"
        width: 50
        horizontalAlignment: Text.AlignHCenter
    }


    TableViewColumn{
        id: protocol
        role: "Protocol"
        title: "Prot"
        width: 70
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id:chain
        role:"Chain"
        title:"Chain"
        width: 100
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id: log
        role : "Log"
        title: "Log"
        width: 50
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id: action
        role : "Action"
        title: "Action"
        width: filterTable.viewport.width - source_ip.width - source_port.width - dest_ip.width - dest_port.width -protocol.width - chain.width - log.width
        horizontalAlignment: Text.AlignHCenter
    }

    onClicked: {
        filterTableModel.selected_row = row;
    }
}

