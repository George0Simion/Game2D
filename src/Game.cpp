#include "Game.h"
#include <iostream>

/* Constructor and Destructor */
Game::Game() : window(nullptr), renderer(nullptr), isRunning(false), player(nullptr), world(nullptr), mazeGenerator(nullptr), difficulty(0), pathfindingManager() {}

Game::~Game() {
    // Set terminate flag for all threads
    {
        std::lock_guard<std::mutex> dungeonLock(dungeonMutex);
        std::lock_guard<std::mutex> lightingLock(lightingMutex);
        std::lock_guard<std::mutex> pathfindingLock(pathfindingMutex);
        std::lock_guard<std::mutex> playerEnemyActionLock(playerEnemyActionMutex);
        terminateThreads = true;
    }

    // Notify all threads to exit
    dungeonCv.notify_one();
    lightingCv.notify_one();
    pathfindingCv.notify_one();
    playerEnemyActionCv.notify_one();

    // Join the dungeon generation thread
    if (dungeonThreadHandle.joinable()) {
        dungeonThreadHandle.join();
    }

    // Join the lighting thread
    if (lightingThreadHandle.joinable()) {
        lightingThreadHandle.join();
    }

    // Join the pathfinding thread
    if (pathfindingThreadHandle.joinable()) {
        pathfindingThreadHandle.join();
    }

    // Join the player and enemy action thread
    if (playerEnemyActionThreadHandle.joinable()) {
        playerEnemyActionThreadHandle.join();
    }

    // Clean up game resources
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

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        isRunning = false;
    } else {
        font = TTF_OpenFont("/home/simion/Desktop/5/Game2D/assets/font.ttf", 32); // Adjust the path and size as needed
        if (!font) {
            printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
            isRunning = false;
        }

        smallFont = TTF_OpenFont("/home/simion/Desktop/5/Game2D/assets/font.ttf", 16); // Adjust the path and size as needed
        if (!smallFont) {
            printf("Failed to load small font! SDL_ttf Error: %s\n", TTF_GetError());
            isRunning = false;
        }
    }

    /* Loading the main character and adding it to the vector */
    SDL_Texture* tex = loadTexture("/home/simion/Desktop/5/Game2D/assets/sprite_good_arrow3.png");
    player = new Player(670, 2850, tex, 4, 0.1f);
    entities.push_back(std::unique_ptr<Entity>(player));
    player->setHealth(Player::INITIAL_HEALTH);                                                 /* Set the health of the player */

    lastTime = SDL_GetTicks();
    deltaTime = 0.0f;                                                       /* Getting the in-game time for the movement */

    menu = new Menu(this);                                                  /* Allocating memory for the menu */
    world = new World(renderer);                                     /* Initialize the world with a seed for procedural generation */

    isPlayerInDungeon = false;

    std::pair<int, int> entrancePos = findDungeonEntrancePosition();
    int tileSize = 96; // The size of each tile in pixels

    // Check if the entrance position is valid
    if (entrancePos.first != -1 && entrancePos.second != -1) {
        dungeonEntrance = {
            entrancePos.first * tileSize,
            entrancePos.second * tileSize - 180,
            tileSize, // Width of the dungeon entrance
            tileSize  // Height of the dungeon entrance
        };

        // Set dungeonExit to the same coordinates
        dungeonExit = dungeonEntrance;
    }

    lightingManager = new LightingManager(renderer, 1920, 1080);
    camera = {0, 0, 1680, 900};                                             /* Initialize the camera */

    // Center the camera on the player initially
    camera.x = player->getX() - camera.w / 2;
    camera.y = player->getY() - camera.h / 2;

    world->update(camera.x + camera.w / 2, camera.y + camera.h / 2);

    tilesetTexture = loadTexture("/home/simion/Desktop/5/Game2D/assets/tileset.png");
    dungeonEntranceTexture = loadTexture("/home/simion/Desktop/5/Game2D/assets/dungeon_entrance.png");
    pathTileTexture = loadTexture("/home/simion/Desktop/5/Game2D/assets/cobblestone_2.png");
    wallTileTexture = loadTexture("/home/simion/Desktop/5/Game2D/assets/Brickwall_Texture.png");
    loadHUDTexture();                                                        /* Load the HUD texture */

    terminateThreads = false;

    dungeonThreadHandle = std::thread(&Game::dungeonGenerationThread, this);
    lightingThreadHandle = std::thread(&Game::lightingThread, this);
    pathfindingThreadHandle = std::thread(&Game::pathfindingThread, this);
    playerEnemyActionThreadHandle = std::thread(&Game::playerEnemyActionThread, this);
}

void Game::dungeonGenerationThread() {
    while (true) {
        std::unique_lock<std::mutex> lock(dungeonMutex);
        dungeonCv.wait(lock, [this] { return terminateThreads; });

        if (terminateThreads) break;

        // Generate dungeon chunks asynchronously
        world->update(camera.x + camera.w / 2, camera.y + camera.h / 2);
    }
}

void Game::lightingThread() {
    while (true) {
        std::unique_lock<std::mutex> lock(lightingMutex);
        lightingCv.wait(lock, [this] { return terminateThreads; });

        if (terminateThreads) break;

        std::vector<SDL_Rect> enemyPositions;
        std::vector<SDL_Rect> spellPositions;

        {
            // Ensure thread-safe access to entities when calculating positions
            std::lock_guard<std::mutex> entityLock(entityMutex);

            for (const auto& entity : entities) {
                if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                    SDL_Rect enemyRect = {
                        static_cast<int>(enemy->getX()) - camera.x,
                        static_cast<int>(enemy->getY()) - camera.y,
                        static_cast<int>(enemy->getCurrentFrame().w * 2),
                        static_cast<int>(enemy->getCurrentFrame().h * 2)
                    };
                    enemyPositions.push_back(enemyRect);
                }

                if (entity->isSpellActive()) {
                    SDL_Rect spellRect = {
                        static_cast<int>(entity->getSpellX()) - camera.x,
                        static_cast<int>(entity->getSpellY()) - camera.y,
                        64, // Width of the spell texture
                        64  // Height of the spell texture
                    };
                    spellPositions.push_back(spellRect);
                }
            }
        }

        // Update lighting effects asynchronously
        lightingManager->renderLighting(
            {static_cast<int>(player->getX()), static_cast<int>(player->getY()), player->getCurrentFrame().w * 2, player->getCurrentFrame().h * 2},
            enemyPositions,
            spellPositions,
            dungeonMaze,
            camera
        );
    }
}


void Game::pathfindingThread() {
    while (true) {
        std::unique_lock<std::mutex> lock(pathfindingMutex);
        pathfindingCv.wait(lock, [this] { return terminateThreads; });

        if (terminateThreads) break;

        // Handle pathfinding requests asynchronously
        for (auto& entity : entities) {
            if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                // Get path to player
                auto path = pathfindingManager.getSharedPathToPlayer(*player, *this, *enemy);
                
                // Update the enemy's path directly
                enemy->pathToPlayer = path;
                enemy->currentPathIndex = 0;  // Reset path index to start at the beginning

                // Follow the path
                enemy->followSharedPath(deltaTime, *player, *this);
            }
        }
    }
}

void Game::playerEnemyActionThread() {
    while (true) {
        std::unique_lock<std::mutex> lock(playerEnemyActionMutex);
        playerEnemyActionCv.wait(lock, [this] { return terminateThreads; });

        if (terminateThreads) break;

        // Process player actions
        if (!isMenuOpen && !player->getIsDead()) {
            player->update(deltaTime, entities, *this);
        }

        // Process enemy actions
        for (auto& entity : entities) {
            if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                enemy->updateBehavior(deltaTime, *player, entities, *this);
            }
        }
    }
}

std::pair<int, int> Game::findDungeonEntrancePosition() {
    for (int y = 0; y < mapMatrix.size(); ++y) {
        for (int x = 0; x < mapMatrix[y].size(); ++x) {
            if (mapMatrix[y][x] == TILE_DUNGEON_ENTRANCE) {
                return {x, y};  // Return the matrix coordinates of the dungeon entrance
            }
        }
    }
    return {-1, -1};  // Return an invalid position if no dungeon entrance is found
}

SDL_Texture* Game::loadTexture(const char* fileName) {                      /* Method for loading the texture for the main character */
    SDL_Surface* tempSurface = IMG_Load(fileName);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return tex;
}

void Game::spawnEnemy() {
    SDL_Texture* enemyTex = loadTexture("/home/simion/Desktop/5/Game2D/assets/enemy4.png");
    float x = 540;
    float y = 100;
    
    // Remove or adapt the PathfindingManager reference as needed.
    auto enemy = std::make_unique<Enemy>(x, y, enemyTex, 8, 0.1f, pathfindingManager);
    enemy->setHealth(Enemy::INITIAL_HEALTH);
    entities.push_back(std::move(enemy));
}

void Game::resetGame(bool resetDungeon) {
    // Clear all entities, including enemies
    for (auto& entity : entities) {
        entity.reset();
    }
    entities.clear();

    // Reinitialize the player
    player = new Player(670, 2850, loadTexture("/home/simion/Desktop/5/Game2D/assets/sprite_good_arrow3.png"), 4, 0.1f);
    player->setHealth(Player::INITIAL_HEALTH);
    entities.push_back(std::unique_ptr<Entity>(player));

    // Reset the world
    world = new World(renderer);

    // Reset the camera position
    camera = {0, 0, 1680, 900};
    camera.x = player->getX() - camera.w / 2;
    camera.y = player->getY() - camera.h / 2;

    world->update(camera.x + camera.w / 2, camera.y + camera.h / 2);

    isMenuOpen = false;
    isRunning = true;

    if (resetDungeon) {
        // Reset dungeon state
        isPlayerInDungeon = false;
        difficulty = 0;
        if (mazeGenerator) {
            delete mazeGenerator;
            mazeGenerator = nullptr;
        }
    }
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
            // Process game-specific inputs first
            switch (event.type) {
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (isMenuOpen) {
                            menu->setState(Menu::NONE);
                            isMenuOpen = false;
                        } else {
                            menu->setState(Menu::MAIN_MENU);
                            isMenuOpen = true;
                        }
                    } else {
                        player->handleInput(event);  // Pass other key events to player
                    }
                    break;
                case SDL_KEYUP:
                    player->handleInput(event);  // Handle key release events
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    player->handleInput(event);  // Pass mouse events to player
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
    bool moved = false;

    float speed = player->isRunning() ? 150.0f : 100.0f;
    float newX = player->getX();
    float newY = player->getY();

    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
        newY -= speed * deltaTime;
        player->setDirection(Entity::Up);
        moved = true;
    }
    if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
        newY += speed * deltaTime;
        player->setDirection(Entity::Down);
        moved = true;
    }
    if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
        newX -= speed * deltaTime;
        player->setDirection(Entity::Left);
        moved = true;
    }
    if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
        newX += speed * deltaTime;
        player->setDirection(Entity::Right);
        moved = true;
    }

    if (moved) {
        // Wall collision handling for both inside and outside the dungeon
        SDL_Rect playerRect = player->getBoundingBox();
        float playerLeft = newX;
        float playerRight = newX + playerRect.w - 8;
        float playerTop = newY + 16;
        float playerBottom = newY + playerRect.h;

        bool collision = isWall(playerLeft, playerTop) || 
                         isWall(playerRight, playerTop) ||
                         isWall(playerLeft, playerBottom) ||
                         isWall(playerRight, playerBottom);

        if (!collision) {
            player->setX(newX);
            player->setY(newY);
        } else {
            // Apply sliding effect
            if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP]) {
                if (!isWall(playerLeft - 1, playerTop) && !isWall(playerLeft - 1, playerBottom)) {
                    player->setX(player->getX() - 1);
                } else if (!isWall(playerRight + 1, playerTop) && !isWall(playerRight + 1, playerBottom)) {
                    player->setX(player->getX() + 1);
                } else {
                    player->setY(player->getY());
                }
            } else if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN]) {
                if (!isWall(playerLeft - 1, playerBottom) && !isWall(playerLeft - 1, playerTop)) {
                    player->setX(player->getX() - 1);
                } else if (!isWall(playerRight + 1, playerBottom) && !isWall(playerRight + 1, playerTop)) {
                    player->setX(player->getX() + 1);
                } else {
                    player->setY(player->getY());
                }
            } else if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT]) {
                if (!isWall(playerLeft, playerTop - 1) && !isWall(playerRight, playerTop - 1)) {
                    player->setY(player->getY() - 1);
                } else if (!isWall(playerLeft, playerBottom + 1) && !isWall(playerRight, playerBottom + 1)) {
                    player->setY(player->getY() + 1);
                } else {
                    player->setX(player->getX());
                }
            } else if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT]) {
                if (!isWall(playerRight, playerTop - 1) && !isWall(playerLeft, playerTop - 1)) {
                    player->setY(player->getY() - 1);
                } else if (!isWall(playerRight, playerBottom + 1) && !isWall(playerLeft, playerBottom + 1)) {
                    player->setY(player->getY() + 1);
                } else {
                    player->setX(player->getX());
                }
            }
        }

        if (player->getAction() != Entity::Shooting && player->getAction() != Entity::Thrusting) {
            player->setAction(Entity::Walking);
            player->startAnimation();
        }
    } else {
        player->stopAnimation();
    }
}

void Game::updateEnemySpellAnimation(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) {
    for (const auto& entity : entities) {
        if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
            if (enemy->getAction() == Entity::Spellcasting && !enemy->isSpellActive()) {
                enemy->setSpellTarget(player->getX(), player->getY());
            }
            enemy->updateSpellPosition(deltaTime, entities, *this);

            // Ensure the enemy's texture is restored after spellcasting
            if (enemy->getAction() == Entity::Spellcasting && !enemy->isSpellActive()) {
                enemy->setAction(Entity::Walking); // Set back to walking after spellcasting
            }
        }
    }
}

bool Game::checkDungeonEntrance() {
    SDL_Rect playerRect = {static_cast<int>(player->getX()), static_cast<int>(player->getY()), 64, 64};
    return SDL_HasIntersection(&playerRect, &dungeonEntrance);
}

void Game::startLevel(int difficulty) {
    if (mazeGenerator) {
        delete mazeGenerator;
    }

    int mazeWidth = 21 + difficulty;
    int mazeHeight = 21 + difficulty;

    mazeGenerator = new MazeGenerator(mazeWidth, mazeHeight);
    dungeonMaze = mazeGenerator->generateMaze();

    // Set specific positions for entrance and exit
    int entranceX = 1; // For example, fixed entrance point at (1, 1)
    int entranceY = 1;
    int exitX = mazeWidth - 2; // Fixed exit point at bottom right corner
    int exitY = mazeHeight - 2;

    dungeonMaze[entranceY][entranceX] = 2; // 2 represents entrance
    dungeonMaze[exitY][exitX] = 3; // 3 represents exit

    int cellSize = 96;

    // Set dungeon entrance and exit coordinates
    dungeonEntrance = {entranceX * cellSize, entranceY * cellSize, cellSize, cellSize};
    dungeonExit = {exitX * cellSize, exitY * cellSize, cellSize, cellSize};

    player->setX(dungeonEntrance.x + 64); // Center player in the entrance tile
    player->setY(dungeonEntrance.y + 64);

    // Adjust the camera to center the player
    camera.x = player->getX() - camera.w / 2 + cellSize / 2;
    camera.y = player->getY() - camera.h / 2 + cellSize / 2;

    // Ensure the camera doesn't go out of bounds
    int mazePixelWidth = mazeWidth * cellSize;
    int mazePixelHeight = mazeHeight * cellSize;
    camera.x = std::max(0, std::min(camera.x, mazePixelWidth - camera.w));
    camera.y = std::max(0, std::min(camera.y, mazePixelHeight - camera.h));

    spawnEnemiesInDungeon(difficulty + 1);
}

void Game::spawnEnemiesInDungeon(int numberOfEnemies) {
    std::vector<std::pair<int, int>> pathCells;

    for (int y = 0; y < dungeonMaze.size(); ++y) {
        for (int x = 0; x < dungeonMaze[y].size(); ++x) {
            if (dungeonMaze[y][x] == 0) {
                pathCells.push_back({x, y});
            }
        }
    }

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::random_shuffle(pathCells.begin(), pathCells.end());

    for (int i = 0; i < numberOfEnemies && i < pathCells.size(); ++i) {
        int x = pathCells[i].first;
        int y = pathCells[i].second;

        float enemyX = x * 96.0f - 32;
        float enemyY = y * 96.0f - 64;

        SDL_Texture* enemyTex = loadTexture("/home/simion/Desktop/3/Game2D/assets/enemy4.png");
        auto enemy = std::make_unique<Enemy>(enemyX, enemyY, enemyTex, 8, 0.1f, pathfindingManager);
        enemy->setHealth(Enemy::INITIAL_HEALTH);
        entities.push_back(std::move(enemy));
    }
}

void Game::enterDungeon() {
    lastPlayerX = player->getX();
    lastPlayerY = player->getY();
    isPlayerInDungeon = true;
    startLevel(0);
    
    // Re-initialize the dungeon entrance coordinates to ensure consistency
    std::pair<int, int> entrancePos = findDungeonEntrancePosition();
    int tileSize = 96;
    if (entrancePos.first != -1 && entrancePos.second != -1) {
        dungeonEntrance = {
            entrancePos.first * tileSize,
            entrancePos.second * tileSize - 180,
            tileSize,
            tileSize
        };
    }
}

void Game::transitionToNextLevel() {
    difficulty++;
    startLevel(difficulty);
}

bool Game::checkDungeonExit() {
    SDL_Rect playerRect = {static_cast<int>(player->getX()), static_cast<int>(player->getY()), 64, 64};
    return SDL_HasIntersection(&playerRect, &dungeonExit);
}

bool Game::checkNextLevelDoor() {
    SDL_Rect playerRect = {static_cast<int>(player->getX()), static_cast<int>(player->getY()), 64, 64};
    SDL_Rect nextLevelRect = {dungeonExit.x, dungeonExit.y, dungeonExit.w - 32, dungeonExit.h - 32}; // Adjusted for the next level door
    return SDL_HasIntersection(&playerRect, &nextLevelRect);
}

void Game::exitDungeon() {
    isPlayerInDungeon = false;
    difficulty = 0; // Reset difficulty

    // Restore player to the last position before entering the dungeon
    player->setX(lastPlayerX);
    player->setY(lastPlayerY);

    // Correct player position relative to the outside entrance
    if (player->getX() > dungeonEntrance.x) {
        player->setX(player->getX() + 64);
    } else {
        player->setX(player->getX() - 64);
    }

    if (player->getY() > dungeonEntrance.y) {
        player->setY(player->getY() + 64);
    } else {
        player->setY(player->getY() - 64);
    }

    delete mazeGenerator;
    mazeGenerator = nullptr;

    // Reset entrance position to ensure consistency
    std::pair<int, int> entrancePos = findDungeonEntrancePosition();
    int tileSize = 96;
    if (entrancePos.first != -1 && entrancePos.second != -1) {
        dungeonEntrance = {
            entrancePos.first * tileSize,
            entrancePos.second * tileSize - 180,
            tileSize,
            tileSize
        };
    }
}

bool Game::isWall(float x, float y) {
    int cellSize = 96; // Adjust cell size as needed
    int mazeX = static_cast<int>(x + 32) / cellSize;
    int mazeY = static_cast<int>(y + 64) / cellSize;

    if (isPlayerInDungeon) {
        // Check bounds for dungeon maze
        if (mazeY < 0 || mazeY >= dungeonMaze.size() || mazeX < 0 || mazeX >= dungeonMaze[0].size()) {
            return true;
        }
        return dungeonMaze[mazeY][mazeX] == -1;
    } else {
        // Check bounds for outer world map
        if (mazeY < 0 || mazeY >= mapMatrix.size() || mazeX < 0 || mazeX >= mapMatrix[0].size()) {
            return true;
        }
        return mapMatrix[mazeY][mazeX] == 6; // Fence collision
    }
}

bool Game::areAllEnemiesCleared() const {
    for (const auto& entity : entities) {
        if (dynamic_cast<const Enemy*>(entity.get())) {
            return false;
        }
    }
    return true;
}

int Game::getDungeonWidth() const {
    return dungeonMaze.empty() ? 0 : dungeonMaze[0].size() * 96; // Assuming each cell is 96 pixels
}

int Game::getDungeonHeight() const {
    return dungeonMaze.size() * 96; // Assuming each cell is 96 pixels
}

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    if (isMenuOpen) {
        return;
    }

    dungeonCv.notify_one();  // Notify dungeon generation thread
    lightingCv.notify_one(); // Notify lighting thread
    pathfindingCv.notify_one(); // Notify pathfinding thread
    playerEnemyActionCv.notify_one();  // Notify player and enemy action thread

    if (player->getIsDead()) {
        player->update(deltaTime, entities, *this);
        if (isPlayerDeathAnimationFinished() && currentTime - deathTime >= DEATH_DELAY) {
            menu->setState(Menu::RESPAWN_MENU);
            isMenuOpen = true;
        }
    } else {
        processInput();
        player->updateCooldowns(deltaTime);

        if (!isPlayerInDungeon && checkDungeonEntrance()) {
            enterDungeon();
        } else if (isPlayerInDungeon) {
            if (checkNextLevelDoor() && areAllEnemiesCleared()) {
                transitionToNextLevel();
            } else if (checkDungeonExit() && areAllEnemiesCleared()) {
                exitDungeon();
            }
        }

        if (isPlayerInDungeon) {
            updateSpellAnimation(deltaTime, entities);
            updateEnemySpellAnimation(deltaTime, entities);

            for (auto& entity : entities) {
                if (entity->isArrowActive()) {
                    SDL_Rect arrowRect = entity->getArrowFrame();
                    arrowRect.x = static_cast<int>(entity->getArrowX());
                    arrowRect.y = static_cast<int>(entity->getArrowY());

                    for (auto& otherEntity : entities) {
                        if (otherEntity.get() != entity.get() && Entity::checkCollision(arrowRect, otherEntity->getBoundingBox())) {
                            if (Enemy* enemy = dynamic_cast<Enemy*>(otherEntity.get())) {
                                applyDamage(*entity, *enemy, Player::ARROW_DAMAGE);
                                entity->shootArrow(Entity::Up); // Deactivate the arrow by resetting its position
                                break;
                            } else if (Player* player = dynamic_cast<Player*>(otherEntity.get())) {
                                applyDamage(*entity, *player, Player::ARROW_DAMAGE); // Adjust arrow damage for player if needed
                                entity->shootArrow(Entity::Up); // Deactivate the arrow by resetting its position
                                break;
                            }
                        }
                    }
                    player->updateArrowPosition(deltaTime, dungeonMaze, 96);
                }
            }

            for (size_t i = 0; i < entities.size(); ++i) {
                if (entities[i]->isMarkedForRemoval()) continue;
                for (size_t j = i + 1; j < entities.size(); ++j) {
                    if (entities[j]->isMarkedForRemoval()) continue;

                    if (Entity::checkCollision(entities[i]->getBoundingBox(), entities[j]->getBoundingBox())) {
                        if (Player* player = dynamic_cast<Player*>(entities[i].get())) {
                            if (Enemy* enemy = dynamic_cast<Enemy*>(entities[j].get())) {
                                resolveCollision(*player, *enemy);
                            }
                        } else if (Player* player = dynamic_cast<Player*>(entities[j].get())) {
                            if (Enemy* enemy = dynamic_cast<Enemy*>(entities[i].get())) {
                                resolveCollision(*player, *enemy);
                            }
                        }
                    }
                }
            }

            float playerX = player->getX();
            float playerY = player->getY();
            updateCamera(playerX, playerY);
        }

        for (size_t i = 0; i < entities.size(); ++i) {
            auto& entity = entities[i];
            if (entity->isMarkedForRemoval()) continue;

            if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                enemy->updateBehavior(deltaTime, *player, entities, *this);
            } else if (Player* playerEntity = dynamic_cast<Player*>(entity.get())) {
                playerEntity->update(deltaTime, entities, *this);
            }
        }

        removeDeadEntities();
        updateCamera(player->getX(), player->getY());
        world->update(camera.x + camera.w / 2, camera.y + camera.h / 2);
    }
}

void Game::removeDeadEntities() {
    entities.erase(std::remove_if(entities.begin(), entities.end(),
                                  [](const std::unique_ptr<Entity>& entity) {
                                      return entity->isMarkedForRemoval();
                                  }), entities.end());
}

void Game::resolveCollision(Player& player, Enemy& enemy) {
    SDL_Rect playerBox = player.getBoundingBox();
    SDL_Rect enemyBox = enemy.getBoundingBox();

    if (Entity::checkCollision(playerBox, enemyBox)) {
        adjustPositionOnCollision(player, enemy);
    }

    SDL_Rect playerAttackBox = player.getAttackBoundingBox();
    SDL_Rect enemyAttackBox = enemy.getAttackBoundingBox();

    Uint32 currentTime = SDL_GetTicks();

    // Check player's attack collision with enemy
    if (Entity::checkCollision(playerAttackBox, enemy.getBoundingBox())) {
        if (player.getAction() == Entity::Thrusting && !player.getDamageApplied() && (currentTime - player.getAttackStartTime() >= player.getAttackDelay())) {
            applyDamage(player, enemy, Player::THRUST_DAMAGE);
            player.setDamageApplied(true);
        } else if (player.getAction() == Entity::Slashing && !player.getDamageApplied() && (currentTime - player.getAttackStartTime() >= player.getAttackDelay())) {
            applyDamage(player, enemy, Player::SLASH_DAMAGE);
            player.setDamageApplied(true);
        }
    }

    // Check enemy's attack collision with player
    if (Entity::checkCollision(enemyAttackBox, player.getBoundingBox())) {
        if (enemy.getAction() == Entity::Thrusting && !enemy.getDamageApplied() && (currentTime - enemy.getAttackStartTime() >= enemy.getAttackDelay())) {
            applyDamage(enemy, player, Enemy::THRUST_DAMAGE);
            enemy.setDamageApplied(true);
        } else if (enemy.getAction() == Entity::Spellcasting && !enemy.getDamageApplied() && (currentTime - enemy.getAttackStartTime() >= enemy.getAttackDelay())) {
            applyDamage(enemy, player, Enemy::SPELL_DAMAGE);
            enemy.setDamageApplied(true);
        }
    }
}

void Game::adjustPositionOnCollision(Player& player, Enemy& enemy) {
    SDL_Rect playerBox = player.getCollisionBoundingBox();
    SDL_Rect enemyBox = enemy.getCollisionBoundingBox();

    // Calculate the overlap between the player and the enemy
    int overlapX = (playerBox.x + playerBox.w / 2) - (enemyBox.x + enemyBox.w / 2);
    int overlapY = (playerBox.y + playerBox.h / 2) - (enemyBox.y + enemyBox.h / 2);

    int halfWidth = (playerBox.w + enemyBox.w) / 2;
    int halfHeight = (playerBox.h + enemyBox.h) / 2;

    if (abs(overlapX) < halfWidth && abs(overlapY) < halfHeight) {
        int offsetX = halfWidth - abs(overlapX);
        int offsetY = halfHeight - abs(overlapY);

        // Resolve collision by adjusting positions based on the overlap amount
        if (offsetX < offsetY) {
            if (overlapX > 0) {
                player.setX(player.getX() + offsetX);
                enemy.setX(enemy.getX() - offsetX);
            } else {
                player.setX(player.getX() - offsetX);
                enemy.setX(enemy.getX() + offsetX);
            }
        } else {
            if (overlapY > 0) {
                player.setY(player.getY() + offsetY);
                enemy.setY(enemy.getY() - offsetY);
            } else {
                player.setY(player.getY() - offsetY);
                enemy.setY(enemy.getY() + offsetY);
            }
        }
    }
}

bool Game::isPlayerDeathAnimationFinished() const {
    return player->getIsDead() && player->isDeathAnimationFinished();
}

void Game::updateCamera(float playerX, float playerY) {
    int deadZoneWidth = 1680;
    int deadZoneHeight = 900;

    float camX = camera.x + camera.w / 2;
    float camY = camera.y + camera.h / 2;

    float deadZoneLeft = camX - deadZoneWidth / 2;
    float deadZoneRight = camX + deadZoneWidth / 2;
    float deadZoneTop = camY - deadZoneHeight / 2;
    float deadZoneBottom = camY + deadZoneHeight / 2;

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
}

void Game::applyDamage(Entity& attacker, Entity& target, int damage) {
    if (!target.isAlive()) return;

    // Check if the target is facing the attacker
    if (isFacing(target, attacker)) {
        float dodgeChance = 0.2f;
        if (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) < dodgeChance) {
            // Dodge successful
            return;
        }
    }

    // Apply damage if not dodged
    target.takeDamage(damage);
    
    // Handle target death
    if (!target.isAlive()) {
        if (Player* player = dynamic_cast<Player*>(&target)) {
            player->setIsDead(true);
            deathTime = SDL_GetTicks();
        } else if (Enemy* enemy = dynamic_cast<Enemy*>(&target)) {
            entitiesToRemove.push_back(&target);
        }
    }
}

bool Game::isFacing(Entity& entity, Entity& target) {
    Entity::Direction facingDirection = entity.getDirection();
    float targetX = target.getX();
    float targetY = target.getY();
    float entityX = entity.getX();
    float entityY = entity.getY();

    switch (facingDirection) {
        case Entity::Up:
            return targetY < entityY;
        case Entity::Down:
            return targetY > entityY;
        case Entity::Left:
            return targetX < entityX;
        case Entity::Right:
            return targetX > entityX;
        default:
            return false;
    }
}

void Game::updateSpellAnimation(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) {
    if (player->getAction() == Entity::Spellcasting && !player->isArrowActive()) {
        // Find the closest enemy
        Enemy* closestEnemy = nullptr;
        float minDistance = std::numeric_limits<float>::max(); // Correct usage of numeric_limits

        for (const auto& entity : entities) {
            if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                float distance = std::hypot(player->getX() - enemy->getX(), player->getY() - enemy->getY());
                if (distance < minDistance) {
                    minDistance = distance;
                    closestEnemy = enemy;
                }
            }
        }

        if (closestEnemy) {
            // Set the spell direction towards the closest enemy
            player->setSpellTarget(closestEnemy->getX(), closestEnemy->getY());
        }
    }

    // Update spell position
    player->updateSpellPosition(deltaTime, entities, *this);
}

void Game::loadHUDTexture() {
    hudTexture = loadTexture("/home/simion/Desktop/3/Game2D/assets/hud.png");
}

void drawRoundedRect(SDL_Renderer* renderer, SDL_Rect rect, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    int x = rect.x;
    int y = rect.y;
    int w = rect.w;
    int h = rect.h;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);

    // Draw the center rectangle
    SDL_Rect centerRect = { x + radius, y, w - 2 * radius, h };
    SDL_RenderFillRect(renderer, &centerRect);

    // Draw the side rectangles
    SDL_Rect leftRect = { x, y + radius, radius, h - 2 * radius };
    SDL_Rect rightRect = { x + w - radius, y + radius, radius, h - 2 * radius };
    SDL_RenderFillRect(renderer, &leftRect);
    SDL_RenderFillRect(renderer, &rightRect);

    // Draw the corners
    for (int woff = 0; woff < radius; woff++) {
        for (int hoff = 0; hoff < radius; hoff++) {
            if ((woff * woff + hoff * hoff) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + radius - woff, y + radius - hoff);
                SDL_RenderDrawPoint(renderer, x + w - radius + woff, y + radius - hoff);
                SDL_RenderDrawPoint(renderer, x + radius - woff, y + h - radius + hoff);
                SDL_RenderDrawPoint(renderer, x + w - radius + woff, y + h - radius + hoff);
            }
        }
    }
}

void Game::renderText(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::renderSmallText(const char* text, int x, int y, SDL_Color color) {
    // Render the outline first
    SDL_Color outlineColor = {0, 0, 0, 255}; // Black color for outline
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx != 0 || dy != 0) {
                SDL_Surface* outlineSurface = TTF_RenderText_Solid(smallFont, text, outlineColor);
                SDL_Texture* outlineTexture = SDL_CreateTextureFromSurface(renderer, outlineSurface);
                SDL_Rect outlineRect = { x + dx, y + dy, outlineSurface->w, outlineSurface->h };
                SDL_RenderCopy(renderer, outlineTexture, NULL, &outlineRect);
                SDL_FreeSurface(outlineSurface);
                SDL_DestroyTexture(outlineTexture);
            }
        }
    }

    // Render the main text
    SDL_Surface* surface = TTF_RenderText_Solid(smallFont, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::renderCooldowns() {
    if (player) {
        int y = 1000;

        std::unordered_map<std::string, int> abilityPositions = {
            {"Spellcasting", 820},
            {"Slashing", 900},
            {"Shooting", 980},
            {"Thrusting", 1060}
        };

        std::unordered_map<std::string, std::string> keybinds = {
            {"Spellcasting", "Q"},
            {"Slashing", "E"},
            {"Shooting", "RC"},
            {"Thrusting", "LC"}
        };

        SDL_Color color = {255, 255, 255, 255}; 
        SDL_Color keybindColor = {200, 200, 0, 255}; // Darker yellow color for keybind text

        for (const auto& ability : abilityPositions) {
            float cooldown = player->getCooldownRemaining(ability.first);
            if (cooldown > 0) {
                std::string cooldownText = std::to_string(static_cast<int>(cooldown));
                renderText(cooldownText.c_str(), ability.second, y, color);
            }
            std::string keybindText = keybinds[ability.first];

            // Different offsets for each keybind to fine-tune their positions
            if (ability.first == "Shooting" || ability.first == "Thrusting") {
                renderSmallText(keybindText.c_str(), ability.second - 25, y + 38, keybindColor);
            } else {
                renderSmallText(keybindText.c_str(), ability.second - 14, y + 38, keybindColor);
            }
        }
    }
}

void Game::renderHUD() {
    // Define the source rectangles for player HUD and ability HUD within the texture
    SDL_Rect playerHudSourceRect = { 0, 0, 550, 85 }; // Coordinates and size of the player HUD in the texture
    SDL_Rect abilityHudSourceRect = { 0, 80, 250, 150 }; // Coordinates and size of the ability HUD in the texture

    // Define the destination rectangles where the HUD parts will be rendered
    int playerHudWidth = 550;  // Width of the player HUD
    int playerHudHeight = 125;  // Height of the player HUD
    int abilityHudWidth = 375; // Width of the ability HUD
    int abilityHudHeight = 125; // Height of the ability HUD

    // Player HUD Position (top-left corner)
    int playerHudX = 10;
    int playerHudY = 10;
    SDL_Rect playerHudDestRect = { playerHudX, playerHudY, playerHudWidth, playerHudHeight };

    // Ability HUD Position (bottom middle of the screen)
    int screenWidth = 1920; // Adjust to your screen width
    int screenHeight = 1080; // Adjust to your screen height
    int abilityHudX = (screenWidth / 2) - (abilityHudWidth / 2);
    int abilityHudY = screenHeight - abilityHudHeight - 10;
    SDL_Rect abilityHudDestRect = { abilityHudX, abilityHudY, abilityHudWidth, abilityHudHeight };

    // Render player HUD
    SDL_RenderCopy(renderer, hudTexture, &playerHudSourceRect, &playerHudDestRect);

    // Render player's health in the HUD
    int healthBarX = playerHudX + 132; // Adjust x position as needed
    int healthBarY = playerHudY + 32; // Adjust y position as needed
    int healthBarWidth = 155;  // Adjust as needed
    int healthBarHeight = 18;  // Adjust as needed
    float healthRatio = static_cast<float>(player->getHealth()) / static_cast<float>(player->getMaxHealth());
    if (healthRatio > 0) {
        SDL_Rect healthRect = { healthBarX, healthBarY, static_cast<int>(healthBarWidth * healthRatio), healthBarHeight };
        // Draw the health bar with rounded corners and slightly whiter red color
        drawRoundedRect(renderer, healthRect, 5, 255, 60, 60, 255); // Rounded corners with radius 5
    }

    // Render the green bar (placeholder)
    int greenBarX = playerHudX + 132; // Adjust x position as needed
    int greenBarY = playerHudY + 61; // Adjust y position as needed
    SDL_Rect greenRect = { greenBarX, greenBarY, healthBarWidth, healthBarHeight };
    drawRoundedRect(renderer, greenRect, 5, 34, 177, 76, 255); // Rounded corners with radius 5, green color

    // Render the stamina bar (blue)
    int blueBarX = playerHudX + 132; // Adjust x position as needed
    int blueBarY = playerHudY + 91; // Adjust y position as needed
    float staminaRatio = static_cast<float>(player->getStamina()) / static_cast<float>(player->getMaxStamina());
    int blueBarWidth = static_cast<int>(healthBarWidth * staminaRatio);
    if (blueBarWidth > 4.75) {
        SDL_Rect blueRect = { blueBarX, blueBarY, blueBarWidth, healthBarHeight };
        drawRoundedRect(renderer, blueRect, 5, 0, 162, 232, 255); // Rounded corners with radius 5, blue color
    }

    // Render ability HUD
    SDL_RenderCopy(renderer, hudTexture, &abilityHudSourceRect, &abilityHudDestRect);

    renderCooldowns();
}

void Game::renderHealthBar(int x, int y, int currentHealth, int maxHealth) {
    int barWidth = 100; // Width of the health bar
    int barHeight = 10; // Height of the health bar
    float healthRatio = static_cast<float>(currentHealth) / static_cast<float>(maxHealth);

    // Draw the background (black)
    SDL_Rect backgroundRect = { x, y, barWidth, barHeight };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &backgroundRect);

    // Draw the health bar (red)
    SDL_Rect healthRect = { x, y, static_cast<int>(barWidth * healthRatio), barHeight };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &healthRect);
}

void Game::render() {
    std::lock_guard<std::mutex> dungeonLock(dungeonMutex);  // Lock dungeon rendering
    std::lock_guard<std::mutex> lightingLock(lightingMutex); // Lock lighting rendering
    std::lock_guard<std::mutex> entityLock(entityMutex); // Lock entity rendering

    SDL_RenderClear(renderer);

    std::vector<SDL_Rect> enemyPositions;
    std::vector<SDL_Rect> spellPositions;

    if (isPlayerInDungeon) {
        // Render the dungeon background and tiles first
        int cellSize = 96; // Adjust cell size as needed
        for (int y = 0; y < dungeonMaze.size(); ++y) {
            for (int x = 0; x < dungeonMaze[y].size(); ++x) {
                SDL_Rect cellRect = {
                    x * cellSize - camera.x,
                    y * cellSize - camera.y,
                    cellSize,
                    cellSize
                };

                if (dungeonMaze[y][x] == -1) {
                    SDL_RenderCopy(renderer, wallTileTexture, NULL, &cellRect);
                } else if (dungeonMaze[y][x] == 0) {
                    SDL_RenderCopy(renderer, pathTileTexture, NULL, &cellRect);
                } else if (dungeonMaze[y][x] == 2) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color for spawn/exit point
                    SDL_RenderFillRect(renderer, &cellRect);
                } else if (dungeonMaze[y][x] == 3) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for next level entrance
                    SDL_RenderFillRect(renderer, &cellRect);
                }
            }
        }

        // Apply dimming by rendering the semi-transparent texture over the entire screen
        SDL_RenderCopy(renderer, lightingManager->getDimmingTexture(), NULL, NULL);

        // Collect enemy positions for lighting effect
        for (const auto& entity : entities) {
            if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                SDL_Rect enemyRect = {
                    static_cast<int>(enemy->getX()) - camera.x,
                    static_cast<int>(enemy->getY()) - camera.y,
                    static_cast<int>(enemy->getCurrentFrame().w * 2),
                    static_cast<int>(enemy->getCurrentFrame().h * 2)
                };
                enemyPositions.push_back(enemyRect);
            }

            // Collect spell positions for additional lighting
            if (entity->isSpellActive()) {
                SDL_Rect spellRect = {
                    static_cast<int>(entity->getSpellX()) - camera.x,
                    static_cast<int>(entity->getSpellY()) - camera.y,
                    64, // Width of the spell texture
                    64  // Height of the spell texture
                };
                spellPositions.push_back(spellRect);
            }
        }

        // Apply lighting effects for player, enemies, and spells
        lightingManager->renderLighting(
            {static_cast<int>(player->getX()), static_cast<int>(player->getY()), player->getCurrentFrame().w * 2, player->getCurrentFrame().h * 2},
            enemyPositions,
            spellPositions,
            dungeonMaze,
            camera
        );

        // Render main entities (Player, Enemies)
        for (const auto& entity : entities) {
            SDL_Rect srcRect = entity->getCurrentFrame();
            SDL_Rect destRect = { 
                static_cast<int>(entity->getX()) - camera.x, 
                static_cast<int>(entity->getY()) - camera.y, 
                static_cast<int>(srcRect.w * 2),  
                static_cast<int>(srcRect.h * 2)  
            };
            SDL_RenderCopy(renderer, entity->getTex(), &srcRect, &destRect);

            // Render spells
            if (entity->isSpellActive()) {
                SDL_Rect spellSrcRect;
                if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                    spellSrcRect = enemy->getSpellFrameForEnemy();
                } else {
                    spellSrcRect = entity->getSpellFrame();
                }

                SDL_Rect spellDestRect = {
                    static_cast<int>(entity->getSpellX()) - camera.x,
                    static_cast<int>(entity->getSpellY()) - camera.y,
                    64,
                    64
                };
                SDL_RenderCopy(renderer, entity->getTex(), &spellSrcRect, &spellDestRect);
            }

            // Render health bar above the entity if it's an enemy
            if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                int healthBarX = destRect.x;
                int healthBarY = destRect.y - 10; // Adjust the Y position to be above the enemy
                renderHealthBar(healthBarX, healthBarY, enemy->getHealth(), enemy->getMaxHealth());
            }
        }

        // Render arrows after lighting effects are applied
        for (const auto& entity : entities) {
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

    } else {
        // Render the world (outside the dungeon)
        world->render(camera.x + camera.w / 2 + 192, camera.y + camera.h / 2 - 256, isPlayerInDungeon, dungeonEntrance, camera, dungeonEntranceTexture, tilesetTexture);

        // Render entities (Player, Enemies, etc.)
        for (const auto& entity : entities) {
            SDL_Rect srcRect = entity->getCurrentFrame();
            SDL_Rect destRect = { 
                static_cast<int>(entity->getX()) - camera.x, 
                static_cast<int>(entity->getY()) - camera.y, 
                static_cast<int>(srcRect.w * 2),  
                static_cast<int>(srcRect.h * 2)  
            };
            SDL_RenderCopy(renderer, entity->getTex(), &srcRect, &destRect);
        }
    }

    // Render HUD
    renderHUD();

    // Render menu if open
    if (isMenuOpen) {
        menu->render();
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    for (auto& entity : entities) {
        entity.reset();
    }
    entities.clear();

    if (spriteSheet) {
        SDL_DestroyTexture(spriteSheet);
        spriteSheet = nullptr;
    }

    if (hudTexture) {
        SDL_DestroyTexture(hudTexture);
        hudTexture = nullptr;
    }

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (smallFont) {
        TTF_CloseFont(smallFont);
        smallFont = nullptr;
    }

    if (pathTileTexture) {
        SDL_DestroyTexture(pathTileTexture);
        pathTileTexture = nullptr;
    }

    if (wallTileTexture) {
        SDL_DestroyTexture(wallTileTexture);
        wallTileTexture = nullptr;
    }

    delete menu;
    menu = nullptr;
    delete world;
    world = nullptr;
    delete lightingManager;
    lightingManager = nullptr;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

