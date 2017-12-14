import QtQuick 2.3

Item
{
    id: player_tank
    focus: true
    x: modelData.pos_x
    y: modelData.pos_y
    width: modelData.width
    height: modelData.height

    Image
    {
        id: player_tank_image
        anchors.centerIn: parent
        source: modelData.image_path
        rotation: modelData.rotation
        visible: modelData.visible
    }

    Keys.onPressed:
    {
        if( event.key === Qt.Key_Left )
        {
            modelData.move_direction = "Left"
            event.accepted = true;
        }
        if( event.key === Qt.Key_Right )
        {
            modelData.move_direction = "Right"
            event.accepted = true;
        }
        else if( event.key === Qt.Key_Up )
        {
            modelData.move_direction = "Up"
            event.accepted = true;
        }
        else if( event.key === Qt.Key_Down )
        {
            modelData.move_direction = "Down"
            event.accepted = true;
        }
        else if( event.key === Qt.Key_Space )
        {
            modelData.fired = true
            event.accepted = true;
        }
    }

    Keys.onReleased:
    {
        if( event.key === Qt.Key_Left ||
            event.key === Qt.Key_Right ||
            event.key === Qt.Key_Up ||
            event.key === Qt.Key_Down )
        {
            modelData.move_direction = "None"
            event.accepted = true;
        }
    }
}
