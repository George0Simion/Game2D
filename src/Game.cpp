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
        renderer = SDL_CreateRenderer(window, -1, 0);                       /* Creating the window and the renderer */
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);           /* Drawing the window */
        }
        isRunning = true;
    } else {
        isRunning = false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {                         /* Check error for the image */
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        isRunning = false;
    }

    /* Loading the main character and adding it to the vector */
    SDL_Texture* tex = loadTexture("/home/simion/Desktop/2/Game2D/assets/maybe.png");
    entities.push_back(Entity(width / 2 - 64, height / 2 - 64, tex));       /* Center the player */

    lastTime = SDL_GetTicks();
    deltaTime = 0.0f;                                                       /* Getting the in-game time for the movement */

    menu = new Menu(this);                                                  /* Allocating memory for the menu */
    world = new World(renderer, 12345);                                     /* Initialize the world with a seed for procedural generation */

    camera = {0, 0, 1680, 900};                                             /* Initialize the camera */

    // Center the camera on the player initially
    camera.x = entities[0].getX() - camera.w / 2;
    camera.y = entities[0].getY() - camera.h / 2;

    // Ensure chunks are generated and rendered correctly around the initial camera position
    world->update(camera.x + camera.w / 2, camera.y + camera.h / 2);
}

SDL_Texture* Game::loadTexture(const char* fileName) {                      /* Method for loading the texture for the main character */
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
}

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f; /* Convert milliseconds to seconds */
    lastTime = currentTime;

    // Process input
    processInput();

    // Update the camera position
    Entity& player = entities[0];
    float playerX = player.getX();
    float playerY = player.getY();

    // Define the dead zone dimensions
    int deadZoneWidth = 1680;
    int deadZoneHeight = 900;

    // Calculate the camera position based on the dead-zone
    float camX = camera.x + camera.w / 2;
    float camY = camera.y + camera.h / 2;

    float deadZoneLeft = camX - deadZoneWidth / 2;
    float deadZoneRight = camX + deadZoneWidth / 2;
    float deadZoneTop = camY - deadZoneHeight / 2;
    float deadZoneBottom = camY + deadZoneHeight / 2;

    if (playerX < deadZoneLeft) {
        camera.x = playerX - (camera.w - deadZoneWidth) / 2; // good
    }
    if (playerX > deadZoneRight) {
        camera.x = playerX - (camera.w + deadZoneWidth) / 2; // good
    }
    if (playerY < deadZoneTop) {
        camera.y = playerY - (camera.h - deadZoneHeight) / 2; // good
    }
    if (playerY > deadZoneBottom) {
        camera.y = playerY - (camera.h + deadZoneHeight) / 2; // good
    }

    // Ensure the dead zone moves with the camera
    camX = camera.x + camera.w / 2;
    camY = camera.y + camera.h / 2;

    deadZoneLeft = camX - deadZoneWidth / 2;
    deadZoneRight = camX + deadZoneWidth / 2;
    deadZoneTop = camY - deadZoneHeight / 2;
    deadZoneBottom = camY + deadZoneHeight / 2;

    // Update the world based on the new camera position
    world->update(camera.x + camera.w / 2, camera.y + camera.h / 2);
}

void Game::render() {
    SDL_RenderClear(renderer);

    // Render the world
    Entity& player = entities[0];
    world->render(camera.x + camera.w / 2 + 192, camera.y + camera.h / 2 - 256);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color

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

    if (isMenuOpen) {
        menu->render();
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
