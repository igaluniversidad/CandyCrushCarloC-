#include "GameStateManager.h"
#include <iostream>

GameStateManager::GameStateManager()
{
    platform = new Platform("CandyCrushCarloC++");
    running = true;
    pendingPops = 0;
    pendingPushCount = 0;
    pendingQuit = false;
    for (int i = 0; i < MAX_PUSH; ++i) pendingPush[i] = nullptr;
}

GameStateManager::~GameStateManager()
{
    ClearAll();
    delete platform;
    platform = nullptr;
}

void GameStateManager::ClearAll()
{
    while (!states.IsEmpty())
    {
        GameState* s = states.Pop();
        if (s != nullptr) { s->Close(); delete s; }
    }
    pendingPops = 0;
    pendingPushCount = 0;
}

GameState* GameStateManager::Current()
{
    if (states.IsEmpty()) return nullptr;
    return states.Top();
}

int GameStateManager::Depth()
{
    return states.GetSize();
}

void GameStateManager::SetState(GameState* state)
{
    if (state == nullptr) return;
    state->Init(platform, this);
    states.Push(state);
}

void GameStateManager::RealaseState()
{
    if (states.IsEmpty()) return;
    GameState* s = states.Pop();
    if (s != nullptr) { s->Close(); delete s; }
}

void GameStateManager::RequestPush(GameState* state)
{
    if (state == nullptr) return;
    if (pendingPushCount < MAX_PUSH) pendingPush[pendingPushCount++] = state;
}

void GameStateManager::RequestPop()
{
    pendingPops++;
}

void GameStateManager::RequestReplace(GameState* state)
{
    pendingPops++;
    RequestPush(state);
}

void GameStateManager::RequestQuit()
{
    pendingQuit = true;
}

void GameStateManager::RequestMenu(GameState* menuState)
{
    // Vaciar todo salvo dejar el menu: pops = profundidad actual.
    pendingPops = states.GetSize();
    if (menuState != nullptr) RequestPush(menuState);
}

void GameStateManager::ProcessRequests()
{
    if (pendingQuit) { running = false; return; }
    // Pops primero (incluye el caso Replace).
    while (pendingPops > 0 && !states.IsEmpty())
    {
        GameState* s = states.Pop();
        if (s != nullptr) { s->Close(); delete s; }
        pendingPops--;
    }
    pendingPops = 0;
    for (int i = 0; i < pendingPushCount; ++i)
    {
        if (pendingPush[i] != nullptr)
        {
            pendingPush[i]->Init(platform, this);
            states.Push(pendingPush[i]);
            pendingPush[i] = nullptr;
        }
    }
    pendingPushCount = 0;
}

void GameStateManager::GameLoop()
{
    float last = Platform::NowSec();
    while (running)
    {
        float now = Platform::NowSec();
        float dt = Platform::ClampDt(now - last);
        last = now;

        if (states.IsEmpty())
        {
            std::cout << "GameStateManager: sin escenas, cerrando." << std::endl;
            break;
        }
        GameState* state = states.Top();
        if (state == nullptr) break;

        platform->CheckEvent(state, &GameState::Input);
        ProcessRequests();
        if (!running || states.IsEmpty()) break;

        state = states.Top();
        if (state == nullptr) break;
        state->Update(dt);
        ProcessRequests();
        if (!running || states.IsEmpty()) break;

        state = states.Top();
        if (state == nullptr) break;
        state->Draw();
        ProcessRequests();
    }
}
