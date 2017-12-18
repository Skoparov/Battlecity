import QtQuick 2.5

Item
{
    id: side_bar
    width: Math.max( t_metrics.tightBoundingRect.width, map_interface.frag_width * 2 ) + 10

    Item
    {
        id: side_bar_frags
        anchors.top: parent.top
        x: 5

        Text
        {
            id: enemies_text
            smooth: true
            font { family: "Courier"; pointSize: 15; weight: Font.Bold }
            color: "white"
            text: "Kills"
        }

        TextMetrics
        {
            id: enemies_text_metrics
            font: enemies_text.font
            text: enemies_text.text
        }

        Grid
        {
            id: frag_count_grid
            rows: map_interface.remaining_frags_num / 2
            columns: 2
            anchors.top : enemies_text.bottom
            spacing: 0

            Repeater
            {
                model: map_interface.remaining_frags
                Frag{}
            }
        }

        Text
        {
            id: lifes
            anchors.top : frag_count_grid.bottom
            smooth: true
            font { family: "Courier"; pointSize: 15; weight: Font.Bold }
            color: "white"
            text: "Lifes"
        }

        Image
        {
            id : lifes_image
            width: 32; height: 32
            source: "qrc:/graphics/heart.png"
            anchors.top : lifes.bottom
        }

        Text
        {
            id: lifes_count
            smooth: true
            anchors.verticalCenter : lifes_image.verticalCenter
            anchors.left : lifes_image.right
            anchors.horizontalCenter : parent.horizontalCenter
            font { family: "Courier"; pointSize: 15; weight: Font.Bold }
            color: "white"
            text: map_interface.remaining_lifes
        }

        TextMetrics
        {
            id: t_metrics
            font: enemies_text.font
            text: enemies_text.text
        }
    }
}

