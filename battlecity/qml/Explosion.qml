import QtQuick 2.0

Item
{
    id: explosion
    x: modelData.pos_x
    y: modelData.pos_y
    width: modelData.width
    height: modelData.height

    AnimatedSprite
    {
        id: explosion_sprite
        anchors.centerIn: parent
        width: modelData.width
        height: modelData.height
        source: modelData.image_path
        frameCount: 3
        frameRate: 6
        loops: 1
    }

    NumberAnimation on rotation
    {
        to: 0
        duration: 200

        onRunningChanged:
        {
            if( !running )
            {
                map_interface.animation_ended( modelData.object_id )
            }
        }
    }
}


