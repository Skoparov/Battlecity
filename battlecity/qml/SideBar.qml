import QtQuick 2.3

Item
{
    id: side_bar

    Item
    {
        id: side_bar_frags
        anchors.top: parent.top
        anchors.left: parent.left

        Grid
        {
            id: frag_count_grid
            columns: 2
            rows: map_interface.remaining_frags_num / 2
            spacing: 0

            Repeater
            {
                model: map_interface.remaining_frags
                Frag{}
            }
        }
    }
}

