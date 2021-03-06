#include "MainGame.h"

#include <iostream>
#include <stdio.h>
#include "CollisionManager.h"
#include "GraphicsManager.h"
#include "MapObject.h"
#include "PlayerShip.h"
#include "SDL_image.h"
#include "Timer.h"
#include "Wall.h"

MainGame::MainGame() {
    _gameState = GameState::PLAY;
    _mapObjects = new std::vector<MapObject*>();
    _graphicsManager = new GraphicsManager(_mapObjects);
    _collisionManager = new CollisionManager(_mapObjects);
}

MainGame::~MainGame() {
    SDL_Quit();
    // Deallocate all MapObjects
    for (unsigned int i = 0; i < _mapObjects->size(); ++i) {
        delete _mapObjects->at(i);
        _mapObjects->at(i) = nullptr;
    }
    delete _graphicsManager;
    _graphicsManager = nullptr;
    delete _mapObjects;
    _mapObjects = nullptr;
    delete _collisionManager;
    _collisionManager = nullptr;
}

void MainGame::run() {
    initSystems();
    gameLoop();
}

void MainGame::initSystems() {
    // Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    // Initialize graphics: create window, etc
    _graphicsManager->initGraphics();
    // Add listeners to vector for them to notified when the game is updated
    gameUpdatedListeners.push_back(_graphicsManager);
    gameUpdatedListeners.push_back(_collisionManager);
    // Create the player ship
    _playerShip = new PlayerShip(30, 30, _graphicsManager);
    _mapObjects->push_back(_playerShip);
    // Create another MapObject to test collisions
    _mapObjects->push_back(new PlayerShip(100, 100, _graphicsManager));
    // Create a Wall object to test collisions
    _mapObjects->push_back(new Wall(200, 200, _graphicsManager));
}

void MainGame::gameLoop() {
    // Timer used to calculate fps
    Timer fpsTimer;
    // Integer for keeping track of how many frames have been rendered so far
    int countedFrames = 0;
    fpsTimer.start();
    while (_gameState != GameState::EXIT) {
        // Calculate and print current avarage fps
        // int avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
        // std::cout << avgFPS << std::endl;
        processInput();
        notifyGameUpdatedListeners();
        for (unsigned int i = 0; i < _mapObjects->size(); ++i) {
            (_mapObjects->at(i))->moveObject();
        }
        ++countedFrames;
    }
}

void MainGame::processInput() {
    SDL_Event event;
    while(SDL_PollEvent(&event) != 0) {
        // If user requests to quit the game
        if (event.type == SDL_QUIT) {
            _gameState = GameState::EXIT;
            return;
        }
    }
    // Process user input
    if (_playerShip != nullptr) {
        _playerShip->processInput();
    }
}

void MainGame::notifyGameUpdatedListeners() {
    for (GameUpdatedListener* listener : gameUpdatedListeners) {
        listener->GameUpdated();
    }
}
