#ifndef slic3r_GUI_wxExtensions_hpp_
#define slic3r_GUI_wxExtensions_hpp_

#include <wx/checklst.h>
#include <wx/combo.h>
#include <wx/dataview.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/bmpcbox.h>
#include <wx/statbmp.h>
#include <wx/popupwin.h>
#include <wx/spinctrl.h>
#include <wx/artprov.h>

#include <vector>
#include <functional>

#include "Widgets/PopupWindow.hpp"

#ifndef __linux__
void sys_color_changed_menu(wxMenu* menu);
#else
inline void sys_color_changed_menu(wxMenu* /* menu */) {}
#endif // no __linux__

[[deprecated("Function renamed to sys_color_changed_menu. This function now redirects to sys_color_changed_menu")]]
inline void msw_rescale_menu(wxMenu* menu) { sys_color_changed_menu(menu); }

wxMenuItem* append_menu_item(wxMenu* menu, int id, const wxString& string, const wxString& description,
    std::function<void(wxCommandEvent& event)> cb, wxBitmapBundle* icon, wxEvtHandler* event_handler = nullptr,
    std::function<bool()> const cb_condition = []() { return true;}, wxWindow* parent = nullptr, int insert_pos = wxNOT_FOUND);
wxMenuItem* append_menu_item(wxMenu* menu, int id, const wxString& string, const wxString& description,
    std::function<void(wxCommandEvent& event)> cb, const std::string& icon = "", wxEvtHandler* event_handler = nullptr,
    std::function<bool()> const cb_condition = []() { return true; }, wxWindow* parent = nullptr, int insert_pos = wxNOT_FOUND);

wxMenuItem* append_submenu(wxMenu* menu, wxMenu* sub_menu, int id, const wxString& string, const wxString& description,
    const std::string& icon = "",
    std::function<bool()> const cb_condition = []() { return true; }, wxWindow* parent = nullptr);

wxMenuItem* append_menu_radio_item(wxMenu* menu, int id, const wxString& string, const wxString& description,
    std::function<void(wxCommandEvent& event)> cb, wxEvtHandler* event_handler);

wxMenuItem* append_menu_check_item(wxMenu* menu, int id, const wxString& string, const wxString& description,
    std::function<void(wxCommandEvent & event)> cb, wxEvtHandler* event_handler,
    std::function<bool()> const enable_condition = []() { return true; }, 
    std::function<bool()> const check_condition = []() { return true; }, wxWindow* parent = nullptr);

void enable_menu_item(wxUpdateUIEvent& evt, std::function<bool()> const cb_condition, wxMenuItem* item, wxWindow* win);

class wxDialog;

void    edit_tooltip(wxString& tooltip);
void    msw_buttons_rescale(wxDialog* dlg, const int em_unit, const std::vector<int>& btn_ids);
int     em_unit(wxWindow* win);
int     mode_icon_px_size();

wxBitmapBundle* get_bmp_bundle(const std::string& bmp_name, int px_cnt = 16);
wxBitmapBundle* get_empty_bmp_bundle(int width, int height);
wxBitmapBundle* get_solid_bmp_bundle(int width, int height, const std::string& color);

// BBS: support resize by fill border
#if 1
wxBitmap create_scaled_bitmap(const std::string& bmp_name, wxWindow *win = nullptr, 
    const int px_cnt = 16, const bool grayscale = false,
    const std::string& new_color = std::string(), // color witch will used instead of orange
    const bool menu_bitmap = false, const bool resize = false);
#else
wxBitmap create_scaled_bitmap(const std::string& bmp_name, wxWindow *win = nullptr, 
    const int px_cnt = 16, const bool grayscale = false, const bool resize = false);
#endif

wxBitmap* get_default_extruder_color_icon(bool thin_icon = false);
std::vector<wxBitmapBundle *> get_extruder_color_icons(bool thin_icon = false);
wxBitmapBundle * get_extruder_color_icon(std::string color, std::string label, int icon_width, int icon_height);

namespace Slic3r {
namespace GUI {
class BitmapComboBox;
}
}
void apply_extruder_selector(Slic3r::GUI::BitmapComboBox** ctrl,
                             wxWindow* parent,
                             const std::string& first_item = "",
                             wxPoint pos = wxDefaultPosition,
                             wxSize size = wxDefaultSize,
                             bool use_thin_icon = false);

class wxCheckListBoxComboPopup : public wxCheckListBox, public wxComboPopup
{
    static const unsigned int DefaultWidth;
    static const unsigned int DefaultHeight;

    wxString m_text;

    // Events sent on mouseclick are quite complex. Function OnListBoxSelection is supposed to pass the event to the checkbox, which works fine on
    // Win. On OSX and Linux the events are generated differently - clicking on the checkbox square generates the event twice (and the square
    // therefore seems not to respond).
    // This enum is meant to save current state of affairs, i.e., if the event forwarding is ok to do or not. It is only used on Linux
    // and OSX by some #ifdefs. It also stores information whether OnListBoxSelection is supposed to change the checkbox status,
    // or if it changed status on its own already (which happens when the square is clicked). More comments in OnCheckListBox(...)
    // There indeed is a better solution, maybe making a custom event used for the event passing to distinguish the original and passed message
    // and blocking one of them on OSX and Linux. Feel free to refactor, but carefully test on all platforms.
    enum class OnCheckListBoxFunction{
        FreeToProceed,
        RefuseToProceed,
        WasRefusedLastTime
    } m_check_box_events_status = OnCheckListBoxFunction::FreeToProceed;


public:
    virtual bool Create(wxWindow* parent);
    virtual wxWindow* GetControl();
    virtual void SetStringValue(const wxString& value);
    virtual wxString GetStringValue() const;
    virtual wxSize GetAdjustedSize(int minWidth, int prefHeight, int maxHeight);

    virtual void OnKeyEvent(wxKeyEvent& evt);

    void OnCheckListBox(wxCommandEvent& evt);
    void OnListBoxSelection(wxCommandEvent& evt);
};


// ***  wxDataViewTreeCtrlComboBox  ***

class wxDataViewTreeCtrlComboPopup: public wxDataViewTreeCtrl, public wxComboPopup
{
    static const unsigned int DefaultWidth;
    static const unsigned int DefaultHeight;
    static const unsigned int DefaultItemHeight;

    wxString	m_text;
    int			m_cnt_open_items{0};

public:
    virtual bool		Create(wxWindow* parent);
    virtual wxWindow*	GetControl() { return this; }
    virtual void		SetStringValue(const wxString& value) { m_text = value; }
    virtual wxString	GetStringValue() const { return m_text; }
//	virtual wxSize		GetAdjustedSize(int minWidth, int prefHeight, int maxHeight);

    virtual void		OnKeyEvent(wxKeyEvent& evt);
    void				OnDataViewTreeCtrlSelection(wxCommandEvent& evt);
    void				SetItemsCnt(int cnt) { m_cnt_open_items = cnt; }
};

inline wxSize get_preferred_size(const wxBitmapBundle& bmp, wxWindow* parent)
{
    if (!bmp.IsOk())
        return wxSize(0, 0);
#ifdef __WIN32__
    return bmp.GetPreferredBitmapSizeFor(parent);
#else
    return bmp.GetDefaultSize();
#endif
}


// ----------------------------------------------------------------------------
// ScalableBitmap
// ----------------------------------------------------------------------------

class ScalableBitmap
{
public:
    ScalableBitmap() {};
    ScalableBitmap( wxWindow *parent,
                    const std::string& icon_name = "",
                    const int px_cnt = 16,
                    const bool grayscale = false,
                    const bool resize = false, // BBS: support resize by fill border
                    const bool use_legacy_bmp = false);

    ~ScalableBitmap() {}

    [[deprecated("Function renamed to sys_color_changed. This function now redirects to sys_color_changed")]]
    inline void msw_rescale() { sys_color_changed(); }
    void sys_color_changed();

    const wxBitmapBundle&     bmp() const { return m_bmp; }
    wxBitmapBundle&           bmp()       { return m_bmp; }
    wxBitmap                  get_bitmap() const { return m_bmp.GetBitmapFor(m_parent); }
    wxWindow*                 parent() const { return m_parent;}
    const std::string&        name() const{ return m_icon_name; }
    int                       px_cnt() const { return m_px_cnt; }

    wxSize              GetSize()   const {
#ifdef __WIN32__
        return m_bmp.GetPreferredBitmapSizeFor(m_parent);
#else
        return m_bmp.GetDefaultSize();
#endif
    }
    int                 GetWidth()  const { return GetSize().GetWidth(); }
    int                 GetHeight() const { return GetSize().GetHeight(); }

private:
    wxWindow*       m_parent{ nullptr };
    wxBitmapBundle  m_bmp = wxBitmapBundle();
    std::string     m_icon_name = "";
    int             m_px_cnt {16};
    bool            m_grayscale{ false };
    bool            m_resize{ false };
    bool            m_legacy_bmp{ false };
};


// ----------------------------------------------------------------------------
// LockButton
// ----------------------------------------------------------------------------

class LockButton : public wxButton
{
public:
    LockButton(
        wxWindow *parent,
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize);
    ~LockButton() {}

    void    OnButton(wxCommandEvent& event);

    bool    IsLocked() const                { return m_is_pushed; }
    void    SetLock(bool lock);

    // create its own Enable/Disable functions to not really disabled button because of tooltip enabling
    void    enable()                        { m_disabled = false; }
    void    disable()                       { m_disabled = true;  }

    [[deprecated("Function renamed to sys_color_changed. This function now redirects to sys_color_changed")]]
    inline void msw_rescale() { sys_color_changed(); }
    void sys_color_changed();

protected:
    void    update_button_bitmaps();

private:
    bool        m_is_pushed = false;
    bool        m_disabled = false;

    ScalableBitmap    m_bmp_lock_closed;
    ScalableBitmap    m_bmp_lock_closed_f;
    ScalableBitmap    m_bmp_lock_open;
    ScalableBitmap    m_bmp_lock_open_f;
};


// ----------------------------------------------------------------------------
// ScalableButton
// ----------------------------------------------------------------------------

class ScalableButton : public wxButton
{
public:
    ScalableButton(){}
    ScalableButton(
        wxWindow *          parent,
        wxWindowID          id,
        const std::string&  icon_name = "",
        const wxString&     label = wxEmptyString,
        const wxSize&       size = wxDefaultSize,
        const wxPoint&      pos = wxDefaultPosition,
        long                style = wxBU_EXACTFIT | wxNO_BORDER,
        int                 bmp_px_cnt = 16);

    ScalableButton(
        wxWindow *          parent,
        wxWindowID          id,
        const ScalableBitmap&  bitmap,
        const wxString&     label = wxEmptyString,
        long                style = wxBU_EXACTFIT | wxNO_BORDER);

    ~ScalableButton() {}

    void SetBitmap_(const ScalableBitmap& bmp);
    bool SetBitmap_(const std::string& bmp_name);
    void SetBitmapDisabled_(const ScalableBitmap &bmp);
    int  GetBitmapHeight();

    [[deprecated("Function renamed to sys_color_changed. This function now redirects to sys_color_changed")]]
    inline void msw_rescale() { sys_color_changed(); }
    void    sys_color_changed();

private:
    wxWindow*       m_parent { nullptr };
    std::string     m_current_icon_name;
    std::string     m_disabled_icon_name;
    int             m_width {-1}; // should be multiplied to em_unit
    int             m_height{-1}; // should be multiplied to em_unit

    // bitmap dimensions 
    int             m_px_cnt{ 16 };
    bool            m_has_border {false};
};


// ----------------------------------------------------------------------------
// ModeButton
// ----------------------------------------------------------------------------

class ModeButton : public ScalableButton
{
public:
    ModeButton(
        wxWindow*           parent,
        wxWindowID          id,
        const std::string&  icon_name = "",
        const wxString&     mode = wxEmptyString,
        const wxSize&       size = wxDefaultSize,
        const wxPoint&      pos = wxDefaultPosition);

    ModeButton(
        wxWindow*           parent,
        const wxString&     mode = wxEmptyString,
        const std::string&  icon_name = "",
        int                 px_cnt = 16);

    ~ModeButton() {}

    void Init(const wxString& mode);

    void    OnButton(wxCommandEvent& event);
    void    OnEnterBtn(wxMouseEvent& event) { focus_button(true); event.Skip(); }
    void    OnLeaveBtn(wxMouseEvent& event) { focus_button(m_is_selected); event.Skip(); }

    void    SetState(const bool state);
    bool    is_selected() { return m_is_selected; }

protected:
    void    focus_button(const bool focus);

private:
    bool        m_is_selected = false;

    wxString    m_tt_selected;
    wxString    m_tt_focused;
};



// ----------------------------------------------------------------------------
// ModeSizer
// ----------------------------------------------------------------------------

class ModeSizer : public wxFlexGridSizer
{
public:
    ModeSizer( wxWindow *parent, int hgap = 0);
    ~ModeSizer() {}

    void SetMode(const /*ConfigOptionMode*/int mode);

    void set_items_flag(int flag);
    void set_items_border(int border);

    [[deprecated("Function renamed to sys_color_changed. This function now redirects to sys_color_changed")]]
    inline void msw_rescale() { sys_color_changed(); }
    void sys_color_changed();
    const std::vector<ModeButton*>& get_btns() { return m_mode_btns; }

private:
    std::vector<ModeButton*> m_mode_btns;
    wxWindow*                m_parent {nullptr};
    double                   m_hgap_unscaled;
};



// ----------------------------------------------------------------------------
// MenuWithSeparators
// ----------------------------------------------------------------------------

class MenuWithSeparators : public wxMenu
{
public:
    MenuWithSeparators(const wxString& title, long style = 0)
        : wxMenu(title, style) {}

    MenuWithSeparators(long style = 0)
        : wxMenu(style) {}

    ~MenuWithSeparators() {}

    void DestroySeparators();
    void SetFirstSeparator();
    void SetSecondSeparator();

private:
    wxMenuItem* m_separator_frst { nullptr };    // use like separator before settings item
    wxMenuItem* m_separator_scnd { nullptr };   // use like separator between settings items
};


// ----------------------------------------------------------------------------
// BlinkingBitmap
// ----------------------------------------------------------------------------

class BlinkingBitmap : public wxStaticBitmap
{
public:
    BlinkingBitmap() {};
    BlinkingBitmap(wxWindow* parent, const std::string& icon_name = "blank_16");

    ~BlinkingBitmap() {}

    void    invalidate();
    void    activate();
    void    blink();

    const wxBitmapBundle& get_bmp() const { return bmp.bmp(); }

private:
    ScalableBitmap  bmp;
    bool            show {false};
};


// BBS add new custom widget
// ----------------------------------------------------------------------------
// ImageTransientPopup
// ----------------------------------------------------------------------------

class ImageTransientPopup : public PopupWindow
{
    public:
    ImageTransientPopup( wxWindow *parent, bool scrolled, wxBitmap bmp);
    virtual ~ImageTransientPopup();

    void SetImage(wxBitmap bmp);

    // PopupWindow virtual methods are all overridden to log them
    virtual void Popup(wxWindow *focus = NULL) wxOVERRIDE;
    virtual void OnDismiss() wxOVERRIDE;
    virtual bool ProcessLeftDown(wxMouseEvent& event) wxOVERRIDE;
    virtual bool Show( bool show = true ) wxOVERRIDE;

private:

    wxScrolledWindow *m_panel;
    wxStaticBitmap* m_image;

private:
    void OnMouse( wxMouseEvent &event );
    void OnSize( wxSizeEvent &event );
    void OnSetFocus( wxFocusEvent &event );
    void OnKillFocus( wxFocusEvent &event );

private:
    wxDECLARE_ABSTRACT_CLASS(ImageTransientPopup);
    wxDECLARE_EVENT_TABLE();
};


#endif // slic3r_GUI_wxExtensions_hpp_
