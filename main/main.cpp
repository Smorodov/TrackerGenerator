#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "TextEditor.h"
#include "nfd.h"
#include "boost/thread.hpp"
#include "boost/signals2.hpp"
#include "boost/assign/list_of.hpp"
#include <mutex>

#include "Observer.h"
#include "Observable.h"

#include "TinyXML.h"

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually.
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


class GUIWidget
{
public:
    enum WidgetType {
        None,
        Header,
        Label,
        Slider,
        Button,
        CheckBox,
        TextInput,
        ListBox,
        ListBoxItem
    };

    std::map<std::string, WidgetType> stringToWidgetType;
    std::map<WidgetType, std::string> widgetTypeToString;

    WidgetType Type;
    std::string Name;
    std::string Value;

    std::vector<GUIWidget> Widgets;
    std::vector<std::string> Items;
    int currentItemIndex;

    GUIWidget()
    {
        Type = GUIWidget::None;
        currentItemIndex = 0;
        stringToWidgetType = boost::assign::map_list_of
            ("None",None)
            ("Header",Header)
            ("Label",Label)
            ("Slider",Slider)
            ("Button",Button)
            ("CheckBox",CheckBox)
            ("TextInput",TextInput)
            ("ListBox",ListBox)
            ("ListBoxItem",ListBoxItem);

        widgetTypeToString = boost::assign::map_list_of
        (None, "None")
            (Header, "Header")
            (Label, "Label")
            (Slider, "Slider")
            (Button, "Button")
            (CheckBox, "CheckBox")
            (TextInput, "TextInput")
            (ListBox, "ListBox")
            (ListBoxItem, "ListBoxItem");
    }

    ~GUIWidget()
    {
    }
    std::string GetTypeString(void)
    {
        return widgetTypeToString[Type];
    }
    void Call(GUIWidget& eventSource)
    {
        switch (Type)
        {
        case Header:
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth());
            if (ImGui::CollapsingHeader(Name.c_str()))
            {
                for (int i = 0; i < Widgets.size(); ++i)
                {
                    Widgets[i].Call(eventSource);                 
                }
            }
            break;
        case Label:
            break;
        case Slider:
            break;
        case Button:
            if (ImGui::Button(Name.c_str(), ImVec2(ImGui::GetWindowWidth(), 30)))
            {
                eventSource = *this;
            }
            break;
        case CheckBox:
            break;
        case TextInput:
            break;
        case ListBox:
            Items.clear();
            for (int i = 0; i < Widgets.size(); ++i)
            {
                Items.push_back(Widgets[i].Name);
            }

            if (currentItemIndex > (Items.size() - 1))
            {
                currentItemIndex = 0;
            }

            if (!Items.empty())
            {
                Value = Items[currentItemIndex];
            }

            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 2);
            if (ImGui::BeginCombo(Name.c_str(), Value.c_str(), 0))
            {
                int n = 0;
                for (; n < Items.size(); n++)
                {
                    bool is_selected = (Value == Items[n]);
                    if (ImGui::Selectable(Items[n].c_str(), is_selected))
                    {
                        Value = Items[n];
                        if (currentItemIndex != n)
                        {
                            eventSource = *this;
                        }
                        currentItemIndex = n;
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            break;
        case ListBoxItem:
            break;
        }
    }

};

// -----------------------------
//
// -----------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}
// -----------------------------
//
// -----------------------------
void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
// -----------------------------
//
// -----------------------------
struct MainWindowObservers
{
    enum { OnWidgetEvent,
           OnRunEvent,
           OnCloseEvent };
    using ObserverTable = std::tuple<
        Observer<void(GUIWidget& widget)>,
        Observer<void(void)>,
        Observer<void(void)>
    >;
};
// -----------------------------
//
// -----------------------------
class MainWindow : public Observable< MainWindowObservers>
{
public:
    std::string LocaleName;
    // Размеры окна
    int width;
    int height;
    // Ширина боковой панели
    int sidePanelWidth;
    // Главное окно
    GLFWwindow* window;

    bool isRunning;
    std::mutex m;
    boost::thread* thr;

    bool isColor;
    int inputType;
    int detectorType;
    int treckerType;
    int outputType;

    bool generateButton;
    bool generateButton_prev;

    void generateProject(void);
    ImVec4 clear_col;
    TextEditor editor;
    TextEditor::LanguageDefinition lang;

    MainWindow()
    {
        LocaleName = "ru_RU.utf8";
        setlocale(LC_ALL, LocaleName.c_str());
        // Размеры окна
        width = 1024;
        height = 768;
        // Ширина боковой панели
        sidePanelWidth = 300;
        // Главное окно
        window = nullptr;

        isRunning = false;
        thr = nullptr;

        isColor = false;
        inputType = 0;
        detectorType = 0;
        treckerType = 0;
        outputType = 0;
        generateButton = false;
        generateButton_prev = false;

        clear_col = ImColor(114, 144, 154);       
        lang = TextEditor::LanguageDefinition::CPlusPlus();
        editor.SetLanguageDefinition(lang);

        ReadGUIConfig();
    }

    std::vector<GUIWidget> Widgets;
    void ReadGUIConfig()
    {
        TiXmlDocument xml("GUIConfig.xml");
        if (!xml.LoadFile()) { std::cout << "Can't load GUIConfig.xml" << std::endl; return; }
        TiXmlElement* root = xml.FirstChildElement("GUIConfig");
        std::string encoding = root->Attribute("encoding");
        std::cout << encoding << std::endl;

        for (TiXmlElement* e = root->FirstChildElement("Header"); e != NULL; e = e->NextSiblingElement("Header"))
        {
            std::string Name = e->Attribute("name");
            std::cout << Name << std::endl;

            GUIWidget H;
            H.Type = GUIWidget::Header;
            H.Name = Name;

            for (TiXmlElement* ec = e->FirstChildElement("Button"); ec != NULL; ec = ec->NextSiblingElement("Button"))
            {
                std::string Name = ec->Attribute("name");
                std::cout << Name << std::endl;
                GUIWidget W;
                W.Type = GUIWidget::Button;
                W.Name = Name;
                H.Widgets.push_back(W);
            }

            for (TiXmlElement* ec = e->FirstChildElement("ListBox"); ec != NULL; ec = ec->NextSiblingElement("ListBox"))
            {
                std::string Name = ec->Attribute("name");
                std::cout << Name << std::endl;

                GUIWidget W;
                W.Name = Name;
                W.Type = GUIWidget::ListBox;

                for (TiXmlElement* eci = ec->FirstChildElement("Item"); eci != NULL; eci = eci->NextSiblingElement("Item"))
                {
                    std::string Name = eci->Attribute("name");
                    std::cout << Name << std::endl;
                    GUIWidget I;
                    I.Type = GUIWidget::ListBoxItem;
                    I.Name = Name;
                    W.Widgets.push_back(I);
                }
                H.Widgets.push_back(W);
            }
            Widgets.push_back(H);
        }
    }

    void setTextFromFile(std::string fileToEdit)
    {       
        {
            std::ifstream t(fileToEdit);
            if (t.good())
            {
                std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
                editor.SetText(str);
            }
        }
    }

    MainWindow::~MainWindow()
    {
        if (thr != nullptr)
        {
            thr->interrupt();
            thr->join();
            delete thr;
            thr = nullptr;
        }
    }
    void InitGraphics()
    {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return;

        // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
        window = glfwCreateWindow(width, height, "Tracker Generator", NULL, NULL);
        if (window == NULL)
            return;
        glfwSetKeyCallback(window, key_callback);
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync
        // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
        bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
        bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
        bool err = gladLoadGL() == 0;
#else
        bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
        if (err)
        {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
            return;
        }
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        // Load Fonts
        io.Fonts->AddFontFromFileTTF("../../misc/fonts/a_FuturaOrto.TTF", 20, NULL, io.Fonts->GetGlyphRangesCyrillic());
    }

    // -----------------------------
    // Отрисовка редактора кода
    // -----------------------------
    void RenderEditor(void)
    {
        enum StringID { Preview,
                        File,
                        Save,
                        Load,
                        Editor,
                        ReadOnly,
                        Undo,
                        Redo,
                        Cut,
                        Copy,
                        Paste,
                        Delete,
                        Insert,
                        SelectAll,
                        View,
                        DarkTheme,
                        LightTheme,
                        OldSchoolTheme,
                        Strings
        };
        std::map<StringID,std::string> stringToCaption;
        stringToCaption = boost::assign::map_list_of
            (Preview, "Preview")
            (File, "File")
            (Save, "Save")
            (Load, "Load")
            (Editor, "Editor")
            (ReadOnly, "Read only")
            (Undo, "Undo")
            (Redo, "Redo")
            (Cut, "Cut")
            (Copy, "Copy")
            (Paste, "Paste")
            (Delete, "Delete")
            (Insert, "Insert")
            (SelectAll, "SelectAll")
            (View, "View")
            (DarkTheme, "DarkTheme")
            (LightTheme, "LightTheme")
            (OldSchoolTheme, "OldSchoolTheme")
            (Strings,"Strings") ;

        ImGui::Begin(stringToCaption[Preview].c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        auto cpos = editor.GetCursorPosition();
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu(stringToCaption[File].c_str()))
            {
                if (ImGui::MenuItem(stringToCaption[Save].c_str()))
                {
                    auto textToSave = editor.GetText();
                    /// save text....
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(stringToCaption[Editor].c_str()))
            {
                bool ro = editor.IsReadOnly();
                if (ImGui::MenuItem(stringToCaption[ReadOnly].c_str(), nullptr, &ro))
                    editor.SetReadOnly(ro);
                ImGui::Separator();

                if (ImGui::MenuItem(stringToCaption[Undo].c_str(), "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
                    editor.Undo();
                if (ImGui::MenuItem(stringToCaption[Redo].c_str(), "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
                    editor.Redo();

                ImGui::Separator();

                if (ImGui::MenuItem(stringToCaption[Copy].c_str(), "Ctrl-C", nullptr, editor.HasSelection()))
                    editor.Copy();
                if (ImGui::MenuItem(stringToCaption[Cut].c_str(), "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                    editor.Cut();
                if (ImGui::MenuItem(stringToCaption[Delete].c_str(), "Del", nullptr, !ro && editor.HasSelection()))
                    editor.Delete();
                if (ImGui::MenuItem(stringToCaption[Insert].c_str(), "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                    editor.Paste();

                ImGui::Separator();

                if (ImGui::MenuItem(stringToCaption[SelectAll].c_str(), nullptr, nullptr))
                    editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(stringToCaption[View].c_str()))
            {
                if (ImGui::MenuItem(stringToCaption[DarkTheme].c_str()))
                    editor.SetPalette(TextEditor::GetDarkPalette());
                if (ImGui::MenuItem(stringToCaption[LightTheme].c_str()))
                    editor.SetPalette(TextEditor::GetLightPalette());
                if (ImGui::MenuItem(stringToCaption[OldSchoolTheme].c_str()))
                    editor.SetPalette(TextEditor::GetRetroBluePalette());
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Text(u8"%6d/%-6d %6d %s | %s | %s | %s ", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
            stringToCaption[Strings].c_str(),
            editor.IsOverwrite() ? "Ovr" : "Ins",
            editor.CanUndo() ? "*" : " ",
            editor.GetLanguageDefinition().mName.c_str());
            editor.Render(stringToCaption[Preview].c_str());
    }


    // -----------------------------
    // Освобождение ресурсов графики
    // -----------------------------
    void TerminateGraphics(void)
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    // -----------------------------
    // 
    // -----------------------------
    void MainWindow::worker(void)
    {
        isRunning = true;
        InitGraphics();
        Notify<MainWindowObservers::OnRunEvent>();
        // -------------
        // Main loop
        // -------------
        while (!boost::this_thread::interruption_requested() && !glfwWindowShouldClose(window))
        {      
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowSize(ImVec2(sidePanelWidth, height));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::Begin("Panel", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);        

            GUIWidget evtSrc;
            for (int i = 0; i < Widgets.size(); ++i)
            {
                    Widgets[i].Call(evtSrc);
                    if (evtSrc.Type!= GUIWidget::None)
                    {                        
                        Notify<MainWindowObservers::OnWidgetEvent>(evtSrc);
                        break;
                    }
            }           
            ImGui::End();

            
            ImGui::SetNextWindowSize(ImVec2(width - sidePanelWidth, height));
            ImGui::SetNextWindowPos(ImVec2(sidePanelWidth, 0));
            
            RenderEditor();

            // Rendering
            ImGui::End();


            glClearColor(clear_col.x, clear_col.y, clear_col.z, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            // Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
        TerminateGraphics();
        isRunning = false;
        Notify<MainWindowObservers::OnCloseEvent>();
    }
    void MainWindow::Run(void)
    {
        if (!isRunning)
        {
            thr = new boost::thread(&MainWindow::worker, this);
            isRunning = true;
        }
    }

    void MainWindow::Stop(void)
    {
        if (isRunning)
        {
            thr->interrupt();
            thr->join();
            delete thr;
            thr = nullptr;
            isRunning = false;
        }
    }

};

// -----------------------------
//
// -----------------------------
// Application: our Observer.
class Application
{
public:
    // -----------------------------
    //
    // -----------------------------
    explicit Application(MainWindow& worker) :
        worker_(worker)
    {
        finished = false;
        worker_.Register<MainWindowObservers::OnWidgetEvent>([this](GUIWidget& widget)
            {
                OnWidget(widget);
            });

        worker_.Register < MainWindowObservers::OnCloseEvent >([this](void)
            {
                OnClose();
            });

        worker_.Register < MainWindowObservers::OnRunEvent >([this](void)
            {
                OnRun();
            });
        std::cout << "Events - registered" << std::endl;

        worker_.Run();

        while (worker_.isRunning)
        {
            Sleep(100);
            finished = true;
        }
        worker_.Stop();
    }
    // -----------------------------
    //
    // -----------------------------
    ~Application()
    {

    }

private:
    bool finished;
    
    void OnRun()
    {
        std::cout << "On run event." << std::endl;
    }

    void OnClose()
    {
        std::cout << "On close event." << std::endl;
    }

    void OnWidget(GUIWidget& widget)
    {
        std::cout << "On " << widget.GetTypeString() << " \"" << widget.Name << "\"" << std::endl;
        
        switch (widget.Type)
        {
        case GUIWidget::Button:
            
            break;
        case GUIWidget::ListBox:
           
            break;
        case GUIWidget::CheckBox:
           
            break;
        case GUIWidget::TextInput:
           
            break;
        }
        //worker_.editor.SetText(u8"Привет !");
    }

    MainWindow& worker_;
};

// -----------------------------
//
// -----------------------------
int main()
{
    MainWindow worker;  
    Application application{ worker };
}

void clean_folder(const std::string path)
{
    if (!path.empty())
    {
        namespace fs = boost::filesystem;

        fs::path apk_path(path);
        fs::recursive_directory_iterator end;

        for (fs::recursive_directory_iterator i(apk_path); i != end; ++i)
        {
            const fs::path cp = (*i);
            if (!boost::filesystem::is_directory(cp))
            {
                boost::filesystem::remove(cp.string());
            }
        }
    }
}

std::string GeneratedFolder = "./generated/";
std::string DBfolder = "./../../codeDB/";

void createFolders(void)
{
    boost::filesystem::path path(GeneratedFolder);
    clean_folder(GeneratedFolder);
    std::vector<std::string> folders_to_create = { "src","common/inputs","common/detectors","common/trackers","common/outputs" };
    for (int i = 0; i < folders_to_create.size(); ++i)
    {
        std::string srcFolder = GeneratedFolder + folders_to_create[i];
        path = boost::filesystem::path(srcFolder);
        if (!boost::filesystem::exists(path))
        {
            boost::filesystem::create_directories(path);
        }
    }
}

void copyCommon(void)
{
    std::vector<std::string> files_from = { "Common/defines.h","Common/nms.h" };
    std::vector<std::string> files_to = { "common/defines.h","common/nms.h" };

    for (int i = 0; i < files_from.size(); ++i)
    {
        boost::filesystem::path from(DBfolder + files_from[i]);
        boost::filesystem::path to(GeneratedFolder + files_to[i]);
        boost::filesystem::copy_file(from, to);
    }
}

void generateProject(void)
{
    createFolders();
    copyCommon();
}