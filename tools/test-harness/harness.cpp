#include "wh_stub.h"
#include MOD_SOURCE

std::wstring g_theme = L"auto";
std::map<std::wstring, std::wstring>& StubSettings() {
    static std::map<std::wstring, std::wstring> m = {
        {L"theme", g_theme}, {L"showAllDisplaysSlider", L"1"}, {L"showOsd", L"1"},
        {L"wheel.mode", L"off"}, {L"wheel.step", L"2"}, {L"wheel.target", L"all"},
        {L"hotkeys[0].keys", L"Ctrl+Alt+F23"}, {L"hotkeys[0].action", L"increase"},
        {L"hotkeys[0].value", L"0"},
        {L"scheduleOptions.transitionSeconds", L"30"}, {L"scheduleOptions.applyOnStartup", L"1"},
        {L"idle.minutes", L"5"}, {L"idle.brightness", L"10"},
    };
    return m;
}

bool SavePng(HBITMAP bmp, PCWSTR path) {
    UINT num = 0, size = 0;
    Gdiplus::GetImageEncodersSize(&num, &size);
    std::vector<BYTE> buf(size);
    auto* enc = (Gdiplus::ImageCodecInfo*)buf.data();
    Gdiplus::GetImageEncoders(num, size, enc);
    for (UINT i = 0; i < num; i++) {
        if (wcscmp(enc[i].MimeType, L"image/png") == 0) {
            Gdiplus::Bitmap b(bmp, nullptr);
            return b.Save(path, &enc[i].Clsid, nullptr) == Gdiplus::Ok;
        }
    }
    return false;
}

bool CaptureScreen(RECT r, PCWSTR path) {
    int w = r.right - r.left, h = r.bottom - r.top;
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateCompatibleBitmap(screen, w, h);
    SelectObject(mem, bmp);
    BitBlt(mem, 0, 0, w, h, screen, r.left, r.top, SRCCOPY | CAPTUREBLT);
    bool ok = SavePng(bmp, path);
    DeleteDC(mem);
    DeleteObject(bmp);
    ReleaseDC(nullptr, screen);
    return ok;
}

int wmain(int argc, wchar_t** argv) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    if (argc > 1) g_theme = argv[1];
    std::wstring out = argc > 2 ? argv[2] : L"panel";
    g_trayUseGuid = false;  // don't bind the tray GUID to this test exe
    WhTool_ModInit();
    Sleep(5000);

    PostMessage(g_hwndMain, WM_APP_TRAY, 0, NIN_SELECT);
    Sleep(2000);
    RECT r;
    GetWindowRect(g_hwndPanel, &r);
    wprintf(L"panel visible=%d size=%dx%d\n", IsWindowVisible(g_hwndPanel),
            (int)(r.right - r.left), (int)(r.bottom - r.top));
    {
        int w = r.right - r.left, h = r.bottom - r.top;
        HDC screen = GetDC(nullptr);
        HDC mem = CreateCompatibleDC(screen);
        HBITMAP bmp = CreateCompatibleBitmap(screen, w, h);
        SelectObject(mem, bmp);
        PrintWindow(g_hwndPanel, mem, 2);
        wprintf(L"panel saved=%d\n", SavePng(bmp, (out + L"-panel.png").c_str()));
        DeleteDC(mem);
        DeleteObject(bmp);
        ReleaseDC(nullptr, screen);
    }
    ShowWindow(g_hwndPanel, SW_HIDE);

    PostMessage(g_hwndMain, WM_HOTKEY, 1, 0);
    Sleep(500);
    GetWindowRect(g_hwndOsd, &r);
    wprintf(L"osd visible=%d size=%dx%d\n", IsWindowVisible(g_hwndOsd),
            (int)(r.right - r.left), (int)(r.bottom - r.top));
    {
        UINT dpi = GetMonitorDpi(MonitorFromRect(&r, MONITOR_DEFAULTTOPRIMARY));
        Gdiplus::Bitmap canvas(r.right - r.left, r.bottom - r.top, PixelFormat32bppARGB);
        PaintOsd(canvas, dpi, g_displays[0].label, g_displays[0].value);
        Gdiplus::Bitmap framed(canvas.GetWidth() + 32, canvas.GetHeight() + 32, PixelFormat32bppARGB);
        {
            Gdiplus::Graphics g(&framed);
            g.Clear(Gdiplus::Color(255, 90, 110, 140));
            g.DrawImage(&canvas, 16, 16);
        }
        UINT num = 0, size = 0;
        Gdiplus::GetImageEncodersSize(&num, &size);
        std::vector<BYTE> buf(size);
        auto* enc = (Gdiplus::ImageCodecInfo*)buf.data();
        Gdiplus::GetImageEncoders(num, size, enc);
        for (UINT i = 0; i < num; i++)
            if (wcscmp(enc[i].MimeType, L"image/png") == 0)
                wprintf(L"osd saved=%d\n", framed.Save((out + L"-osd.png").c_str(), &enc[i].Clsid, nullptr) == Gdiplus::Ok);
    }

    WhTool_ModUninit();
    wprintf(L"exit ok\n");
    return 0;
}
