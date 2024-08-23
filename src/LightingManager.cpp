#include "LightingManager.h"
#include <cmath>

const int TILE_SIZE = 96;

LightingManager::LightingManager(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight), 
      lightingTexture(nullptr), mediumLightingTexture(nullptr), largeLightingTexture(nullptr), dimmingTexture(nullptr) {

    // Create the primary, medium, and large light textures
    createLightTexture(100, 0.8f, lightingTexture);       // Primary light circle
    createLightTexture(150, 0.4f, mediumLightingTexture); // Medium circle
    createLightTexture(200, 0.2f, largeLightingTexture);  // Large circle

    // Create the dimming texture (semi-transparent black texture)
    dimmingTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, screenWidth, screenHeight);
    if (!dimmingTexture) {
        SDL_Log("Failed to create dimming texture: %s", SDL_GetError());
        return;
    }
    SDL_SetTextureBlendMode(dimmingTexture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, dimmingTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 140); // Semi-transparent black for less dimming
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL); // Reset to default render target
}

LightingManager::~LightingManager() {
    if (lightingTexture) {
        SDL_DestroyTexture(lightingTexture);
        lightingTexture = nullptr;
    }
    if (mediumLightingTexture) {
        SDL_DestroyTexture(mediumLightingTexture);
        mediumLightingTexture = nullptr;
    }
    if (largeLightingTexture) {
        SDL_DestroyTexture(largeLightingTexture);
        largeLightingTexture = nullptr;
    }
    if (dimmingTexture) {
        SDL_DestroyTexture(dimmingTexture);
        dimmingTexture = nullptr;
    }
}

SDL_Texture* LightingManager::getDimmingTexture() const {
    return dimmingTexture;
}

void LightingManager::createLightTexture(int radius, float intensity, SDL_Texture*& texture) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, radius * 2, radius * 2, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        SDL_Log("Failed to create surface: %s", SDL_GetError());
        return;
    }

    SDL_LockSurface(surface);

    Uint32* pixels = (Uint32*)surface->pixels;
    for (int y = 0; y < radius * 2; ++y) {
        for (int x = 0; x < radius * 2; ++x) {
            int dx = x - radius;
            int dy = y - radius;
            float distance = sqrtf(dx * dx + dy * dy);
            float alpha = (1.0f - (distance / radius)) * intensity;

            if (alpha < 0.0f) alpha = 0.0f;
            if (alpha > 1.0f) alpha = 1.0f;
            Uint8 alphaValue = static_cast<Uint8>(alpha * 255);

            // Use a more yellowish color
            pixels[y * radius * 2 + x] = SDL_MapRGBA(surface->format, 255, 240, 150, alphaValue); // More yellowish light
        }
    }

    SDL_UnlockSurface(surface);

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_FreeSurface(surface);
}

void LightingManager::renderLighting(const SDL_Rect& playerPosition, const std::vector<SDL_Rect>& enemyPositions, const std::vector<SDL_Rect>& spellPositions, const std::vector<std::vector<int>>& dungeonMaze, const SDL_Rect& camera) {
    // Render the dimming texture over the entire screen
    SDL_RenderCopy(renderer, dimmingTexture, NULL, NULL);

    // Render smaller, medium, and large light textures for smoother transition around the player
    SDL_Rect lightRect = {
        playerPosition.x + playerPosition.w / 2 - camera.x - 150, // Center the light on the player
        playerPosition.y + playerPosition.h / 2 - camera.y - 150,
        300, // Width of the light texture
        300  // Height of the light texture
    };
    SDL_RenderCopy(renderer, lightingTexture, NULL, &lightRect);

    SDL_Rect mediumLightRect = {
        playerPosition.x + playerPosition.w / 2 - camera.x - 200, // Center the medium light on the player
        playerPosition.y + playerPosition.h / 2 - camera.y - 200,
        400, // Width of the medium light texture
        400  // Height of the medium light texture
    };
    SDL_RenderCopy(renderer, mediumLightingTexture, NULL, &mediumLightRect);

    SDL_Rect largeLightRect = {
        playerPosition.x + playerPosition.w / 2 - camera.x - 250, // Center the large light on the player
        playerPosition.y + playerPosition.h / 2 - camera.y - 250,
        500, // Width of the large light texture
        500  // Height of the large light texture
    };
    SDL_RenderCopy(renderer, largeLightingTexture, NULL, &largeLightRect);

    // Apply the same logic for each enemy
    for (const auto& enemyPosition : enemyPositions) {
        SDL_Rect enemyLightRect = {
            enemyPosition.x + enemyPosition.w / 2 - 150,
            enemyPosition.y + enemyPosition.h / 2 - 150,
            300, // Width of the light texture
            300  // Height of the light texture
        };
        SDL_RenderCopy(renderer, lightingTexture, NULL, &enemyLightRect);

        SDL_Rect enemyMediumLightRect = {
            enemyPosition.x + enemyPosition.w / 2 - 200,
            enemyPosition.y + enemyPosition.h / 2 - 200,
            400, // Width of the medium light texture
            400  // Height of the medium light texture
        };
        SDL_RenderCopy(renderer, mediumLightingTexture, NULL, &enemyMediumLightRect);

        SDL_Rect enemyLargeLightRect = {
            enemyPosition.x + enemyPosition.w / 2 - 250,
            enemyPosition.y + enemyPosition.h / 2 - 250,
            500, // Width of the large light texture
            500  // Height of the large light texture
        };
        SDL_RenderCopy(renderer, largeLightingTexture, NULL, &enemyLargeLightRect);
    }

    // Render lighting for spells
    for (const auto& spellPosition : spellPositions) {
        SDL_Rect spellLightRect = {
            spellPosition.x + spellPosition.w / 2 - 25,
            spellPosition.y + spellPosition.h / 2 - 25,
            50, // Width of the light texture for spell
            50  // Height of the light texture for spell
        };
        SDL_RenderCopy(renderer, lightingTexture, NULL, &spellLightRect);

        SDL_Rect spellMediumLightRect = {
            spellPosition.x + spellPosition.w / 2 - 50,
            spellPosition.y + spellPosition.h / 2 - 50,
            100, // Width of the medium light texture for spell
            100  // Height of the medium light texture for spell
        };
        SDL_RenderCopy(renderer, mediumLightingTexture, NULL, &spellMediumLightRect);

        SDL_Rect spellLargeLightRect = {
            spellPosition.x + spellPosition.w / 2 - 75,
            spellPosition.y + spellPosition.h / 2 - 75,
            150, // Width of the large light texture for spell
            150  // Height of the large light texture for spell
        };
        SDL_RenderCopy(renderer, largeLightingTexture, NULL, &spellLargeLightRect);
    }
}

