/*
 * b_spawn.cpp
 *
 */

#include "b_spawn.h"

#include "../../other/globals.h"
#include "../../other/hardware.h"
#include "../../graphic/gra_lib.h"

namespace Api {

SpawnIntervalButton::SpawnIntervalButton(int x, int y)
:
    BitmapButton(GraLib::get(gloB->file_bitmap_game_panel_2), x, y, 34, 20),
    label(get_xl() - 13, 0),
    execute_left (false),
    execute_right(false),
    ticks_lmb_is_held_for(0)
{
    label.set_align(Label::CENTERED);
    label.set_undraw_color(0);

    add_child(label);
    set_spawnint(0);
}



SpawnIntervalButton::~SpawnIntervalButton()
{
}



void SpawnIntervalButton::set_spawnint(int i)
{
    spawnint = i;
    label.set_number(i);
    set_draw_required();
}



void SpawnIntervalButton::check_whether_to_appear_down()
{
    set_down(false);
    if (is_mouse_here())
        if (hardware.get_mlh() || hardware.get_mrh())
            set_down();
    if (hardware.key_hold(get_hotkey()))
        set_down();
}



void SpawnIntervalButton::calc_self()
{
    execute_left  = false;
    execute_right = false;

    if (get_hidden()) {
        ticks_lmb_is_held_for = 0;
        return;
    }
    // we don't use the normal Button::calc_self code, but replicate some
    // of its behavior here. Since we execute on mouse press, not on release,
    // we have to alter some parts of
    if ((get_hotkey() == KEY_ENTER && hardware.key_enter_once())
                                   || hardware.key_once(get_hotkey()))
        execute_left = true;

    if (is_mouse_here()) {
        execute_left  = execute_left  || hardware.get_ml();
        execute_right = execute_right || hardware.get_mr();
        if (hardware.get_mlh())
            ++ticks_lmb_is_held_for;
        else
            ticks_lmb_is_held_for = 0;
    }
    else {
        ticks_lmb_is_held_for = 0;
    }
    if (ticks_lmb_is_held_for > 30)
        execute_left = true;

    check_whether_to_appear_down();
}

}
// end namespace Api
