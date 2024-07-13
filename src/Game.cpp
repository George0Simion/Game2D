#include "Game.h"
#include <iostream>

/* Constructor and Destructor */
Game::Game() : window(nullptr), renderer(nullptr), isRunning(false), world(nullptr) {}

Game::~Game() {
    clean();
}

void Game::init(const char* title, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, 0); /* Creating the window and the renderer */
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); /* Drawing the window */
        }
        isRunning = true;
    } else {
        isRunning = false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { /* Check error for the image */
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        isRunning = false;
    }

    /* Loading the main character and adding it to the vector */
    SDL_Texture* tex = loadTexture("/home/simion/Desktop/2/Game2D/assets/maybe.png");
    entities.push_back(Entity(width / 2 - 64, height / 2 - 64, tex));  // Center the player

    lastTime = SDL_GetTicks();
    deltaTime = 0.0f; /* Getting the in-game time for the movement */

    menu = new Menu(this); /* Allocating memory for the menu */

    world = new World(renderer); /* Initialize and load the world map */
    world->loadMap("/home/simion/Desktop/2/Game2D/src/map.json"); /* Adjust the path if necessary */

    camera = {0, 0, 1300, 1000}; /* Initialize the camera */
}

SDL_Texture* Game::loadTexture(const char* fileName) { /* Method for loading the texture for the main character */
    SDL_Surface* tempSurface = IMG_Load(fileName);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return tex;
}

/* Handling the events of the game such as the opening of the menu, if the game is running or not
   and some more are coming  */
void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (isMenuOpen) {
            menu->handleInput(event);
        } else {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        isMenuOpen = !isMenuOpen;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void Game::processInput() {
    if (isMenuOpen) return; /* Ignore game input when menu is open */

    const Uint8* state = SDL_GetKeyboardState(NULL);
    Entity& player = entities[0];

    float speed = 100.0f; /* Movement speed in pixels per second */

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
        player.setY(player.getY() - speed * deltaTime); /* Move up */
    }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
        player.setY(player.getY() + speed * deltaTime); /* Move down */
    }
    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        player.setX(player.getX() - speed * deltaTime); /* Move left */
    }
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        player.setX(player.getX() + speed * deltaTime); /* Move right */
    }

    // Keep entity within the window bounds (assuming window size 1600x1200)
    if (player.getX() < 0) player.setX(0);
    if (player.getX() > 1600 - player.getCurrentFrame().w) player.setX(1600 - player.getCurrentFrame().w);
    if (player.getY() < 0) player.setY(0);
    if (player.getY() > 1200 - player.getCurrentFrame().h) player.setY(1200 - player.getCurrentFrame().h);
}

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f; /* Convert milliseconds to seconds */
    lastTime = currentTime;

    // Process input
    processInput();

    // Update the camera position
    Entity& player = entities[0];
    int playerX = static_cast<int>(player.getX());
    int playerY = static_cast<int>(player.getY());

    // Define the dead zone
    int deadZoneMarginX = 48;
    int deadZoneMarginY = 48;
    int deadZoneWidth = camera.w - 2 * deadZoneMarginX;
    int deadZoneHeight = camera.h - 2 * deadZoneMarginY;

    int deadZoneLeft = camera.x + (camera.w - deadZoneWidth) / 2;
    int deadZoneRight = camera.x + (camera.w + deadZoneWidth) / 2;
    int deadZoneTop = camera.y + (camera.h - deadZoneHeight) / 2;
    int deadZoneBottom = camera.y + (camera.h + deadZoneHeight) / 2;

    if (playerX < deadZoneLeft) {
        camera.x = playerX - (camera.w - deadZoneWidth) / 2;
    }
    if (playerX > deadZoneRight) {
        camera.x = playerX - (camera.w + deadZoneWidth) / 2;
    }
    if (playerY < deadZoneTop) {
        camera.y = playerY - (camera.h - deadZoneHeight) / 2;
    }
    if (playerY > deadZoneBottom) {
        camera.y = playerY - (camera.h + deadZoneHeight) / 2;
    }

    // Prevent the camera from going out of bounds
    if (camera.x < 0) camera.x = 0;
    if (camera.y < 0) camera.y = 0;
    if (camera.x > 1600 - camera.w) camera.x = 1600 - camera.w;
    if (camera.y > 1200 - camera.h) camera.y = 1200 - camera.h;
}

/* Rendering method for the entities */
void Game::render() {
    // Clear the screen with white color
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    SDL_RenderClear(renderer);

    // Render the world
    Entity& player = entities[0];
    world->render(camera.x, camera.y);

    // Render entities
    for (Entity& e : entities) {
        SDL_Rect srcRect = e.getCurrentFrame();
        SDL_Rect destRect = { 
            static_cast<int>(e.getX()) - camera.x, 
            static_cast<int>(e.getY()) - camera.y, 
            srcRect.w, 
            srcRect.h 
        };
        SDL_RenderCopy(renderer, e.getTex(), &srcRect, &destRect);
    }

    SDL_RenderPresent(renderer);
}

/* Destroying everything */
void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    delete menu;
    delete world;
    SDL_Quit();
}
