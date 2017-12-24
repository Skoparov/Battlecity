import QtQuick 2.0

Item
{
    id: animation
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
        frameCount: modelData.frames_num
        frameRate: modelData.frame_rate
        loops: modelData.loops_num
    }

//    NumberAnimation on rotation
//    {
//        to: 0
//        duration: modelData.duration
//    }
}


