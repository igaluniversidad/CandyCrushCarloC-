#pragma once
#include "../GameState.h"
#include "../Image.h"
#include "../Text.h"
#include "../Logic/Board.h"
#include "../MyLib/LinkedQueue.h"
#include "../MyLib/RingBuffer.h"
#include "../MyLib/ObjectPool.h"
#include "../Game/Particle.h"

// =====================================================================
//  GameplayState - El juego Match-3 (usa TODAS tus estructuras).
//  Stack: el manager lo apila. Grid: Board. Queue: eventos de destruccion.
//  FloodFill: bomba (Board::DetonatePreview). ObjectPool: particulas VFX.
//  RingBuffer: snapshots para Undo (3 usos).
//  Fases: Idle -> Swapping -> Popping -> Falling -> (cascada?) -> Idle.
// =====================================================================

class GameplayState : public GameState
{
public:
    enum Phase { Idle, Swapping, Popping, Falling };

private:
    Platform* platform;
    GameStateManager* manager;

    Board* board;
    LinkedQueue<CellPos>* destroyQueue;
    RingBuffer<BoardSnapshot>* history;
    ObjectPool<Particle>* particles;

    Image* bg;
    Image* btnImg;
    Image* gems[6];
    Image* bombs[6];

    Text* scoreT;
    Text* timeT;
    Text* movesT;
    Text* multT;
    Text* msgT;
    Text* undoT;
    Text* btnPauseT;
    Text* btnUndoT;
    Text* btnMenuT;

    int score;
    int movesLeft;
    int undosLeft;
    int cascadeLevel;
    float timeLeft;
    float idleTime;
    float hintPulse;
    float popTimer;
    float swapTimer;
    float msgTimer;
    char msgBuf[96];

    Phase phase;
    int selR, selC;
    bool dying[Board::ROWS][Board::COLS];
    int dyingCount;
    float cellY[Board::ROWS][Board::COLS]; // pos Y visual por celda (caida suave)
    int hintR1, hintC1, hintR2, hintC2;
    bool hintReady;
    int lastScoreShown, lastTimeShown, lastMovesShown, lastMultShown, lastUndoShown;
    unsigned char gemColor[6][3];

    void LoadAssets();
    void FreeAssets();
    void RefreshTexts(bool force);
    void SetMsg(const char* s);
    void PushHistory();
    bool TrySwap(int r1, int c1, int r2, int c2);
    void BeginResolving();
    void BeginPopping();
    void FinishPopping();
    void BeginFalling();
    void SpawnExplosion(float cx, float cy, int colorIdx);
    void UpdateParticles(float dt);
    void DrawParticles();
    void CellToXY(int r, int c, float& x, float& y);
    bool PointToCell(float mx, float my, int& r, int& c);
    void ComputeHintIfNeeded();

public:
    GameplayState();
    ~GameplayState();
    void Init(Platform* platform, GameStateManager* manager) override;
    void Draw() override;
    bool Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit) override;
    void Update(float dt) override;
    void Close() override;
};
