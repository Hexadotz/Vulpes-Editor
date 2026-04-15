// UI/uiManager.h
#pragma once
#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <memory>

#include "uiObjManager.h"
#include "uiEditorToolsManager.h"
#include "uiConsoleManager.h"

class uiManager {
public:
    static inline bool draw_grid = true;
    static inline int cell_size = 32;
    static inline bool snap_to_grid = true; 

    static int initialize_Ui(sf::RenderWindow& window);
    static bool confirm_window(const char* msg);
    static void draw_Ui();
    static void shutdown_Ui();

    static void saveSceneWithDialog();
    static void loadSceneWithDialog();

private:
    static void debug_panel();
};

#endif