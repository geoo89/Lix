#include <sstream>

#include "bit_brow.h"

#include "../api/button/b_bitmap.h" // flip button >>
#include "../api/button/b_combin.h"
#include "../graphic/gra_lib.h"
#include "../level/obj_lib.h"
#include "../other/hardware.h"
#include "../other/help.h"
#include "../other/language.h"
#include "../other/user.h"

namespace Api {

const int BitmapBrowser::this_yl             (420);
const int BitmapBrowser::frame_offset         (20);

const int BitmapBrowser::dir_list_xl         (100);
const int BitmapBrowser::dir_list_yl         (300);
const int BitmapBrowser::dir_list_entry_yl    (20);

const int BitmapBrowser::file_list_xl        (480);
const int BitmapBrowser::file_list_yl        (360);
const int BitmapBrowser::file_list_entry_space(10);
const int BitmapBrowser::file_list_entry_xl   (60);
const int BitmapBrowser::file_list_entry_yl   (60);

BitmapBrowser::BitmapBrowser(
    ListFile::SearchCrit crit,
    const Filename&      cdir,
    const std::string&   tit,
    const int            initial_page
) :
    // Fenster selber als einziges auf das OSD, alles andere auf das Fenster
    Window(0, 0, frame_offset*3 + dir_list_xl + file_list_xl,
     this_yl, tit),

    dir_list(frame_offset, frame_offset + 20,
     dir_list_xl, dir_list_yl, gloB->dir_bitmap, gloB->dir_bitmap),

    list_bitmap(2*frame_offset + dir_list_xl,
     frame_offset + 20, file_list_xl, file_list_yl, crit),

    cancel(frame_offset, 3*frame_offset + dir_list_yl,
     dir_list_xl, 40),

    return_object(0)
{
    add_child(dir_list);
    add_child(list_bitmap);
    add_child(cancel);

    cancel.set_text  (Language::common_cancel);
    cancel.set_hotkey(useR->key_me_exit);

    load_dir(cdir, initial_page);
}



BitmapBrowser::~BitmapBrowser()
{
}



void BitmapBrowser::load_dir(const Filename& s, const int initial_page)
{
    dir_list   .set_current_dir(s);
    list_bitmap.load_dir(s, initial_page);
    set_subtitle(dir_list.get_current_dir().get_dir_rootless());
}



void BitmapBrowser::calc_self()
{
    // Verzeichnis wechseln
    if (dir_list.get_clicked()) {
        list_bitmap.load_dir(dir_list.get_current_dir());
        set_subtitle(dir_list.get_current_dir().get_dir_rootless());
    }
    // always exit with RMB, never use this to change the directory
    else if (hardware.get_mr()) {
        set_exit();
    }
    // Dateibuttons angeklickt?
    else if (list_bitmap.get_clicked()) {
        return_object = ObjLib::get(list_bitmap.get_current_file());
        set_exit();
    }
    else if (cancel.get_clicked()
     || (hardware.get_mr() && !this->is_mouse_here())) {
        set_exit();
    }
}



// ############################################################################
// ########################################################## Klasse ListBitmap
// ############################################################################



const int ListBitmap::xl     = BitmapBrowser::file_list_entry_xl;
const int ListBitmap::yl     = BitmapBrowser::file_list_entry_yl;
const int ListBitmap::in_row = BitmapBrowser::file_list_xl / xl;

ListBitmap::ListBitmap(
    const int x,  const int y,
    const int xl, const int yl,
    const ListFile::SearchCrit crit
) :
    ListFile(x, y, xl, yl, crit)
{
    set_bottom_button(
      BitmapBrowser::file_list_xl/BitmapBrowser::file_list_entry_xl
     *BitmapBrowser::file_list_yl/BitmapBrowser::file_list_entry_yl
     - 1);
}



ListBitmap::~ListBitmap()
{
}



void ListBitmap::on_dir_load()
{
    // Is there an original graphics set to show?
    if (get_current_dir() .get_dir_rootful().find(
     gloB->dir_bitmap_orig.get_dir_rootful()) != std::string::npos)
     for (int set = ObjLib::DIRT; set != ObjLib::MAX; ++set) {
        const std::string& set_name = ObjLib::orig_set_to_string(set);
        if (get_current_dir().get_dir_rootful().find(set_name)
         != std::string::npos) {
            std::string base = set_name;
            base += '-';
            // Add all objects from the graphics set
            int nr = 0;
            const Object* ob;
            for (nr = 0; (ob = ObjLib::get_orig_terrain(set, nr)) != 0; ++nr) {
                std::ostringstream filename;
                filename << base << 't' << nr;
                Filename fn(filename.str());
                if (get_search_criterion()(fn)) put_to_file_list(fn);
            }
            for (nr = 0; (ob = ObjLib::get_orig_special(set, nr)) != 0; ++nr) {
                std::ostringstream filename;
                filename << base << 's' << nr;
                Filename fn(filename.str());
                if (get_search_criterion()(fn)) put_to_file_list(fn);
            }
            // Only the display of one graphics set is required
            break;
        }
    }
}

void ListBitmap::add_file_button(const int nr, const int which_from_file)
{
    CombinedButton* b = new CombinedButton(
     xl * (nr % in_row),
     yl * (nr / in_row), xl, yl);

    const Filename& fn = get_file(which_from_file);

    // Original graphics sets' objects don't need a directory
    if (get_current_dir() .get_dir_rootful().find(
     gloB->dir_bitmap_orig.get_dir_rootful()) != std::string::npos)
    {
        b->set_object(ObjLib::get(fn));
        // Draw the entire number to the
        const std::string& str = fn.get_rootful();
        int pos = str.size() - 1;
        while (pos > 0 && str[pos] >= '0' && str[pos] <= '9') --pos;
        b->set_text(str.substr(pos + 1));
    }
    else {
        b->set_object(ObjLib::get(fn));
        b->set_text(fn.get_file_no_ext_no_pre_ext());
    }
    button_push_back(b);
}



void ListBitmap::add_flip_button()
{
    BitmapButton* b = new BitmapButton(
     GraLib::get(gloB->file_bitmap_edit_flip),
     xl * (get_bottom_button() % in_row),
     yl * (get_bottom_button() / in_row));
    button_push_back(b);
}

}
// Ende von namespace Api
