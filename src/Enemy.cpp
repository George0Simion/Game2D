#include "Enemy.h"
#include <iostream>
#include <queue>
#include <unordered_map>
#include <algorithm>

const float Enemy::SPELL_COOLDOWN = 10.0f;

const int ENEMY_PADDING_X = 0;
const int ENEMY_PADDING_Y = 0;

const int CELL_SIZE = 96;

Enemy::Enemy(float p_x, float p_y, SDL_Texture* p_tex, int numFrames, float animationSpeed)
    : Entity(p_x, p_y, p_tex, numFrames, animationSpeed), 
      bounceCount(0),
      attackCooldown(0.0f), 
      spellRange(300.0f), 
      thrustRange(100.0f), 
      moveSpeed(75.0f), 
      directionChangeCooldown(0.5f), 
      timeSinceLastDirectionChange(0.0f), 
      hasTarget(false), 
      hasPath(false) {}


void Enemy::setSpellTarget(float targetX, float targetY) {
    spellActive = true;
    spellX = x + FRAME_WIDTH / 2;
    spellY = y - 10;
    spellTargetX = targetX;
    spellTargetY = targetY;
    spellStartTime = SDL_GetTicks();
    spellDuration = SPELL_DURATION;
    setAction(Spellcasting);
    spellCooldownRemaining = SPELL_COOLDOWN;
}

void Enemy::updateSpellPosition(float deltaTime, std::vector<std::unique_ptr<Entity>>& entities, Game& game) {
    if (spellActive) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - spellStartTime > spellDuration) {
            deactivateSpell();
            return;
        }

        Player* closestPlayer = nullptr;
        float minDistance = std::numeric_limits<float>::max();
        for (const auto& entity : entities) {
            if (Player* player = dynamic_cast<Player*>(entity.get())) {
                if (!player->getIsDead()) {
                    float distance = std::hypot(spellX - player->getX(), spellY - player->getY());
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestPlayer = player;
                    }
                }
            }
        }
        if (closestPlayer) {
            spellTargetX = closestPlayer->getX();
            spellTargetY = closestPlayer->getY();
        } else {
            deactivateSpell();
            return;
        }

        float dx = spellTargetX - spellX;
        float dy = spellTargetY - spellY;
        float distance = std::hypot(dx, dy);

        if (distance > 5.0f) {
            float angle = atan2(dy, dx);
            float curve = sin(SDL_GetTicks() * spellCurveFactor);

            float controlPointX = (spellX + spellTargetX) / 2 + curve * 50;
            float controlPointY = (spellY + spellTargetY) / 2 + curve * 50;

            float t = (spellSpeed / 2) * deltaTime / distance;
            spellX = (1 - t) * (1 - t) * spellX + 2 * (1 - t) * t * controlPointX + t * t * spellTargetX;
            spellY = (1 - t) * (1 - t) * spellY + 2 * (1 - t) * t * controlPointY + t * t * spellTargetY;
        } else {
            deactivateSpell();
        }

        SDL_Rect spellRect = { static_cast<int>(spellX), static_cast<int>(spellY), FRAME_WIDTH - 40, FRAME_HEIGHT - 40 };
        for (auto& entity : entities) {
            if (Player* player = dynamic_cast<Player*>(entity.get())) {
                if (!player->getIsDead()) {
                    SDL_Rect playerBoundingBox = player->getBoundingBox();
                    if (SDL_HasIntersection(&spellRect, &playerBoundingBox)) {
                        player->takeDamage(SPELL_DAMAGE);
                        if (!player->isAlive()) {
                            player->setIsDead(true);
                        }
                        deactivateSpell();
                        break;
                    }
                }
            }
        }

        if (isSpellCollidingWithWall(spellX, spellY, 64, game.getDungeonMaze())) {
            if (bounceCount < maxBounces) {
                if (spellX <= 0 || spellX >= game.getDungeonWidth() - FRAME_WIDTH) {
                    dx = -dx;
                }
                if (spellY <= 0 || spellY >= game.getDungeonHeight() - FRAME_HEIGHT) {
                    dy = -dy;
                }
                bounceCount++;
            } else {
                deactivateSpell();
            }
        }
    }
}

std::vector<std::pair<int, int>> Enemy::findPathToPlayer(Player& player, Game& game) {
    auto& dungeonMaze = game.getDungeonMaze();
    int startX = static_cast<int>((getX() + ENEMY_PADDING_X) / CELL_SIZE);
    int startY = static_cast<int>((getY() + ENEMY_PADDING_Y) / CELL_SIZE);
    int goalX = static_cast<int>((player.getX()) / CELL_SIZE);
    int goalY = static_cast<int>((player.getY()) / CELL_SIZE);

    std::vector<std::pair<int, int>> path;
    if (startX == goalX && startY == goalY) return path;

    std::priority_queue<std::tuple<int, int, int, int, std::vector<std::pair<int, int>>>> openSet;
    std::unordered_map<int, std::unordered_map<int, int>> costSoFar;
    openSet.emplace(0, startX, startY, 0, path);
    costSoFar[startX][startY] = 0;

    while (!openSet.empty()) {
        auto [priority, x, y, cost, path] = openSet.top();
        openSet.pop();

        if (x == goalX && y == goalY) return path;

        static const std::vector<std::pair<int, int>> directions = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
        for (const auto& [dx, dy] : directions) {
            int nextX = x + dx;
            int nextY = y + dy;
            int newCost = cost + 1;

            if (nextX >= 0 && nextX < dungeonMaze[0].size() && nextY >= 0 && nextY < dungeonMaze.size() && dungeonMaze[nextY][nextX] != -1) {
                if (!costSoFar.count(nextX) || !costSoFar[nextX].count(nextY) || newCost < costSoFar[nextX][nextY]) {
                    costSoFar[nextX][nextY] = newCost;
                    int priority = newCost + abs(nextX - goalX) + abs(nextY - goalY);
                    auto newPath = path;
                    newPath.emplace_back(nextX, nextY);
                    openSet.emplace(priority, nextX, nextY, newCost, newPath);
                }
            }
        }
    }

    return {}; // Return an empty path if no path is found
}

int heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

void Enemy::followPlayer(float deltaTime, Player& player, Game& game) {
    float moveSpeed = 100.0f;
    bool moved = false;

    timeSinceLastDirectionChange += deltaTime;

    auto& dungeonMaze = game.getDungeonMaze(); // Access the dungeonMaze using the getter method

    if (!pathToPlayer.empty()) {
        auto [nextX, nextY] = pathToPlayer[currentPathIndex];
        float targetX = nextX * CELL_SIZE;
        float targetY = nextY * CELL_SIZE;

        if (abs(targetX - getX()) > abs(targetY - getY())) {
            if (targetX > getX()) {
                if (!game.isWall(getX() + moveSpeed * deltaTime + ENEMY_PADDING_X, getY() + ENEMY_PADDING_Y)) {
                    setX(getX() + moveSpeed * deltaTime);
                    moved = true;
                }
                setDirection(Right);
            } else {
                if (!game.isWall(getX() - moveSpeed * deltaTime + ENEMY_PADDING_X, getY() + ENEMY_PADDING_Y)) {
                    setX(getX() - moveSpeed * deltaTime);
                    moved = true;
                }
                setDirection(Left);
            }
        } else {
            if (targetY > getY()) {
                if (!game.isWall(getX() + ENEMY_PADDING_X, getY() + moveSpeed * deltaTime + ENEMY_PADDING_Y)) {
                    setY(getY() + moveSpeed * deltaTime);
                    moved = true;
                }
                setDirection(Down);
            } else {
                if (!game.isWall(getX() + ENEMY_PADDING_X, getY() - moveSpeed * deltaTime + ENEMY_PADDING_Y)) {
                    setY(getY() - moveSpeed * deltaTime);
                    moved = true;
                }
                setDirection(Up);
            }
        }

        if (moved && abs(getX() - targetX) < moveSpeed * deltaTime && abs(getY() - targetY) < moveSpeed * deltaTime) {
            currentPathIndex++;
        }
    }

    if (!moved && timeSinceLastDirectionChange >= directionChangeCooldown) {
        timeSinceLastDirectionChange = 0.0f; // Reset timer

        // Choose a new direction where there is no wall
        std::vector<Direction> directions = {Up, Down, Left, Right};
        std::random_shuffle(directions.begin(), directions.end());
        for (Direction dir : directions) {
            float newX = getX();
            float newY = getY();
            switch (dir) {
                case Up:
                    newY -= moveSpeed * deltaTime;
                    break;
                case Down:
                    newY += moveSpeed * deltaTime;
                    break;
                case Left:
                    newX -= moveSpeed * deltaTime;
                    break;
                case Right:
                    newX += moveSpeed * deltaTime;
                    break;
            }
            if (!game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y)) {
                setX(newX);
                setY(newY);
                setDirection(dir);
                break;
            }
        }
    } else if (!moved) {
        timeSinceLastDirectionChange = directionChangeCooldown; // Force direction change if stuck
    }

    startAnimation();
}

void Enemy::moveDirectlyToPlayer(float deltaTime, Player& player, Game& game) {
    if (isMarkedForRemoval()) return;

    float distanceX = player.getX() - getX();
    float distanceY = player.getY() - getY();
    float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

    if (spellCooldownRemaining > 0.0f) {
        spellCooldownRemaining -= deltaTime;
    }

    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
    } else {
        decideAction(player, distance);
    }

    if (getAction() == Walking) {
        float moveSpeed = 100.0f;
        bool moved = false;

        if (fabs(distanceX) > fabs(distanceY)) {
            if (distanceX > 0) {
                if (!game.isWall(getX() + moveSpeed * deltaTime + ENEMY_PADDING_X, getY() + ENEMY_PADDING_Y)) {
                    setX(getX() + moveSpeed * deltaTime);
                    moved = true;
                }
                setDirection(Right);
            } else {
                if (!game.isWall(getX() - moveSpeed * deltaTime + ENEMY_PADDING_X, getY() + ENEMY_PADDING_Y)) {
                    setX(getX() - moveSpeed * deltaTime);
                    moved = true;
                }
                setDirection(Left);
            }
        } else {
            if (distanceY > 0) {
                if (!game.isWall(getX() + ENEMY_PADDING_X, getY() + moveSpeed * deltaTime + ENEMY_PADDING_Y)) {
                    setY(getY() + moveSpeed * deltaTime);
                    moved = true;
                }
                setDirection(Down);
            } else {
                if (!game.isWall(getX() + ENEMY_PADDING_X, getY() - moveSpeed * deltaTime + ENEMY_PADDING_Y)) {
                    setY(getY() - moveSpeed * deltaTime);
                    moved = true;
                }
                setDirection(Up);
            }
        }

        if (!moved) {
            pathToPlayer = findPathToPlayer(player, game);
            currentPathIndex = 0;
            followPlayer(deltaTime, player, game);
        }
    }

    startAnimation();
}

void Enemy::randomMove(float deltaTime, Game& game) {
    float moveSpeed = 100.0f;
    bool moved = false;

    timeSinceLastDirectionChange += deltaTime;

    if (timeSinceLastDirectionChange >= directionChangeCooldown) {  // Check if cooldown has passed
        timeSinceLastDirectionChange = 0.0f;  // Reset timer
        std::vector<Direction> directions = {Up, Down, Left, Right};
        std::random_shuffle(directions.begin(), directions.end());
        for (Direction dir : directions) {
            float newX = getX();
            float newY = getY();
            switch (dir) {
                case Up:
                    newY -= moveSpeed * deltaTime;
                    if (!game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y) && 
                        !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y) &&
                        !game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1) &&
                        !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1)) {
                        setY(newY);
                        setDirection(Up);
                        moved = true;
                    }
                    break;
                case Down:
                    newY += moveSpeed * deltaTime;
                    if (!game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y) && 
                        !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y) &&
                        !game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1) &&
                        !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1)) {
                        setY(newY);
                        setDirection(Down);
                        moved = true;
                    }
                    break;
                case Left:
                    newX -= moveSpeed * deltaTime;
                    if (!game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y) && 
                        !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y) &&
                        !game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1) &&
                        !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1)) {
                        setX(newX);
                        setDirection(Left);
                        moved = true;
                    }
                    break;
                case Right:
                    newX += moveSpeed * deltaTime;
                    if (!game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y) && 
                        !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y) &&
                        !game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1) &&
                        !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1)) {
                        setX(newX);
                        setDirection(Right);
                        moved = true;
                    }
                    break;
            }
            if (moved) break;
        }
    }

    if (!moved) {
        // Continue moving in the same direction if not changed
        switch (getDirection()) {
            case Up:
                if (!game.isWall(getX() + ENEMY_PADDING_X, getY() - moveSpeed * deltaTime + ENEMY_PADDING_Y) &&
                    !game.isWall(getX() + ENEMY_PADDING_X + FRAME_WIDTH - 1, getY() - moveSpeed * deltaTime + ENEMY_PADDING_Y) &&
                    !game.isWall(getX() + ENEMY_PADDING_X, getY() - moveSpeed * deltaTime + ENEMY_PADDING_Y + FRAME_HEIGHT - 1) &&
                    !game.isWall(getX() + ENEMY_PADDING_X + FRAME_WIDTH - 1, getY() - moveSpeed * deltaTime + ENEMY_PADDING_Y + FRAME_HEIGHT - 1)) {
                    setY(getY() - moveSpeed * deltaTime);
                    moved = true;
                } else {
                    timeSinceLastDirectionChange = directionChangeCooldown; // Force direction change
                }
                break;
            case Down:
                if (!game.isWall(getX() + ENEMY_PADDING_X, getY() + moveSpeed * deltaTime + ENEMY_PADDING_Y) &&
                    !game.isWall(getX() + ENEMY_PADDING_X + FRAME_WIDTH - 1, getY() + moveSpeed * deltaTime + ENEMY_PADDING_Y) &&
                    !game.isWall(getX() + ENEMY_PADDING_X, getY() + moveSpeed * deltaTime + ENEMY_PADDING_Y + FRAME_HEIGHT - 1) &&
                    !game.isWall(getX() + ENEMY_PADDING_X + FRAME_WIDTH - 1, getY() + moveSpeed * deltaTime + ENEMY_PADDING_Y + FRAME_HEIGHT - 1)) {
                    setY(getY() + moveSpeed * deltaTime);
                    moved = true;
                } else {
                    timeSinceLastDirectionChange = directionChangeCooldown; // Force direction change
                }
                break;
            case Left:
                if (!game.isWall(getX() - moveSpeed * deltaTime + ENEMY_PADDING_X, getY() + ENEMY_PADDING_Y) &&
                    !game.isWall(getX() - moveSpeed * deltaTime + ENEMY_PADDING_X + FRAME_WIDTH - 1, getY() + ENEMY_PADDING_Y) &&
                    !game.isWall(getX() - moveSpeed * deltaTime + ENEMY_PADDING_X, getY() + ENEMY_PADDING_Y + FRAME_HEIGHT - 1) &&
                    !game.isWall(getX() - moveSpeed * deltaTime + ENEMY_PADDING_X + FRAME_WIDTH - 1, getY() + ENEMY_PADDING_Y + FRAME_HEIGHT - 1)) {
                    setX(getX() - moveSpeed * deltaTime);
                    moved = true;
                } else {
                    timeSinceLastDirectionChange = directionChangeCooldown; // Force direction change
                }
                break;
            case Right:
                if (!game.isWall(getX() + moveSpeed * deltaTime + ENEMY_PADDING_X, getY() + ENEMY_PADDING_Y) &&
                    !game.isWall(getX() + moveSpeed * deltaTime + ENEMY_PADDING_X + FRAME_WIDTH - 1, getY() + ENEMY_PADDING_Y) &&
                    !game.isWall(getX() + moveSpeed * deltaTime + ENEMY_PADDING_X, getY() + ENEMY_PADDING_Y + FRAME_HEIGHT - 1) &&
                    !game.isWall(getX() + moveSpeed * deltaTime + ENEMY_PADDING_X + FRAME_WIDTH - 1, getY() + ENEMY_PADDING_Y + FRAME_HEIGHT - 1)) {
                    setX(getX() + moveSpeed * deltaTime);
                    moved = true;
                } else {
                    timeSinceLastDirectionChange = directionChangeCooldown; // Force direction change
                }
                break;
        }
    }

    if (!moved) {
        // Choose a new direction where there is no wall
        std::vector<Direction> directions = {Up, Down, Left, Right};
        std::random_shuffle(directions.begin(), directions.end());
        for (Direction dir : directions) {
            float newX = getX();
            float newY = getY();
            switch (dir) {
                case Up:
                    newY -= moveSpeed * deltaTime;
                    break;
                case Down:
                    newY += moveSpeed * deltaTime;
                    break;
                case Left:
                    newX -= moveSpeed * deltaTime;
                    break;
                case Right:
                    newX += moveSpeed * deltaTime;
                    break;
            }
            if (!game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y) && 
                !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y) &&
                !game.isWall(newX + ENEMY_PADDING_X, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1) &&
                !game.isWall(newX + ENEMY_PADDING_X + FRAME_WIDTH - 1, newY + ENEMY_PADDING_Y + FRAME_HEIGHT - 1)) {
                setX(newX);
                setY(newY);
                setDirection(dir);
                break;
            }
        }
    }

    startAnimation();
}

void Enemy::updateBehavior(float deltaTime, Player& player, std::vector<std::unique_ptr<Entity>>& entities, Game& game) {
    if (isMarkedForRemoval()) return;

    float distanceX = player.getX() - getX();
    float distanceY = player.getY() - getY();
    float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

    if (spellCooldownRemaining > 0.0f) {
        spellCooldownRemaining -= deltaTime;
    }

    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
    }

    // Track player's cell position
    static int lastPlayerCellX = static_cast<int>(player.getX() / 96);
    static int lastPlayerCellY = static_cast<int>(player.getY() / 96);
    
    int currentPlayerCellX = static_cast<int>(player.getX() / 96);
    int currentPlayerCellY = static_cast<int>(player.getY() / 96);

    // Recalculate path if player moves to a new cell
    if (currentPlayerCellX != lastPlayerCellX || currentPlayerCellY != lastPlayerCellY) {
        pathToPlayer = findPathToPlayer(player, game);
        currentPathIndex = 0;
        lastPlayerCellX = currentPlayerCellX;
        lastPlayerCellY = currentPlayerCellY;
        hasPath = true;
    }

    // Check distances and decide behavior
    if (distance <= 150.0f) {
        moveDirectlyToPlayer(deltaTime, player, game);
    } else if (distance <= 800.0f) {
        if (spellCooldownRemaining <= 0.0f) {
            decideAction(player, distance);
        }
        followPlayer(deltaTime, player, game);
    } else {
        randomMove(deltaTime, game);
        hasPath = false;  // Reset path when player is far
    }

    updateEnemy(deltaTime, player, entities, game);
}

void Enemy::updateEnemy(float deltaTime, Player& player, std::vector<std::unique_ptr<Entity>>& entities, Game& game) {
    if (isMarkedForRemoval()) return;

    float actualSpeed = isRunning() ? 1.5f * moveSpeed : moveSpeed;
    float actualAnimationSpeed = isRunning() ? getAnimationSpeed() / 1.5f : getAnimationSpeed();

    if (isMoving() || getAction() != Walking) {
        setAnimationTimer(getAnimationTimer() + deltaTime);

        if (getAnimationTimer() >= actualAnimationSpeed) {
            setAnimationTimer(0.0f);
            setCurrentFrameIndex((getCurrentFrameIndex() + 1) % getNumFrames());

            SDL_Rect& currentFrame = getCurrentFrameRef();
            currentFrame.x = getCurrentFrameIndex() * FRAME_WIDTH;
            currentFrame.y = (getDirection() + getActionOffset() * 4) * FRAME_HEIGHT;

            if ((getAction() == Slashing && getCurrentFrameIndex() == getNumFrames() - 1) ||
                (getAction() == Spellcasting && getCurrentFrameIndex() == getNumFrames() - 1) ||
                (getAction() == Thrusting && getCurrentFrameIndex() == getNumFrames() - 1)) {
                stopAnimation();
                setAction(Walking);
            }
        }
    }

    if (isSpellActive()) {
        updateSpellPosition(deltaTime, entities, game);
    }
}

int Enemy::getActionOffset() const {
    int actionOffset = 0;
    switch (getAction()) {
        case Walking: actionOffset = 2; break;
        case Thrusting: actionOffset = 1; break;
        case Spellcasting: actionOffset = 0; break;
        default: actionOffset = 2; break;
    }
    return actionOffset;
}

void Enemy::decideAction(Player& player, float distance) {
    int randomFactor = rand() % 100;

    if (distance > spellRange && randomFactor < 20 && spellCooldownRemaining <= 0.0f) {
        setAction(Spellcasting);
        attackCooldown = 5.0f;
        setAttackStartTime(SDL_GetTicks());
        setAttackDelay(500);
        setDamageApplied(false);
        spellCooldownRemaining = SPELL_COOLDOWN;
    } else if (distance <= thrustRange * 1.2 && randomFactor < 60) {
        setAction(Thrusting);
        attackCooldown = 1.0f;
        setAttackStartTime(SDL_GetTicks());
        setAttackDelay(150);
        setDamageApplied(false);
    } else {
        setAction(Walking);
    }

    startAnimation();
}

SDL_Rect Enemy::getAttackBoundingBox() const {
    SDL_Rect boundingBox = Entity::getAttackBoundingBox();

    if (getAction() == Thrusting) {
        int thrustRange = getThrustRange();
        switch (getDirection()) {
            case Up:
                boundingBox.y -= thrustRange; boundingBox.h += thrustRange; break;
            case Down:
                boundingBox.h += thrustRange; break;
            case Left:
                boundingBox.x -= thrustRange; boundingBox.w += thrustRange; break;
            case Right:
                boundingBox.w += thrustRange; break;
        }
    }

    return boundingBox;
}

int Enemy::getThrustRange() const {
    return 60;
}