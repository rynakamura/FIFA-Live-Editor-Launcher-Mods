#include <gui.h>

GUI::GUI() {

}

GUI::~GUI() {

}

void GUI::Init() {
    if (initialized) return;
    sprintf(about_txt, "%s - %s", TOOL_NAME, TOOL_VERSION);

    about_window_size = ImVec2(
        1280.0f * 0.8f,
        800.0f - 30.0f
    );
    about_window_pos = ImVec2(
        static_cast<float>(GetSystemMetrics(SM_CXSCREEN) / 2),
        static_cast<float>(GetSystemMetrics(SM_CYSCREEN) / 2)
    );

    locale_window.Init();
    initialized = true;
}

void GUI::MainDockspace() {
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);


    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = GetMainDockspaceID();
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        if (create_dockspace_layout) {
            create_dockspace_layout = false;
            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            // split the dockspace into 2 nodes --
            // DockBuilderSplitNode takes in the following args in the following order
            //   window ID to split, direction, fraction (between 0 and 1),
            // the final two setting let's us choose which id we want (which ever one we DON'T set as NULL,
            // will be returned by the function)
            // out_id_at_dir is the id of the node in the direction we specified earlier,
            // out_id_at_opposite_dir is in the opposite direction
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.33f, nullptr, &dockspace_id);
            auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.33f, nullptr, &dockspace_id);
            auto dock_id_right_down = ImGui::DockBuilderSplitNode(dock_id_right, ImGuiDir_Down, 0.33f, nullptr, &dock_id_right);

            // we now dock our windows into the docking node we made above
            locale_window.Dock(dockspace_id);
            injector_window.Dock(dock_id_left);
            ImGui::DockBuilderDockWindow("Info", dock_id_right);
            ImGui::DockBuilderDockWindow("Disclaimer", dock_id_right_down);

            ImGui::DockBuilderFinish(dockspace_id);
        }
    }
    ImGui::End();

    DrawMainMenuBar();
}

void GUI::SetupImGUI() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.IniFilename = g_Config.imgui_ini.c_str();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    //io.ConfigViewportsNoDefaultParent = true;
    //io.ConfigDockingAlwaysTabBar = true;
    //io.ConfigDockingTransparentPayload = true;
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
    //io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }


    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
}

void GUI::DrawMainMenuBar() {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem(injector_window.GetWindowName(), NULL, &injector_window.show);
            ImGui::MenuItem("Locale.Ini", NULL, &locale_window.show);
            ImGui::MenuItem("Info", NULL, &show_info_window);
            ImGui::MenuItem("Disclaimer", NULL, &show_disclaimer);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItemURL("Wiki", "https://github.com/xAranaktu/FIFA-23-Live-Editor/wiki");
            ImGui::MenuItemURL("Getting Started", "https://github.com/xAranaktu/FIFA-23-Live-Editor/wiki/Getting-Started");
            ImGui::MenuItemURL("Discord", "https://discord.gg/va9EtdB");
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About")) {
            show_about = true;
        }

        //if (ImGui::BeginMenu("Edit"))
        //{
        //    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
        //    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
        //    ImGui::Separator();
        //    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        //    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        //    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
        //    ImGui::EndMenu();
        //}
        ImGui::EndMainMenuBar();
    }
}

void GUI::DrawInfoWindow(bool* p_open) {
    ImGui::Begin("Info", p_open);
    ImGui::Text("Version: %s", g_Core.GetToolVer());

    ImGui::End();
}

void GUI::DrawDisclaimer(bool* p_open) {
    ImGui::Begin("Disclaimer", p_open);

    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(DisclaimerContent);
    }

    if (ImGui::Checkbox("Show Warning at startup", &g_Config.launch_values.show_disclaimer_msg)) {
        g_Config.Save();
    }

    ImGui::End();
}

void GUI::DrawAbout(bool* p_open) {
    ImGuiIO& io = ImGui::GetIO();

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowPos(about_window_pos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(about_window_size, ImGuiCond_Appearing);
    ImGui::Begin("About", p_open, window_flags);
    ImVec2 avail_space = ImGui::GetContentRegionAvail();

    ImGui::BeginGroup();

    ImGui::TextCenter(std::string(about_txt));
    ImGui::TextCenter("Created by ", "Aranaktu", "https://github.com/xAranaktu");

    ImGui::EndGroup();

    ImGui::BeginChild("#3rdPartyLibs", ImVec2(avail_space.x, avail_space.y * 0.40f));
    ImGui::TextCenter("Used Libraries:");
    ImGui::InputTextMultiline("##3rdPartyLibs Content", &third_party_libs, ImVec2(avail_space.x, -10), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly);
    ImGui::EndChild();

    ImGui::BeginChild("#Credits", ImVec2(avail_space.x, avail_space.y * 0.40f));

    ImGui::InputTextMultiline("##Credits Content", &credits, ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly);
    ImGui::EndChild();

    ImGui::TextCenter("Support this project on ", "Patreon", "https://www.patreon.com/xAranaktu");

    ImGui::End();
}

void GUI::Draw() {
    MainDockspace();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    if (locale_window.show)
        locale_window.Draw(&locale_window.show);

    if (injector_window.show)
        injector_window.Draw(&injector_window.show);

    if (show_info_window)
        DrawInfoWindow(&show_info_window);

    if (show_disclaimer)
        DrawDisclaimer(&show_disclaimer);

    if (show_about)
        DrawAbout(&show_about);

}

GUI g_GUI;