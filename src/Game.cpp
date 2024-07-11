#include "Game.h"

Game::Game() : window(nullptr), renderer(nullptr), isRunning(false) {}

Game::~Game() {
    clean();
}

void Game::init(const char* title, int width, int height, bool fullscreen) {
    int flags = fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        isRunning = true;
    } else {
        isRunning = false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        isRunning = false;
    }

    // Load texture
    SDL_Texture* tex = loadTexture("/home/simion/Desktop/Game/assets/sper.png");
    entities.push_back(Entity(100, 100, tex));

    lastTime = SDL_GetTicks();
    deltaTime = 0.0f;

    menu = new Menu(this);
}

SDL_Texture* Game::loadTexture(const char* fileName) {
    SDL_Surface* tempSurface = IMG_Load(fileName);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return tex;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (isMenuOpen) {
            menu->handleInput();
        } else {
            switch (event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        isMenuOpen = true;
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);  // Dim the screen
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void Game::processInput() {
    if (isMenuOpen) return;  // Ignore game input when menu is open

    const Uint8* state = SDL_GetKeyboardState(NULL);
    Entity& player = entities[0];

    float speed = 100.0f;  // Movement speed in pixels per second

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
        player.setY(player.getY() - speed * deltaTime);  // Move up
    }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
        player.setY(player.getY() + speed * deltaTime);  // Move down
    }
    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        player.setX(player.getX() - speed * deltaTime);  // Move left
    }
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        player.setX(player.getX() + speed * deltaTime);  // Move right
    }

    // Keep entity within the window bounds (assuming window size 800x600)
    if (player.getX() < 0) player.setX(0);
    if (player.getX() > 800 - player.getCurrentFrame().w) player.setX(800 - player.getCurrentFrame().w);
    if (player.getY() < 0) player.setY(0);
    if (player.getY() > 600 - player.getCurrentFrame().h) player.setY(600 - player.getCurrentFrame().h);
}


void Game::update() {
    /*
        Movement Update: If you have additional logic for smooth or continuous movement, it could go here.
        Collision Detection: Check for collisions between entities or between an entity and the boundaries of the screen.
        Game State Updates: Update scores, check for win/lose conditions, etc.

        Movement based on time rather than per-frame 
        (also known as time-based movement or frame-independent movement) ensures that your game runs smoothly regardless of the frame rate.
    */

    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f;  // Convert milliseconds to seconds
    lastTime = currentTime;

    // Process input
    processInput();
}

void Game::render() {
    SDL_RenderClear(renderer);

    for (Entity& e : entities) {
        SDL_Rect srcRect = e.getCurrentFrame();
        SDL_Rect destRect = { static_cast<int>(e.getX()), static_cast<int>(e.getY()), srcRect.w, srcRect.h };
        SDL_RenderCopy(renderer, e.getTex(), &srcRect, &destRect);
    }

    if (isMenuOpen) {
        menu->render();
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    delete menu;
    SDL_Quit();
}
