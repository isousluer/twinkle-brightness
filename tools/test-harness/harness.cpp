#include "wh_stub.h"
#include MOD_SOURCE

std::wstring g_theme = L"auto";
std::map<std::wstring, std::wstring>& StubStorage() {
    static std::map<std::wstring, std::wstring> m;
    return m;
}
std::map<std::wstring, std::wstring>& StubSettings() {
    static std::map<std::wstring, std::wstring> m = {
        {L"theme", g_theme}, {L"showAllDisplaysSlider", L"1"}, {L"showOsd", L"1"},
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

bool SaveOsdPng(const std::vector<OsdItem>& items, UINT dpi, const std::wstring& path) {
    int w, h;
    GetOsdSize(items.size(), dpi, &w, &h);
    Gdiplus::Bitmap canvas(w, h, PixelFormat32bppARGB);
    PaintOsd(canvas, dpi, items);
    Gdiplus::Bitmap framed(w + 32, h + 32, PixelFormat32bppARGB);
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
            return framed.Save(path.c_str(), &enc[i].Clsid, nullptr) == Gdiplus::Ok;
    return false;
}

int g_failures;
void Check(bool ok, PCWSTR what) {
    wprintf(L"%s: %s\n", ok ? L"PASS" : L"FAIL", what);
    if (!ok) g_failures++;
}

// Logic tests with fake displays. Runs before the mod starts, so nothing
// reaches real hardware; queued requests are discarded afterwards.
void RunLogicTests() {
    auto fake = [](PCWSTR key, PCWSTR label, int value) {
        UiDisplay d;
        d.info.key = key;
        d.info.method = Method::Ddc;
        d.label = label;
        d.value = value;
        return d;
    };
    g_displays = {fake(L"a", L"Left", 30), fake(L"b", L"Right", 70)};

    // Linked: the shared slider and hotkeys set all displays to
    // the same level, starting from their average.
    g_linked = true;
    PanelRow all{};
    all.display = -1;
    Check(RowValue(all) == 50, L"linked slider shows the average");
    SetRowValue(all, 55);
    Check(g_displays[0].value == 55 && g_displays[1].value == 55, L"linked slider sets 55/55");
    g_displays[0].value = 30; g_displays[1].value = 70;
    ChangeDisplays(DefaultTargets(nullptr), 10, -1, g_linked);
    Check(g_displays[0].value == 60 && g_displays[1].value == 60, L"linked step from the average -> 60/60");

    // Unlinked: only the display on the given monitor changes.
    g_linked = false;
    g_displays[0].info.monitor = (HMONITOR)1;
    g_displays[1].info.monitor = (HMONITOR)2;
    g_displays[0].value = 30; g_displays[1].value = 70;
    auto right = DefaultTargets((HMONITOR)2);
    ChangeDisplays(right, 10, -1, false);
    Check(right.size() == 1 && g_displays[0].value == 30 && g_displays[1].value == 80, L"unlinked step changes only the active display");
    Check(DefaultTargets((HMONITOR)3).empty(), L"unlinked: no display on an unknown monitor");
    g_linked = true;
    g_displays[1].value = 55;

    // OSD content.
    std::vector<UiDisplay*> targets = {&g_displays[0], &g_displays[1]};
    g_displays[0].value = 55;
    Check(BuildOsdItems(targets).size() == 1, L"OSD: equal levels -> one row");
    g_displays[1].value = 80;
    auto items = BuildOsdItems(targets);
    Check(items.size() == 2 && items[0].value == 55 && items[1].value == 80, L"OSD: different levels -> one row each");

    // On Turkish Windows, settings written with the English default names
    // keep matching, numbered ones too.
    g_strings = kStringsTr;
    DisplayInfo numbered;
    numbered.name = L"Ekran (2)";
    Check(NameMatches(numbered, L"Display (2)") && !NameMatches(numbered, L"Display (1)"), L"English default name: numbered");
    numbered.name = L"Ekranlar";
    Check(EnglishDefaultName(numbered).empty(), L"English default name: own names untouched");
    g_strings = kStringsEn;

    // Tray gauge: the average (67.5%) lights 6 of the 8 rays, rounded up.
    g_settings.trayIconLevel = true;
    Check(TrayLitRays() == 6, L"tray gauge: 55/80 -> 6 rays");
    g_settings.trayIconLevel = false;
    Check(TrayLitRays() == 8, L"tray gauge off -> all rays");
    g_settings.trayIconLevel = true;

    // Levels saved before an idle dim are restored at the next start.
    g_idleRestore = {{L"a", 30}, {L"b", 70}};
    SaveIdleRestore();
    g_idleRestore.clear();
    g_displays[0].value = g_displays[1].value = 10;
    RestoreSavedIdleLevels();
    Check(g_displays[0].value == 30 && g_displays[1].value == 70, L"interrupted idle dim restored 30/70");
    Check(StubStorage().empty(), L"saved idle levels cleared");

    // Level lists for the cycle hotkey.
    Check(ParseLevelList(L"20, 50, 100") == std::vector<int>{20, 50, 100}, L"level list: 20, 50, 100");
    Check(ParseLevelList(L" 10%;x, ,150,-5 30 4a") == std::vector<int>{10, 100, 0, 30}, L"level list: blanks/invalid skipped, clamped");
    Check(ParseLevelList(L"").empty() && ParseLevelList(L" , ;").empty(), L"level list: empty");
    Check(ParseLevelList(L"50, 50, 100, 120") == std::vector<int>{50, 100}, L"level list: repeated neighbors merged");
    Check(ParseLevelList(L"100, 50, 100") == std::vector<int>{100, 50} && ParseLevelList(L"40, 40") == std::vector<int>{40}, L"level list: last equal to first dropped");

    // The next level of a cycle.
    std::vector<int> cycle = {20, 50, 100};
    Check(NextCycleLevel(cycle, 20) == 50 && NextCycleLevel(cycle, 50) == 100, L"cycle: next entry");
    Check(NextCycleLevel(cycle, 100) == 20, L"cycle: wraps after the last entry");
    Check(NextCycleLevel(cycle, 35) == 50 && NextCycleLevel(cycle, 10) == 20, L"cycle: first entry above an unlisted level");
    Check(NextCycleLevel(cycle, 90) == 100 && NextCycleLevel({20, 50}, 90) == 20, L"cycle: above all entries -> wraps");
    Check(NextCycleLevel(cycle, -1) == 20, L"cycle: unknown level -> first entry");
    Check(NextCycleLevel({100, 50, 20}, 100) == 50 && NextCycleLevel({100, 50, 20}, 20) == 100, L"cycle: descending list");
    Check(NextCycleLevel({}, 50) == -1, L"cycle: empty list");

    // A cycle starts from the targets' average and sets them all.
    g_displays[0].value = 20; g_displays[1].value = 30;
    ChangeDisplays(targets, 0, CycleTarget(cycle, targets), true);
    Check(g_displays[0].value == 50 && g_displays[1].value == 50, L"cycle: from the average 25 -> 50/50");
    ChangeDisplays(targets, 0, CycleTarget(cycle, targets), true);
    Check(g_displays[0].value == 100 && g_displays[1].value == 100, L"cycle: 50 -> 100/100");

    // Turning a single monitor off and on again; laptop screens can't.
    g_displays.push_back(fake(L"c", L"Laptop", 40));
    g_displays[2].info.method = Method::Wmi;
    Check(ToggleDisplaysPower({&g_displays[0]}) && g_displaysOff == std::set<std::wstring>{L"a"}, L"power: one monitor off");
    Check(ToggleDisplaysPower({&g_displays[0], &g_displays[1]}) && g_displaysOff.size() == 2, L"power: partly off -> all off");
    Check(ToggleDisplaysPower({&g_displays[0], &g_displays[1]}) && g_displaysOff.empty(), L"power: all off -> on again");
    Check(!ToggleDisplaysPower({&g_displays[2]}) && g_displaysOff.empty(), L"power: laptop screen not turned off");
    {
        std::lock_guard<std::mutex> lock(hw::g_mutex);
        Check(hw::g_pendingPower.size() == 2 && hw::g_pendingPower[L"a"] && hw::g_pendingPower[L"b"], L"power: latest requests are 'on'");
        hw::g_pendingPower.clear();
    }
    g_displays.pop_back();

    // Monitors that are off are saved, so that the next start turns them on.
    ToggleDisplaysPower({&g_displays[1]});
    Check(StubStorage()[L"displaysOffCount"] == L"1" && StubStorage()[L"displaysOffKey0"] == L"b", L"power: off monitor saved");
    // A snapshot that includes the requests replaces the UI's state: here the
    // "off" command failed, so the monitor is on.
    SyncDisplaysOff({g_displays[0].info, g_displays[1].info});
    Check(g_displaysOff.empty() && !StubStorage().contains(L"displaysOffCount"), L"power: failed command -> shown as on, save cleared");
    g_displaysOff = {L"a"};
    TurnOnDisplaysOff();
    {
        std::lock_guard<std::mutex> lock(hw::g_mutex);
        Check(g_displaysOff.empty() && hw::g_pendingPower[L"a"], L"power: turned on again when the mod exits");
        hw::g_pendingPower.clear();
    }

    // The worker doesn't write the level of a monitor that is off, and
    // keeps it for when it's turned on.
    hw::HwDisplay off;
    off.info.key = L"x";
    off.info.method = Method::Ddc;
    off.info.off = true;
    off.info.raw = 50;
    hw::g_displays = {off};
    auto failedSets = hw::ApplyAll({{L"x", 30}});
    Check(failedSets.empty() && hw::g_displays[0].info.raw == 30 && hw::g_displays[0].rawPending, L"power: level of an off monitor kept, not written");
    hw::g_displays.clear();

    // Tray wheel: linked from the average, unlinked each from its own level.
    g_settings.trayScrollStep = 5;
    g_lastHwReadTick = GetTickCount();
    g_displays[0].value = 20; g_displays[1].value = 40;
    StepFromTrayWheel(2, nullptr);
    Check(g_displays[0].value == 40 && g_displays[1].value == 40, L"tray wheel linked: avg 30 + 2*5 -> 40/40");
    g_linked = false;
    g_displays[0].value = 20;
    StepFromTrayWheel(-1, nullptr);
    Check(g_displays[0].value == 15 && g_displays[1].value == 35, L"tray wheel unlinked: each -5");
    g_linked = true;
    // Stale monitor levels: the steps wait for a read, then run.
    g_lastHwReadTick = GetTickCount() - 10000;
    StepFromTrayWheel(1, nullptr);
    StepFromTrayWheel(1, nullptr);
    Check(g_deferredWheelSteps == 2 && g_displays[0].value == 15, L"tray wheel waits for fresh levels");
    RunDeferredHotkeys();
    Check(g_deferredWheelSteps == 0 && g_displays[0].value == 35 && g_displays[1].value == 35, L"tray wheel deferred steps run: avg 25 + 10");
    {
        std::lock_guard<std::mutex> lock(hw::g_mutex);
        hw::g_requestRefresh = false;
    }

    g_displays.clear();
    std::lock_guard<std::mutex> lock(hw::g_mutex);
    hw::g_pendingSet.clear();
}

int wmain(int argc, wchar_t** argv) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    if (argc > 1) g_theme = argv[1];
    std::wstring out = argc > 2 ? argv[2] : L"panel";
    g_trayUseGuid = false;  // don't bind the tray GUID to this test exe
    LoadSettings();
    RunLogicTests();
    WhTool_ModInit();
    Sleep(5000);

    // The panel, linked (the default) and unlinked. The harness thread
    // flips g_linked directly while the panel is hidden; nothing reaches
    // the hardware, unlike SetLinked.
    auto capturePanel = [&](PCWSTR suffix) {
        PostMessage(g_hwndMain, WM_APP_TRAY, 0, NIN_SELECT);
        Sleep(2000);
        RECT r;
        GetWindowRect(g_hwndPanel, &r);
        wprintf(L"panel%s visible=%d size=%dx%d\n", suffix, IsWindowVisible(g_hwndPanel),
                (int)(r.right - r.left), (int)(r.bottom - r.top));
        int w = r.right - r.left, h = r.bottom - r.top;
        HDC screen = GetDC(nullptr);
        HDC mem = CreateCompatibleDC(screen);
        HBITMAP bmp = CreateCompatibleBitmap(screen, w, h);
        SelectObject(mem, bmp);
        PrintWindow(g_hwndPanel, mem, 2);
        wprintf(L"panel%s saved=%d\n", suffix, SavePng(bmp, (out + L"-panel" + suffix + L".png").c_str()));
        DeleteDC(mem);
        DeleteObject(bmp);
        ReleaseDC(nullptr, screen);
        PostMessage(g_hwndMain, WM_APP_TRAY, 0, NIN_SELECT);
        Sleep(600);
    };
    capturePanel(L"-linked");
    g_linked = false;
    capturePanel(L"");

    // "+0%" hotkey while unlinked: shows the indicator for the active
    // display without changing brightness.
    PostMessage(g_hwndMain, WM_HOTKEY, 1, 0);
    Sleep(500);
    RECT r;
    GetWindowRect(g_hwndOsd, &r);
    wprintf(L"osd visible=%d size=%dx%d\n", IsWindowVisible(g_hwndOsd),
            (int)(r.right - r.left), (int)(r.bottom - r.top));
    UINT dpi = GetMonitorDpi(MonitorFromRect(&r, MONITOR_DEFAULTTOPRIMARY));
    if (!g_displays.empty()) {
        wprintf(L"osd saved=%d\n", SaveOsdPng({{g_displays[0].label, g_displays[0].value}}, dpi, out + L"-osd.png"));
    }
    wprintf(L"osd-multi saved=%d\n", SaveOsdPng({{L"LG HDR 4K", 33}, {L"Built-in display", 40}}, dpi, out + L"-osd-multi.png"));

    WhTool_ModUninit();
    wprintf(L"logic test failures: %d\nexit ok\n", g_failures);
    return g_failures ? 1 : 0;
}
