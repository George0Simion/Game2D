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

    spawnEnemy();
}

SDL_Texture* Game::loadTexture(const char* fileName) {                      /* Method for loading the texture for the main character */
    SDL_Surface* tempSurface = IMG_Load(fileName);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    return tex;
}

void Game::spawnEnemy() {
    SDL_Texture* enemyTex = loadTexture("/home/simion/Desktop/2/Game2D/assets/enemy3.png");
    float x = 100;
    float y = 100;
    auto enemy = std::make_unique<Enemy>(x, y, enemyTex, 8, 0.1f);
    enemy->setHealth(Enemy::INITIAL_HEALTH);
    entities.push_back(std::move(enemy));
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
                        isMenuOpen = !isMenuOpen;
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

void Game::update() {
    Uint32 currentTime = SDL_GetTicks();
    deltaTime = (currentTime - lastTime) / 1000.0f;
    lastTime = currentTime;

    processInput();

    // Update all entities
    for (auto& entity : entities) {
        if (Enemy* enemy = dynamic_cast<Enemy*>(entity.get())) {
            enemy->updateBehavior(deltaTime, *player);
        }
        entity->update(deltaTime);
    }

    // Handle arrow collisions and update positions
    for (auto& entity : entities) {
        if (entity->isArrowActive()) {
            SDL_Rect arrowRect = entity->getArrowFrame();
            arrowRect.x = static_cast<int>(entity->getArrowX());
            arrowRect.y = static_cast<int>(entity->getArrowY());

            for (auto& otherEntity : entities) {
                if (otherEntity.get() != entity.get() && Entity::checkCollision(arrowRect, otherEntity->getBoundingBox())) {
                    // Apply damage to the target if hit by an arrow
                    if (Enemy* enemy = dynamic_cast<Enemy*>(otherEntity.get())) {
                        applyDamage(*entity, *enemy, Player::ARROW_DAMAGE);
                        entity->shootArrow(Entity::Up); // Deactivate the arrow by resetting its position
                        break; // Exit the inner loop to prevent multiple hits in one frame
                    } else if (Player* player = dynamic_cast<Player*>(otherEntity.get())) {
                        applyDamage(*entity, *player, Player::ARROW_DAMAGE); // Adjust arrow damage for player if needed
                        entity->shootArrow(Entity::Up); // Deactivate the arrow by resetting its position
                        break; // Exit the inner loop to prevent multiple hits in one frame
                    }
                }
            }
        }
    }

    // Check for collisions and resolve them
    for (size_t i = 0; i < entities.size(); ++i) {
        for (size_t j = i + 1; j < entities.size(); ++j) {
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

    // Remove entities marked for removal
    for (Entity* entity : entitiesToRemove) {
        auto it = std::remove_if(entities.begin(), entities.end(),
                                 [entity](const std::unique_ptr<Entity>& e) { return e.get() == entity; });
        entities.erase(it, entities.end());
    }
    entitiesToRemove.clear();

    float playerX = player->getX();
    float playerY = player->getY();

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

    camX = camera.x + camera.w / 2;
    camY = camera.y + camera.h / 2;

    deadZoneLeft = camX - deadZoneWidth / 2;
    deadZoneRight = camX + deadZoneWidth / 2;
    deadZoneTop = camY - deadZoneHeight / 2;
    deadZoneBottom = camY + deadZoneHeight / 2;

    world->update(camera.x + camera.w / 2, camera.y + camera.h / 2);
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

void Game::applyDamage(Entity& attacker, Entity& target, int damage) {
    target.takeDamage(damage);
    if (!target.isAlive()) {
        if (Player* player = dynamic_cast<Player*>(&target)) {
            isRunning = false; // Stop the game if the player dies
        } else if (Enemy* enemy = dynamic_cast<Enemy*>(&target)) {
            // Mark the enemy for removal
            entitiesToRemove.push_back(&target);
        }
    }
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

        // Render health bar above the entity
        int healthBarX = destRect.x;
        int healthBarY = destRect.y - 10; // Adjust the Y position to be above the entity
        renderHealthBar(healthBarX, healthBarY, entity->getHealth(), entity->getMaxHealth());
    }

    // renderHealthBar(10, 10, player->getHealth(), Player::INITIAL_HEALTH);

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