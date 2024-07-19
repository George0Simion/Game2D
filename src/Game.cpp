#include "Game.h"
#include <iostream>

/* Constructor and Destructor */
Game::Game() : window(nullptr), renderer(nullptr), isRunning(false), player(nullptr), world(nullptr) {}

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
    SDL_Texture* tex = loadTexture("/home/simion/Desktop/2/Game2D/assets/sprite_good_arrow2.png");
    player = new Player(width / 2 - 64, height / 2 - 64, tex, 4, 0.1f);     /* Center the player */
    entities.push_back(std::unique_ptr<Entity>(player));

    lastTime = SDL_GetTicks();
    deltaTime = 0.0f;                                                       /* Getting the in-game time for the movement */

    menu = new Menu(this);                                                  /* Allocating memory for the menu */
    world = new World(renderer, 12345);                                     /* Initialize the world with a seed for procedural generation */

    camera = {0, 0, 1680, 900};                                             /* Initialize the camera */

    // Center the camera on the player initially
    camera.x = player->getX() - camera.w / 2;
    camera.y = player->getY() - camera.h / 2;

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
        if (event.type == SDL_QUIT) {
            isRunning = false;
        } else if (isMenuOpen) {
            menu->handleInput(event);
        } else {
            player->handleInput(event);
        }
    }
}

void Game::processInput() {
    if (isMenuOpen) return;

    const Uint8* state = SDL_GetKeyboardState(NULL);
    bool moved = false;

    float speed = player->isRunning() ? 150.0f : 100.0f;

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
        player->setY(player->getY() - speed * deltaTime);
        player->setDirection(Entity::Up);
        moved = true;
    }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
        player->setY(player->getY() + speed * deltaTime);
        player->setDirection(Entity::Down);
        moved = true;
    }
    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        player->setX(player->getX() - speed * deltaTime);
        player->setDirection(Entity::Left);
        moved = true;
    }
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        player->setX(player->getX() + speed * deltaTime);
        player->setDirection(Entity::Right);
        moved = true;
    }

    if (moved) {
        if (player->getAction() != Entity::Shooting && player->getAction() != Entity::Thrusting) {
            player->setAction(Entity::Walking);
            player->startAnimation();
        }
    } else {
        player->stopAnimation();
    }
}

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f; /* Convert milliseconds to seconds */
    lastTime = currentTime;

    // Process input
    processInput();

    // Update entities
    for (auto& entity : entities) {
        entity->update(deltaTime); // This is where Entity::update is called
    }

    // Update the camera position
    float playerX = player->getX();
    float playerY = player->getY();

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
    world->render(camera.x + camera.w / 2 + 192, camera.y + camera.h / 2 - 256);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (const auto& entity : entities) {
        SDL_Rect srcRect = entity->getCurrentFrame();
        SDL_Rect destRect = { 
            static_cast<int>(entity->getX()) - camera.x, 
            static_cast<int>(entity->getY()) - camera.y, 
            static_cast<int>(srcRect.w * 2),  
            static_cast<int>(srcRect.h * 2)  
        };
        SDL_RenderCopy(renderer, entity->getTex(), &srcRect, &destRect);

        if (entity->isArrowActive()) {
            SDL_Rect arrowSrcRect = entity->getArrowFrame();
            SDL_Rect arrowDestRect = {
                static_cast<int>(entity->getArrowX()) - camera.x,
                static_cast<int>(entity->getArrowY()) - camera.y,
                64,
                64
            };
            SDL_RenderCopy(renderer, entity->getTex(), &arrowSrcRect, &arrowDestRect);
        }
    }

    if (isMenuOpen) {
        menu->render();
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(spriteSheet);
    delete menu;
    delete world;
    SDL_Quit();
}
