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

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        isRunning = false;
    } else {
        font = TTF_OpenFont("/home/simion/Desktop/3/Game2D/assets/font.ttf", 32); // Adjust the path and size as needed
        if (!font) {
            printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
            isRunning = false;
        }
    }

    /* Loading the main character and adding it to the vector */
    SDL_Texture* tex = loadTexture("/home/simion/Desktop/3/Game2D/assets/sprite_good_arrow3.png");
    player = new Player(width / 2 - 64, height / 2 - 64, tex, 4, 0.1f);     /* Center the player */
    entities.push_back(std::unique_ptr<Entity>(player));
    player->setHealth(Player::INITIAL_HEALTH);                                                 /* Set the health of the player */

    lastTime = SDL_GetTicks();
    deltaTime = 0.0f;                                                       /* Getting the in-game time for the movement */

    menu = new Menu(this);                                                  /* Allocating memory for the menu */
    world = new World(renderer, 12345);                                     /* Initialize the world with a seed for procedural generation */

    camera = {0, 0, 1680, 900};                                             /* Initialize the camera */

    // Center the camera on the player initially
    camera.x = player->getX() - camera.w / 2;
    camera.y = player->getY() - camera.h / 2;

    world->update(camera.x + camera.w / 2, camera.y + camera.h / 2);

    loadHUDTexture();                                                        /* Load the HUD texture */
    spawnEnemy();
}

SDL_Texture* Game::loadTexture(const char* fileName) {                      /* Method for loading the texture for the main character */
    SDL_Surface* tempSurface = IMG_Load(fileName);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return tex;
}

void Game::spawnEnemy() {
    SDL_Texture* enemyTex = loadTexture("/home/simion/Desktop/3/Game2D/assets/enemy4.png");
    float x = 540;
    float y = 100;
    auto enemy = std::make_unique<Enemy>(x, y, enemyTex, 8, 0.1f);
    enemy->setHealth(Enemy::INITIAL_HEALTH);
    entities.push_back(std::move(enemy));
}

void Game::resetGame() {
    for (auto& entity : entities) {
        entity.reset();
    }
    entities.clear();

    // Reinitialize game state without recreating the window and renderer
    player = new Player(1920 / 2 - 64, 1080 / 2 - 64, loadTexture("/home/simion/Desktop/3/Game2D/assets/sprite_good_arrow3.png"), 4, 0.1f);
    player->setHealth(Player::INITIAL_HEALTH);
    entities.push_back(std::unique_ptr<Entity>(player));

    world = new World(renderer, 12345); // Reinitialize the world with a seed for procedural generation

    camera = {0, 0, 1680, 900}; // Initialize the camera

    // Center the camera on the player initially
    camera.x = player->getX() - camera.w / 2;
    camera.y = player->getY() - camera.h / 2;

    world->update(camera.x + camera.w / 2, camera.y + camera.h / 2);

    spawnEnemy();

    // init("Game Window", 1920, 1080, false);  // Reinitialize the game
    isMenuOpen = false;
    isRunning = true;
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

void Game::updateEnemySpellAnimation(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities) {
    for (const auto& entity : entities) {
        if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
            if (enemy->getAction() == Entity::Spellcasting && !enemy->isSpellActive()) {
                enemy->setSpellTarget(player->getX(), player->getY());
            }
            enemy->updateSpellPosition(deltaTime, entities);

            // Ensure the enemy's texture is restored after spellcasting
            if (enemy->getAction() == Entity::Spellcasting && !enemy->isSpellActive()) {
                enemy->setAction(Entity::Walking); // Set back to walking after spellcasting
            }
        }
    }
}

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    if (isMenuOpen) {
        return;
    }

    if (player->getIsDead()) {
        player->update(deltaTime, entities, *this);
        if (isPlayerDeathAnimationFinished() && currentTime - deathTime >= DEATH_DELAY) {
            menu->setState(Menu::RESPAWN_MENU);
            isMenuOpen = true;
        }
    } else {
        processInput();
        player->updateCooldowns(deltaTime);

        for (size_t i = 0; i < entities.size(); ++i) {
            auto& entity = entities[i];
            if (entity->isMarkedForRemoval()) continue;

            if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
                enemy->updateBehavior(deltaTime, *player, entities);

            } else if (Player* playerEntity = dynamic_cast<Player*>(entity.get())) {
                playerEntity->update(deltaTime, entities, *this);
            }
        }

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
                            player->updateArrowPosition(deltaTime, entities);
                            applyDamage(*entity, *player, Player::ARROW_DAMAGE); // Adjust arrow damage for player if needed
                            entity->shootArrow(Entity::Up); // Deactivate the arrow by resetting its position
                            break;
                        }
                    }
                }
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

        removeDeadEntities();

        float playerX = player->getX();
        float playerY = player->getY();
        updateCamera(playerX, playerY);

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
    target.takeDamage(damage);
    if (!target.isAlive()) {
        if (Player* player = dynamic_cast<Player*>(&target)) {
            player->setIsDead(true);
            deathTime = SDL_GetTicks();

        } else if (Enemy* enemy = dynamic_cast<Enemy*>(&target)) {
            entitiesToRemove.push_back(&target);
        }
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
    player->updateSpellPosition(deltaTime, entities);
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

void Game::renderCooldowns() {
    if (player) {
        int y = 1000;

        std::unordered_map<std::string, int> abilityPositions = {
            {"Spellcasting", 820},
            {"Slashing", 900},
            {"Shooting", 980}
        };

        SDL_Color color = {255, 255, 255, 255}; // White color for text

        for (const auto& ability : abilityPositions) {
            float cooldown = player->getCooldownRemaining(ability.first);
            if (cooldown > 0) {
                std::string cooldownText = std::to_string(static_cast<int>(cooldown));
                renderText(cooldownText.c_str(), ability.second, y, color);
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

    // Render the blue bar (placeholder)
    int blueBarX = playerHudX + 132; // Adjust x position as needed
    int blueBarY = playerHudY + 91; // Adjust y position as needed
    SDL_Rect blueRect = { blueBarX, blueBarY, healthBarWidth, healthBarHeight };
    drawRoundedRect(renderer, blueRect, 5, 0, 162, 232, 255); // Rounded corners with radius 5, blue color

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

    renderHUD();

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

    delete menu;
    menu = nullptr;
    delete world;
    world = nullptr;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

