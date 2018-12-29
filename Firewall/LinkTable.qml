import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

TableView{
    id:linkTable
    frameVisible: true
    sortIndicatorVisible: true

    model:linkTableModel

    onSortIndicatorOrderChanged: {
        linkTableModel.sort(sortIndicatorColumn, sortIndicatorOrder);
    }

    onSortIndicatorColumnChanged: {
        linkTableModel.sort(sortIndicatorColumn, sortIndicatorOrder);
    }

    TableViewColumn{
        id: source
        role: "Source"
        title: "Source"
        width: linkTable.viewport.width / 6
        horizontalAlignment: Text.AlignHCenter
    }


    TableViewColumn{
        id:dest
        role : "Dest"
        title: "Destination"
        width: linkTable.viewport.width / 6
        horizontalAlignment: Text.AlignHCenter
    }


    TableViewColumn{
        id: protocol
        role: "Protocol"
        title: "Prot"
        width: linkTable.viewport.width / 12
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id:timeout
        role:"Timeout"
        title:"Time"
        width: linkTable.viewport.width / 10
        horizontalAlignment: Text.AlignHCenter
    }

    TableViewColumn{
        id:info
        role:"Info"
        title:"Info"
        width:  linkTable.viewport.width - source.width - dest.width - protocol.width - timeout.width
        horizontalAlignment: Text.AlignHCenter
    }
}
