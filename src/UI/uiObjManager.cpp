
#include "uiObjManager.h"
#include <filesystem>
#include "../obj/entites/entity2D.hpp"
#include "../obj/entites/testEntity.hpp"

using json = nlohmann::json;
std::vector<EntityData> objectManagerUi::entity_list;
std::vector<std::string> objectManagerUi::scene_entities;

void objectManagerUi::setSelectedEntity(Entity2D* entity) {
    s_selectedEntity = entity;
    if (entity) {
        //std::cout << "[UI] Selected Entity2D: " << entity->getName()
            //<< " (ID: " << entity->getUniqueID() << ")" << std::endl;

        CONSOLE_INFO("[UI] Selected Entity2D: " + entity->getName() + " (ID: " + std::to_string(entity->getUniqueID()) + ")");
    }
}

Entity2D* objectManagerUi::getSelectedEntity() {
    return s_selectedEntity;
}

void objectManagerUi::add_object(const char* label) {
    scene_entities.push_back(label);
}

ImVec2 objectManagerUi::get_panel_size() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float screen_width = viewport->Size.x;
    float screen_height = viewport->Size.y;

    return ImVec2(screen_width * 0.25f, screen_height * 0.5f);
}

void objectManagerUi::load_entity_info()
{
    entity_list.clear();

    std::string file_path = "../../../../asset/entityList.json";
    std::fstream file(file_path);

    json data;

    if (file.is_open()) {
        CONSOLE_INFO("Loaded from file");
        data = json::parse(file);

    }
    else {
        CONSOLE_INFO("Using embedded file");
        data = json::parse(EmbeddedData::ENTITY_LIST);
    }

    CONSOLE_INFO("file opened");

    for (const auto& entity : data["entities"])
    {
        EntityData entity_data;
        entity_data.id = entity.value("id", "error");
        entity_data.display_name = entity.value("display_name", "error");
        entity_data.description = entity.value("description", "penis");

        entity_list.push_back(entity_data);
    }

    file.close();
}

// user interface drawing here
void objectManagerUi::draw_Ui() {
    objectManagerUi::add_windowUi();
    objectManagerUi::scene_hierarchyUi();
    objectManagerUi::property_panelUi();
    // ImGui::ShowStyleEditor(); 
}

void objectManagerUi::scene_hierarchyUi() {
    ImGui::SetNextWindowSize(get_panel_size());
    ImGui::SetNextWindowPos(ImVec2(0, 19));
    ImGui::Begin("Scene Hierarchy", nullptr, panel_flags);

    auto* activeScene = SceneManager::instance().getActiveScene();

    if (activeScene) {
        if (ImGui::Button("Add Entity")) {
            objectManagerUi::draw_add_window = true;
        }
        ImGui::Separator();

        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick;

        // Render the actual scene hierarchy
        if (ImGui::TreeNodeEx(activeScene->getName().c_str(), base_flags)) {
            // IMPORTANT: Store IDs, not pointers
            std::vector<int> rootIDs;
            for (auto& entity : activeScene->getRootEntities()) {
                if (entity) {
                    rootIDs.push_back(entity->getUniqueID());
                }
            }

            // Render using IDs to re-find entities safely
            for (int id : rootIDs) {
                Entity2D* entity = activeScene->findEntityByID(id);
                if (entity) {
                    renderEntityNode(entity);
                }
            }
            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void objectManagerUi::vector2_widget(sf::Vector2f& value)
{
    if (ImGui::BeginTable("position_table", 3)) {

        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, get_panel_size().y);
        ImGui::TableSetupColumn("Axis", ImGuiTableColumnFlags_WidthFixed, 20.0f);

        float slider_width = 100.0f;

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Position:");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(slider_width);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.5f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.4f, 0.15f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

        ImGui::DragFloat("##", &value.x, 0.005f);
        ImGui::PopStyleColor(4);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(slider_width);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.3f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.15f, 0.4f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));

        ImGui::DragFloat("##", &value.y, 0.005f);
        ImGui::PopStyleColor(4);

        ImGui::EndTable();
    }
}

void objectManagerUi::rotation_widget(float& value)
{
    if (ImGui::BeginTable("position_table", 2)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, get_panel_size().y);

        static float min_rotation = -360.0f;
        static float max_rotaion = 360.f;

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Rotation:");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(90.0f);
        ImGui::SliderScalar("##", ImGuiDataType_Float, &value, &min_rotation, &max_rotaion);
        ImGui::SetItemTooltip("Ctrl + Click to input value manually");

        ImGui::EndTable();
    }
}

void objectManagerUi::property_panelUi() {
    ImGui::SetNextWindowSize(get_panel_size());
    ImGui::SetNextWindowPos(ImVec2(0, 19 + get_panel_size().y));
    ImGui::Begin("Properties", nullptr, panel_flags);

    if (s_selectedEntity) {
        ImGui::Text("Object: %s", s_selectedEntity->getName().c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("(ID: %d)", s_selectedEntity->getUniqueID());
        ImGui::Separator();

        //renderTransformWidgetsForEntity(s_selectedEntity);

        s_selectedEntity->renderEditorUI();
    }
    ImGui::End();
}

void objectManagerUi::add_windowUi() {
    if (objectManagerUi::draw_add_window)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImVec2 true_center = ImVec2(viewport->GetCenter().x - add_window_size.x / 2, viewport->GetCenter().y - add_window_size.y / 2);
        ImGui::SetNextWindowPos(true_center);
        ImGui::SetNextWindowSize(add_window_size);
        ImGui::Begin("Add component", nullptr, panel_flags);

        std::string cur_entity_name;
        std::string cur_entity_description;

        static int item_selected_idx = 0;
        int item_highlighted_idx = -1;
        ImGui::Text("Component to add: ");
        ImGui::Separator();
        if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 7 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int n = 0; n < entity_list.size(); n++)
            {
                const EntityData entity = entity_list.at(n);
                const bool is_selected = (item_selected_idx == n);

                if (ImGui::Selectable(entity.display_name.c_str(), is_selected)) {
                    item_selected_idx = n;
                    
                }

                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                    cur_entity_name = entity.display_name;
                    cur_entity_description = entity.description;
                }
            }
            ImGui::EndListBox();
        }
        ImGui::Separator();
        ImGui::Text("Entity Description:\n%s", cur_entity_description.c_str());
        if (ImGui::Button("Add"))
        {
            objectManagerUi::add_object(cur_entity_name.c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            objectManagerUi::draw_add_window = false;
        }
        ImGui::End();
    }
}

void objectManagerUi::renderEntityNode(Entity2D* entity) {
    if (!entity) return;

    int entityID = entity->getUniqueID();
    auto* scene = SceneManager::instance().getActiveScene();
    if (!scene) return;

    Entity2D* found = scene->findEntityByID(entityID);
    if (!found) return;

    entity = found;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (entity->getChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }
    if (entity->isSelected()) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    bool open = ImGui::TreeNodeEx(entity->getName().c_str(), flags);

    // Handle selection
    if (ImGui::IsItemClicked()) {
        setSelectedEntity(entity);
        scene->selectEntity(entity);
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {

        ImGui::SetDragDropPayload("ENTITY", &entityID, sizeof(int));

        ImGui::Text("Moving: %s", entity->getName().c_str());
        ImGui::Text("(ID: %d)", entityID);

        ImGui::EndDragDropSource();
    }


    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY")) {
            int droppedID = *(int*)payload->Data;
            Entity2D* droppedEntity = scene->findEntityByID(droppedID);

            if (!droppedEntity) {
                CONSOLE_WARNING("[UI] Drop failed: Entity no longer exists");
            }
            else if (droppedEntity == entity) {
                CONSOLE_WARNING("[UI] Cannot drop entity on itself");
            }
            else if (entity->isDescendantOf(droppedEntity)) {
                CONSOLE_WARNING("[UI] Cannot make parent a child of its own descendant");
            }
            else {
                CONSOLE_INFO("[UI] Reparenting: " + droppedEntity->getName() + " -> " + entity->getName());

                bool isRoot = false;
                for (const auto& rootEntity : scene->getRootEntities()) {
                    if (rootEntity.get() == droppedEntity) {
                        isRoot = true;
                        break;
                    }
                }

                if (isRoot) {

                    auto removed = scene->removeEntity(droppedEntity);
                    if (removed) {
                        entity->addChild(std::move(removed));
                        CONSOLE_SUCCESS("[UI] Reparented root entity to child");

                        scene->selectEntity(droppedEntity);
                        setSelectedEntity(droppedEntity);
                    }
                    else {
                        CONSOLE_ERROR("[UI] Failed to remove entity from root");
                    }
                }
                else {
                    Entity2D* oldParent = droppedEntity->getParent();
                    if (oldParent) {
                        auto removed = oldParent->removeChild(droppedEntity);
                        if (removed) {
                            entity->addChild(std::move(removed));
                            CONSOLE_SUCCESS("[UI] Reparented child entity");

                            // Update selection to the moved entity
                            scene->selectEntity(droppedEntity);
                            setSelectedEntity(droppedEntity);
                        }
                        else {
                            CONSOLE_ERROR("[UI] Failed to remove entity from old parent");
                        }
                    }
                    else {
                        CONSOLE_ERROR("[UI] Entity has no parent but is not a root entity?");
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }


    if (ImGui::BeginPopupContextItem()) {

        if (ImGui::MenuItem("Duplicate")) {
            Entity2D* duplicated = scene->duplicateEntity(entity);
            if (duplicated) {
                scene->selectEntity(duplicated);
                setSelectedEntity(duplicated);
                CONSOLE_SUCCESS("[UI] Duplicated: " + entity->getName());
            }
        }

        if (ImGui::MenuItem("Delete")) {
            int deleteID = entity->getUniqueID();
            Entity2D* toDelete = scene->findEntityByID(deleteID);

            if (toDelete) {
                CONSOLE_INFO("[UI] Queueing for deletion: " + toDelete->getName());

               
                if (s_selectedEntity && s_selectedEntity->getUniqueID() == deleteID) {
                    s_selectedEntity = nullptr;
                }
                scene->deselectAll();

                
                queueEntityForDeletion(toDelete);
            }

            ImGui::CloseCurrentPopup();
        }

        ImGui::Separator();


        if (ImGui::MenuItem("Create Child")) {
            auto newChild = std::make_unique<TestEntity>("Child", sf::Color::Cyan);
            newChild->setPosition(entity->getPosition() + sf::Vector2f(50.0f, 50.0f));
            Entity2D* childPtr = newChild.get();

            entity->addChild(std::move(newChild));

            setSelectedEntity(childPtr);
            scene->selectEntity(childPtr);
            CONSOLE_SUCCESS("[UI] Created child for: " + entity->getName());
        }

        if (entity->getParent()) {
            ImGui::Separator();
            if (ImGui::MenuItem("Detach from Parent")) {
                Entity2D* parent = entity->getParent();
                int entityID = entity->getUniqueID();

                auto removed = parent->removeChild(entity);
                if (removed) {
                    Entity2D* detachedPtr = removed.get();
                    scene->addEntity(std::move(removed));

                    scene->selectEntity(detachedPtr);
                    setSelectedEntity(detachedPtr);

                    CONSOLE_SUCCESS("[UI] Detached: " + detachedPtr->getName() + " from parent");
                }
                else {
                    CONSOLE_ERROR("[UI] Failed to detach entity");
                }
            }
        }
        if (ImGui::MenuItem("Create Parent")) {
            auto newParent = std::make_unique<TestEntity>("Parent", sf::Color(128, 128, 128));
            newParent->setPosition(entity->getPosition());
            Entity2D* parentPtr = newParent.get();

            bool isRoot = false;
            for (const auto& rootEntity : scene->getRootEntities()) {
                if (rootEntity.get() == entity) {
                    isRoot = true;
                    break;
                }
            }

            if (isRoot) {
                auto removed = scene->removeEntity(entity);
                if (removed) {
                    newParent->addChild(std::move(removed));
                    scene->addEntity(std::move(newParent));

                    scene->selectEntity(parentPtr);
                    setSelectedEntity(parentPtr);
                    CONSOLE_SUCCESS("[UI] Created parent for: " + entity->getName());
                }
            }
            else {
            
                Entity2D* oldParent = entity->getParent();
                auto removed = oldParent->removeChild(entity);
                if (removed) {
                    newParent->addChild(std::move(removed));
                    oldParent->addChild(std::move(newParent));

                    scene->selectEntity(parentPtr);
                    setSelectedEntity(parentPtr);
                    CONSOLE_SUCCESS("[UI] Created parent wrapper");
                }
            }
        }

        ImGui::EndPopup();
    }

    if (open) {

        Entity2D* stillThere = scene->findEntityByID(entityID);
        if (stillThere) {

            std::vector<int> childIDs;
            for (const auto& child : stillThere->getChildren()) {
                childIDs.push_back(child->getUniqueID());
            }

            // Render each child by finding it by ID
            for (int childID : childIDs) {
                Entity2D* child = scene->findEntityByID(childID);
                if (child) {
                    renderEntityNode(child);
                }
            }
        }
        ImGui::TreePop();
    }
}

void objectManagerUi::renderTransformWidgetsForEntity(Entity2D* entity) {
    if (!entity) return;

    if (ImGui::TreeNode("Transform")) {
        sf::Vector2f pos = entity->getPosition();
        if (ImGui::DragFloat2("Position", &pos.x, 0.5f)) {
            entity->setPosition(pos);
        }

        float rotation = entity->getRotation().asDegrees();
        if (ImGui::DragFloat("Rotation", &rotation, 1.0f, -360.0f, 360.0f)) {
            entity->setRotation(sf::degrees(rotation));
        }
        sf::Vector2f scale = entity->getScale();
        if (ImGui::DragFloat2("Scale", &scale.x, 0.01f, 0.01f, 10.0f)) {
            entity->setScale(scale);
        }

        int zOrder = entity->getZOrder();
        if (ImGui::DragInt("Z-Order", &zOrder, 1, -100, 100)) {
            entity->setZOrder(zOrder);
        }

        bool visible = entity->isVisible();
        if (ImGui::Checkbox("Visible", &visible)) {
            entity->setVisible(visible);
        }

        ImGui::Separator();

        sf::Vector2f worldPos = entity->getWorldPosition();
        ImGui::Text("World Position: (%.1f, %.1f)", worldPos.x, worldPos.y);

        if (ImGui::Button("Reset Position")) {
            entity->setPosition({ 0.0f, 0.0f });
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Rotation")) {
            entity->setRotation(sf::Angle::Zero);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Scale")) {
            entity->setScale({ 1.0f, 1.0f });
        }

        ImGui::TreePop();
    }
}

// UI/uiObjManager.cpp

void objectManagerUi::queueEntityForDeletion(Entity2D* entity) {
    if (entity) {
        s_deletionQueue.push_back(entity);
        CONSOLE_INFO("[UI] Queued for deletion: " + entity->getName());
    }
}

void objectManagerUi::processDeletionQueue() {
    if (s_deletionQueue.empty()) return;

    auto* scene = SceneManager::instance().getActiveScene();
    if (!scene) {
        s_deletionQueue.clear();
        return;
    }

    for (Entity2D* entity : s_deletionQueue) {
        // Verify entity still exists
        Entity2D* found = scene->findEntityByID(entity->getUniqueID());
        if (found) {
            CONSOLE_INFO("[UI] Processing deletion: " + found->getName());

            // Clear selection if needed
            if (s_selectedEntity && s_selectedEntity->getUniqueID() == found->getUniqueID()) {
                s_selectedEntity = nullptr;
            }

            scene->removeEntity(found);
        }
    }

    s_deletionQueue.clear();
}