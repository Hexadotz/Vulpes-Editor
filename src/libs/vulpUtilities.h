// Vulpes Editor utilities
//This code contains part functions that are used by multiple other files for simplicity sake
#pragma once
#include <string>

namespace EmbeddedData {
	constexpr const char* ENTITY_LIST = R"({
	    "entities": [
        {
          "id": "ent_sprite",
          "display_name": "Sprite",
          "description": "Allows the drawing of 2D images"
        },
        {
          "id": "ent_tile_map",
          "display_name": "TileMap",
          "description": "Entity that allows for creating levels"
        },
        {
          "id": "ent_audio_player",
          "display_name": "AudioPlayer",
          "description": "Entity that plays Audio"
        },
        {
          "id": "ent_light",
          "display_name": "Light",
          "description": "Entity that eluminate the area"
        }]	
    })";
}
