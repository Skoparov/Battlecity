import QtQuick 2.5
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.4

Item
{
    id: side_bar
    width: Math.max( enemies_text_metrics.tightBoundingRect.width, map_interface.frag_width * 2 ) + 10

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
            text: "Foes"
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

        // Lifes

        Text
        {
            id: lifes
            anchors.top : frag_count_grid.bottom
            anchors.topMargin: 5
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
            anchors.leftMargin: 7;
            font { family: "Courier"; pointSize: 15; weight: Font.Bold }
            color: "white"
            text: map_interface.player_remaining_lifes
        }

        Image
        {
            id : base_lifes_image
            width: 32; height: 32
            source: "qrc:/graphics/player_base.png"
            anchors.top : lifes_count.bottom
            anchors.topMargin: 5
        }

        Text
        {
            id: base_lifes_count
            smooth: true
            anchors.verticalCenter : base_lifes_image.verticalCenter
            anchors.left : base_lifes_image.right
            anchors.leftMargin: 7;
            font { family: "Courier"; pointSize: 15; weight: Font.Bold }
            color: "white"
            text: map_interface.base_remaining_health
        }

        Button
        {
            id: pause_resume_button
            anchors.topMargin: 12;
            anchors.top : base_lifes_count.bottom
            anchors.horizontalCenter: enemies_text
            text: map_interface.pause_play_button_text
            visible: map_interface.pause_play_button_visible

            style: ButtonStyle
            {
                background: Rectangle
                {
                    border.color: "#333333"
                    radius: 0

                    gradient: Gradient
                    {
                        GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
                        GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
                    }
                }
            }

            onClicked:
            {
                map_interface.pause_resume()
            }
        }
    }
}

