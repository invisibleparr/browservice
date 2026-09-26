#pragma once

#include "rect.hpp"
#include "widget.hpp"

class CefBrowser;
class CefRenderHandler;

namespace browservice {

class BrowserAreaEventHandler {
public:
    virtual void onBrowserAreaViewDirty() = 0;

    // Called when the text input mode of the currently focused text field in
    // the browser changes. The argument is a cef_text_input_mode_t value cast
    // to int; CEF_TEXT_INPUT_MODE_NONE means that no text field is focused.
    virtual void onBrowserAreaTextInputModeChanged(int mode) = 0;
};

class TextLayout;

// BrowserArea is a special widget in the sense that it renders continuously
// (outside render() calls) and does not notify of updates using
// WidgetParent::onWidgetViewDirty; instead, it calls
// BrowserAreaEventHandler::onBrowserAreaViewDirty. This is to avoid redrawing
// the rest of the UI every time the browser area updates.
class BrowserArea :
    public Widget,
    public enable_shared_from_this<BrowserArea>
{
SHARED_ONLY_CLASS(BrowserArea);
public:
    BrowserArea(CKey,
        weak_ptr<WidgetParent> widgetParent,
        weak_ptr<BrowserAreaEventHandler> eventHandler
    );
    ~BrowserArea();

    // Creates a new CefRenderHandler than retains a pointer to this BrowserArea
    // and paints the browser contents to the viewport
    CefRefPtr<CefRenderHandler> createCefRenderHandler();

    // Sets the browser that will be kept up to date about size changes of
    // this widget. The browser can be unset by passing a null pointer.
    void setBrowser(CefRefPtr<CefBrowser> browser);

    // Inform the browser again about focus and mouseover status. Should be
    // called when loading a new page
    void refreshStatusEvents();

    // After calling showError and before clearError, the browser area switches
    // to a special mode in which it only shows the given error message.
    void showError(string message);
    void clearError();

    // Notify the browser area that the browser has changed the cursor type.
    void setCursor(int cursor);

private:
    class RenderHandler;

    virtual void widgetViewportUpdated_() override;

    virtual void widgetMouseDownEvent_(int x, int y, int button) override;
    virtual void widgetMouseUpEvent_(int x, int y, int button) override;
    virtual void widgetMouseDoubleClickEvent_(int x, int y) override;
    virtual void widgetMouseWheelEvent_(int x, int y, int delta) override;
    virtual void widgetMouseMoveEvent_(int x, int y) override;
    virtual void widgetMouseEnterEvent_(int x, int y) override;
    virtual void widgetMouseLeaveEvent_(int x, int y) override;
    virtual void widgetKeyDownEvent_(int key) override;
    virtual void widgetKeyUpEvent_(int key) override;
    virtual void widgetGainFocusEvent_(int x, int y) override;
    virtual void widgetLoseFocusEvent_() override;

    virtual void widgetTouchBeginEvent_(int x, int y, int id) override;
    virtual void widgetTouchUpdateEvent_(int x, int y, int id) override;
    virtual void widgetTouchEndEvent_(int x, int y, int id, bool cancelled) override;

    weak_ptr<BrowserAreaEventHandler> eventHandler_;
    CefRefPtr<CefBrowser> browser_;

    bool popupOpen_;
    Rect popupRect_;

    uint32_t eventModifiers_;

    bool errorActive_;
    shared_ptr<TextLayout> errorLayout_;
};

}
