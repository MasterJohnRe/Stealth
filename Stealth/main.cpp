
#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "HelperFunctions.h"
#include "StealthService.h"
#include "NamedPipesHandler.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

LPCSTR PROCESS_NAME = "The Witcher 3";

StealthService stealthService = StealthService();
NamedPipesHandler namedPipesHandler;
MemoryAccess memoryHandler;
static auto logger = spdlog::basic_logger_mt("file_logger", "logfile.txt");

struct AddressEntry {
    std::string description;
    DWORD64 address;
    std::string type;
    std::string value;
};

void setupWindow(GLFWwindow*& window) {
    if (!glfwInit())
        exit(EXIT_FAILURE);

    // Set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(1280, 720, "ImGui Example", NULL, NULL);
    if (window == NULL)
        exit(EXIT_FAILURE);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
}

void cleanup(GLFWwindow* window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void AddAddressEntry(std::vector<AddressEntry>& addresses) {
    AddressEntry new_entry;
    new_entry.description = "New Entry";
    new_entry.address = 0x0000;
    new_entry.type = "int";
    new_entry.value = "0";
    addresses.push_back(new_entry);
}

// Function to display the new access window
void RenderAccessWindow(const std::vector<AccessEntry>& accessEntries, bool& showAccessWindow) {
    if (!showAccessWindow) return;
    HelperFunctions helperFunctionsService = HelperFunctions();
    // Create a new ImGui window to display access data
     // Set the initial size of the window to be larger
    ImGui::SetNextWindowSize(ImVec2(650, 800), ImGuiCond_Always);      // Set width to 1000 and height to 800
    ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_Always);         // Optional: Set position to be closer to top left
    ImGui::Begin("Memory Access Viewer", &showAccessWindow);  // Window title is "Memory Access Viewer"

   // Create a child window to control the size of the table inside
    ImGui::BeginChild("TableRegion", ImVec2(950, 400), true, ImGuiWindowFlags_AlwaysUseWindowPadding);  // Adjust size here

    // Create a table to show the address, value, and count with specific size
    if (ImGui::BeginTable("AccessTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 450.0f);  // Set column width
        ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed, 150.0f);  // Set column width
        ImGui::TableHeadersRow();

        // Display each entry in the table
        for (const auto& entry : accessEntries) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", helperFunctionsService.ConvertAddressToString(entry.RIP).c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%d", entry.count);
        }

        ImGui::EndTable();
    }
    ImGui::EndChild();  // End the child region

    // Close button at the bottom
    if (ImGui::Button("Close")) {
        namedPipesHandler.closePipe();
    }

    ImGui::End();
}

// Function to find the matching entry using simple iteration
std::vector<AccessEntry>::iterator findAccessEntry(const AccessEntry& accessEntry, std::vector<AccessEntry>& accessEntries) {
    for (auto it = accessEntries.begin(); it != accessEntries.end(); ++it) {
        if (it->RIP == accessEntry.RIP) {
            return it;  // Return the iterator if RIP matches
        }
    }
    return accessEntries.end();  // Return end() if no match is found
}

void RenderAddressWindow(std::vector<AddressEntry>& addresses) {
    HelperFunctions helperFunctionsService = HelperFunctions();
    static bool showAccessWindow = false;  // Track whether to show the access window
    static std::vector<AccessEntry> accessEntries;  // Stores the entries read from the named pipe

    // Create the main window
    ImGui::Begin("Address Viewer");

    // Add a button to add addresses
    if (ImGui::Button("Add Address Manually")) {
        AddAddressEntry(addresses);
    }

    // Spacing between button and table
    ImGui::Separator();

    // Create the table with columns
    if (ImGui::BeginTable("AddressTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
    {
        // Define the columns
        ImGui::TableSetupColumn("Description");
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        // Track the index of the row to delete or find
        int rowToDelete = -1;
        int rowToFind = -1;

        // Fill the table with address entries
        for (size_t row = 0; row < addresses.size(); ++row)
        {
            ImGui::TableNextRow();

            // Make the entire row a selectable item
            ImGui::TableSetColumnIndex(0);
            ImGui::Selectable(("##Row" + std::to_string(row)).c_str(), false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0, 0));

            // If right-click on the entire row, open a context menu
            if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                ImGui::OpenPopup(("RowPopup" + std::to_string(row)).c_str());
            }

            // Show the context menu for the whole row
            if (ImGui::BeginPopup(("RowPopup" + std::to_string(row)).c_str())) {
                static char buffer[128];

                // Edit Description
                strcpy_s(buffer, addresses[row].description.c_str());
                if (ImGui::InputText("Edit Description", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    addresses[row].description = buffer;
                }

                // Edit Address as a string and convert it to DWORD64
                strcpy_s(buffer, helperFunctionsService.ConvertAddressToString(addresses[row].address).c_str());
                if (ImGui::InputText("Edit Address", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    addresses[row].address = helperFunctionsService.ConvertStringToAddress(buffer);
                }

                // Edit Type
                strcpy_s(buffer, addresses[row].type.c_str());
                if (ImGui::InputText("Edit Type", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    addresses[row].type = buffer;
                }

                // Edit Value
                strcpy_s(buffer, addresses[row].value.c_str());
                if (ImGui::InputText("Edit Value", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
                    addresses[row].value = buffer;
                }

                // Add the "Find what access this address" button
                if (ImGui::Button("Find what access this address")) {
                    rowToFind = row;  // Mark the row for finding address access
                    ImGui::CloseCurrentPopup();
                }

                // Add a delete button in the context menu
                if (ImGui::Button("Delete Row")) {
                    rowToDelete = row;  // Mark the row for deletion
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            // Render text for each column, converting DWORD address to a readable string
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", addresses[row].description.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", helperFunctionsService.ConvertAddressToString(addresses[row].address).c_str());
            ImGui::TableSetColumnIndex(2); ImGui::Text("%s", addresses[row].type.c_str());
            ImGui::TableSetColumnIndex(3); ImGui::Text("%s", addresses[row].value.c_str());
        }

        // Remove the row outside of the loop to prevent issues with invalidating iterators
        if (rowToDelete != -1) {
            addresses.erase(addresses.begin() + rowToDelete);
        }

        // Handle the "Find what access this address" operation
        if (rowToFind != -1) {
            try {
                showAccessWindow = true;
                stealthService.attachToProcess(PROCESS_NAME);
                namedPipesHandler.createPipe("FindWhatAccessThisAddressNamedPipe");
                stealthService.findWhatAccessThisAddress();
                namedPipesHandler.writeData(helperFunctionsService.ConvertAddressToString(addresses[rowToFind].address), 15000);
            }
            catch (PipeWriteException ex) {
                logger->error("Error: can't write address to pipeline: ", ex.what());
            }
        }

        ImGui::EndTable();
    }

    ImGui::End();

    // Render the Access Window after creating or updating it
    RenderAccessWindow(accessEntries, showAccessWindow);

    if (showAccessWindow) {
        try{
            std::string data;
            namedPipesHandler.readDataAsync();  // Start async read operation  // Read from the named pipe
            if (namedPipesHandler.dataFuture.wait_for(std::chrono::milliseconds(300)) == std::future_status::ready) {
                std::string data = namedPipesHandler.dataFuture.get();  // Retrieve the data when ready
                logger->info("Data received in GUI: ", data);
                AccessEntry currentAccessEntry = helperFunctionsService.parseAccessEntry(data);
                // Parse the data into accessEntries
                auto it = findAccessEntry(currentAccessEntry, accessEntries);
                if (it != accessEntries.end()) {
                    currentAccessEntry.count = currentAccessEntry.count + it->count;
                    accessEntries.erase(it);
                    accessEntries.push_back(currentAccessEntry);  // Custom parsing function
                }
                else {
                    accessEntries.push_back(currentAccessEntry);  // Custom parsing function
                }
            }
        }
        catch (PipeNotOpenException ex) {
            logger->error("Failed reading data from FindWhatAccessThisAddress Pipeline", ex.what());
        }
        catch (PipeReadException ex) {
            logger->error("Failed reading data from FindWhatAccessThisAddress Pipeline", ex.what());
        }
        catch (const std::exception& ex) {
            logger->error("Exception caught: " ,ex.what());
        }
    }
}

int createOpenGLWindow() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Set up OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL Example with Right-Click Context Menu", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Application state
    std::vector<AddressEntry> addresses;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render the address window
        RenderAddressWindow(addresses);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    spdlog::set_default_logger(logger);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
    // Your application code here
    createOpenGLWindow();
    return 0;
}