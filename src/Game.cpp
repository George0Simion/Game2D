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
    SDL_Texture* tex = loadTexture("/home/simion/Desktop/2/Game2D/assets/sprite_good_arrow2.png");
    entities.push_back(Entity(width / 2 - 64, height / 2 - 64, tex, 4, 0.1f));       /* Center the player */

    lastTime = SDL_GetTicks();
    deltaTime = 0.0f;                                                       /* Getting the in-game time for the movement */

    menu = new Menu(this);                                                  /* Allocating memory for the menu */
    world = new World(renderer, 12345);                                     /* Initialize the world with a seed for procedural generation */

    camera = {0, 0, 1680, 900};                                             /* Initialize the camera */

    // Center the camera on the player initially
    camera.x = entities[0].getX() - camera.w / 2;
    camera.y = entities[0].getY() - camera.h / 2;

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
                    } else if (event.key.keysym.sym == SDLK_e) {
                        if (!entities[0].isMoving()) {
                            entities[0].setAction(Entity::Slashing);
                            entities[0].startAnimation();
                        }
                    } else if (event.key.keysym.sym == SDLK_q) {
                        if (!entities[0].isMoving()) {
                            entities[0].setAction(Entity::Spellcasting);
                            entities[0].startAnimation();
                        }
                    } else if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
                        entities[0].setRunning(true);
                    }
                    break;
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
                        entities[0].setRunning(false);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        entities[0].setAction(Entity::Shooting);
                        entities[0].startAnimation();
                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        entities[0].setAction(Entity::Thrusting);
                        entities[0].startAnimation();
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (entities[0].getAction() == Entity::Shooting) {
                            entities[0].stopAnimation();
                            entities[0].shootArrow(entities[0].getDirection());

                            switch (entities[0].getDirection()) {
                                case Entity::Up:
                                    entities[0].setAction(Entity::ArrowFlyingUp);
                                    break;
                                case Entity::Down:
                                    entities[0].setAction(Entity::ArrowFlyingDown);
                                    break;
                                case Entity::Left:
                                    entities[0].setAction(Entity::ArrowFlyingLeft);
                                    break;
                                case Entity::Right:
                                    entities[0].setAction(Entity::ArrowFlyingRight);
                                    break;
                            }
                            entities[0].startAnimation();
                        }

                    } else if (event.button.button == SDL_BUTTON_RIGHT) {
                        if (entities[0].getAction() == Entity::Thrusting) {
                            entities[0].stopAnimation();
                            entities[0].setAction(Entity::Walking);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void Game::processInput() {
    if (isMenuOpen) return;

    const Uint8* state = SDL_GetKeyboardState(NULL);
    Entity& player = entities[0];

    float speed = player.isRunning() ? 150.0f : 100.0f;
    bool moved = false;

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
        player.setY(player.getY() - speed * deltaTime);
        player.setDirection(Entity::Up);
        moved = true;
    }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
        player.setY(player.getY() + speed * deltaTime);
        player.setDirection(Entity::Down);
        moved = true;
    }
    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        player.setX(player.getX() - speed * deltaTime);
        player.setDirection(Entity::Left);
        moved = true;
    }
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        player.setX(player.getX() + speed * deltaTime);
        player.setDirection(Entity::Right);
        moved = true;
    }

    int mouseX, mouseY;
    Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        if (player.getAction() != Entity::Shooting) {
            player.setAction(Entity::Shooting);
            player.startAnimation();
        }
    } else if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        if (player.getAction() != Entity::Thrusting) {
            player.setAction(Entity::Thrusting);
            player.startAnimation();
        }
    }

    if (!moved && player.getAction() == Entity::Walking) {
        player.stopAnimation();
    } else if (moved) {
        if (player.getAction() != Entity::Shooting && player.getAction() != Entity::Thrusting) {
            player.startAnimation();
            player.setAction(Entity::Walking);
        }
    }
}

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f; /* Convert milliseconds to seconds */
    lastTime = currentTime;

    // Process input
    processInput();

    // Update entities
    for (Entity& entity : entities) {
        entity.update(deltaTime); // This is where Entity::update is called
    }

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
    world->render(camera.x + camera.w / 2 + 192, camera.y + camera.h / 2 - 256);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (Entity& e : entities) {
        SDL_Rect srcRect = e.getCurrentFrame();
        SDL_Rect destRect = { 
            static_cast<int>(e.getX()) - camera.x, 
            static_cast<int>(e.getY()) - camera.y, 
            static_cast<int>(srcRect.w * 2),  
            static_cast<int>(srcRect.h * 2)  
        };
        SDL_RenderCopy(renderer, e.getTex(), &srcRect, &destRect);

        if (e.isArrowActive()) {
            SDL_Rect arrowSrcRect = e.getArrowFrame();
            SDL_Rect arrowDestRect = {
                static_cast<int>(e.getArrowX()) - camera.x,
                static_cast<int>(e.getArrowY()) - camera.y,
                64,
                64
            };
            SDL_RenderCopy(renderer, e.getTex(), &arrowSrcRect, &arrowDestRect);
        }
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
    SDL_DestroyTexture(spriteSheet);
    delete menu;
    delete world;
    SDL_Quit();
}
