#include "GameplayState.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "../GameStateManager.h"
#include "../Game/CandyConfig.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "FontUtil.h"
#include "UiButton.h"
#include <cstdio>
#include <cstring>

GameplayState::GameplayState()
{
    platform = nullptr; manager = nullptr;
    board = nullptr; destroyQueue = nullptr; history = nullptr; particles = nullptr;
    bg = nullptr; btnImg = nullptr;
    for (int i = 0; i < 6; ++i) { gems[i] = nullptr; bombs[i] = nullptr; }
    scoreT = timeT = movesT = multT = msgT = undoT = nullptr;
    btnPauseT = btnUndoT = btnMenuT = nullptr;
    score = 0; movesLeft = CandyConfig::GAME_MOVES; undosLeft = CandyConfig::UNDO_USES;
    cascadeLevel = 1; timeLeft = CandyConfig::GAME_TIME;
    idleTime = 0; hintPulse = 0; popTimer = 0; swapTimer = 0; msgTimer = 0;
    msgBuf[0] = '\0';
    phase = Idle; selR = -1; selC = -1; dyingCount = 0;
    hintR1 = hintC1 = hintR2 = hintC2 = -1; hintReady = false;
    lastScoreShown = lastTimeShown = lastMovesShown = lastMultShown = lastUndoShown = -9999;
    for (int r = 0; r < Board::ROWS; ++r)
        for (int c = 0; c < Board::COLS; ++c) { dying[r][c] = false; cellY[r][c] = 0; }
    // colores de las particulas, iguales a los personajes del pack
    const unsigned char preset[6][3] = {
        {230, 50, 60},    // manzana roja
        {250, 150, 30},   // naranja
        {110, 200, 70},   // verde
        {150, 95, 60},    // coco cafe
        {240, 130, 220},  // gema rosa
        {235, 240, 250}   // leche blanca
    };
    for (int i = 0; i < 6; ++i)
        for (int k = 0; k < 3; ++k) gemColor[i][k] = preset[i][k];
}

GameplayState::~GameplayState() {}

void GameplayState::LoadAssets()
{
    bg = new Image();
    bg->LoadImage(CandyConfig::FILE_BG);
    btnImg = new Image();
    btnImg->LoadImage("Assets/btn.png");
    for (int i = 0; i < 6; ++i)
    {
        char path[64];
        gems[i] = new Image();
        snprintf(path, sizeof(path), "Assets/gem%d.png", i);
        gems[i]->LoadImage(path);
        bombs[i] = new Image();
        snprintf(path, sizeof(path), "Assets/gem%d_bomb.png", i);
        bombs[i]->LoadImage(path);
    }
    std::string font = ResolveFontPath();
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color yellow = { 255, 230, 120, 255 };
    SDL_Color cyan = { 120, 230, 255, 255 };
    SDL_Color cafe = { 90, 40, 20, 255 }; // botones amarillos llevan letra oscura
    scoreT = new Text(font, 36, "Score: 0", yellow);
    timeT = new Text(font, 36, "Tiempo: 90", white);
    movesT = new Text(font, 36, "Movs: 30", white);
    multT = new Text(font, 32, "x1", cyan);
    msgT = new Text(font, 30, "", white);
    undoT = new Text(font, 30, "Undos: 3", white);
    btnPauseT = new Text(font, 22, "PAUSA", cafe);
    btnUndoT = new Text(font, 22, "UNDO", cafe);
    btnMenuT = new Text(font, 22, "MENU", cafe);
}

void GameplayState::FreeAssets()
{
    delete bg; bg = nullptr;
    delete btnImg; btnImg = nullptr;
    for (int i = 0; i < 6; ++i)
    {
        delete gems[i]; gems[i] = nullptr;
        delete bombs[i]; bombs[i] = nullptr;
    }
    delete scoreT; delete timeT; delete movesT; delete multT; delete msgT; delete undoT;
    delete btnPauseT; delete btnUndoT; delete btnMenuT;
    scoreT = timeT = movesT = multT = msgT = undoT = nullptr;
    btnPauseT = btnUndoT = btnMenuT = nullptr;
}

void GameplayState::Init(Platform* p, GameStateManager* m)
{
    platform = p; manager = m;
    unsigned int seed = (unsigned int)(Platform::NowSec() * 1000.0f) + 987u;
    if (seed == 0) seed = 12345u;
    board = new Board(seed);
    destroyQueue = new LinkedQueue<CellPos>();
    history = new RingBuffer<BoardSnapshot>(CandyConfig::UNDO_CAPACITY);
    particles = new ObjectPool<Particle>(CandyConfig::PARTICLE_POOL);
    LoadAssets();
    for (int r = 0; r < Board::ROWS; ++r)
        for (int c = 0; c < Board::COLS; ++c)
        {
            float x, y;
            CellToXY(r, c, x, y);
            cellY[r][c] = y;
        }
    BoardSnapshot base;
    board->SaveTo(base, score, movesLeft);
    history->Push(base);
    SetMsg("Junta 3+ en linea. 4+ crea BOMBA.");
    RefreshTexts(true);
}

void GameplayState::Close()
{
    FreeAssets();
    delete board; board = nullptr;
    delete destroyQueue; destroyQueue = nullptr;
    delete history; history = nullptr;
    delete particles; particles = nullptr;
}

void GameplayState::SetMsg(const char* s)
{
    strncpy_s(msgBuf, s ? s : "", sizeof(msgBuf) - 1);
    msgTimer = 2.8f;
}

void GameplayState::RefreshTexts(bool force)
{
    char buf[64];
    int tsec = (timeLeft > 0) ? (int)(timeLeft + 0.99f) : 0;
    if (force || score != lastScoreShown)
    {
        snprintf(buf, sizeof(buf), "Score: %d", score);
        if (scoreT) scoreT->Update(buf);
        lastScoreShown = score;
    }
    if (force || tsec != lastTimeShown)
    {
        snprintf(buf, sizeof(buf), "Tiempo: %d", tsec);
        if (timeT) timeT->Update(buf);
        lastTimeShown = tsec;
    }
    if (force || movesLeft != lastMovesShown)
    {
        snprintf(buf, sizeof(buf), "Movs: %d", movesLeft);
        if (movesT) movesT->Update(buf);
        lastMovesShown = movesLeft;
    }
    if (force || cascadeLevel != lastMultShown)
    {
        snprintf(buf, sizeof(buf), "Combo: x%d", cascadeLevel);
        if (multT) multT->Update(buf);
        lastMultShown = cascadeLevel;
    }
    if (force || undosLeft != lastUndoShown)
    {
        snprintf(buf, sizeof(buf), "Undos: %d", undosLeft);
        if (undoT) undoT->Update(buf);
        lastUndoShown = undosLeft;
    }
    if (msgT) msgT->Update(msgBuf);
}

void GameplayState::CellToXY(int r, int c, float& x, float& y)
{
    x = (float)(CandyConfig::BOARD_X + c * CandyConfig::CELL);
    y = (float)(CandyConfig::BOARD_Y + r * CandyConfig::CELL);
}

bool GameplayState::PointToCell(float mx, float my, int& r, int& c)
{
    if (mx < CandyConfig::BOARD_X || my < CandyConfig::BOARD_Y) return false;
    c = (int)((mx - CandyConfig::BOARD_X) / CandyConfig::CELL);
    r = (int)((my - CandyConfig::BOARD_Y) / CandyConfig::CELL);
    if (r < 0 || r >= Board::ROWS || c < 0 || c >= Board::COLS) return false;
    return true;
}

void GameplayState::PushHistory()
{
    BoardSnapshot pre;
    board->SaveTo(pre, score, movesLeft);
    history->Push(pre);
}

bool GameplayState::TrySwap(int r1, int c1, int r2, int c2)
{
    if (board == nullptr) return false;
    int dr = r1 - r2; if (dr < 0) dr = -dr;
    int dc = c1 - c2; if (dc < 0) dc = -dc;
    if (dr + dc != 1) return false;
    if (!board->InBounds(r1, c1) || !board->InBounds(r2, c2)) return false;
    if (board->IsEmptyCell(r1, c1) || board->IsEmptyCell(r2, c2)) return false;

    bool bombInvolved = board->IsBomb(r1, c1) || board->IsBomb(r2, c2);
    BoardSnapshot pre;
    board->SaveTo(pre, score, movesLeft);
    board->SwapCells(r1, c1, r2, c2);

    if (bombInvolved)
    {
        // Detona la mancha de cada bomba implicada (union sin duplicar).
        bool combined[Board::ROWS][Board::COLS];
        for (int r = 0; r < Board::ROWS; ++r)
            for (int c = 0; c < Board::COLS; ++c) combined[r][c] = false;
        int cells[2][2] = { {r1, c1}, {r2, c2} };
        int total = 0;
        for (int k = 0; k < 2; ++k)
        {
            int br = cells[k][0], bc = cells[k][1];
            if (!board->IsBomb(br, bc)) continue;
            bool part[Board::ROWS][Board::COLS];
            board->DetonatePreview(br, bc, part);
            for (int r = 0; r < Board::ROWS; ++r)
                for (int c = 0; c < Board::COLS; ++c)
                    if (part[r][c] && !combined[r][c]) { combined[r][c] = true; total++; }
        }
        if (total <= 0) { board->SwapCells(r2, c2, r1, c1); return false; }
        history->Push(pre);
        movesLeft--;
        for (int r = 0; r < Board::ROWS; ++r)
            for (int c = 0; c < Board::COLS; ++c) dying[r][c] = combined[r][c];
        dyingCount = total;
        destroyQueue->Clear();
        for (int r = 0; r < Board::ROWS; ++r)
            for (int c = 0; c < Board::COLS; ++c)
                if (dying[r][c]) destroyQueue->Enqueue(CellPos(r, c));
        cascadeLevel = 1;
        selR = selC = -1;
        idleTime = 0; hintReady = false;
        BeginPopping();
        char b[64];
        snprintf(b, sizeof(b), "BOMBA! %d dulces", total);
        SetMsg(b);
        return true;
    }

    bool mark[Board::ROWS][Board::COLS];
    int n = board->FindMatches(mark);
    if (n <= 0) { board->SwapCells(r2, c2, r1, c1); return false; }

    history->Push(pre);
    movesLeft--;
    selR = selC = -1;
    idleTime = 0; hintReady = false;
    cascadeLevel = 1;
    phase = Swapping;
    swapTimer = 0;
    return true;
}

void GameplayState::BeginResolving()
{
    bool mark[Board::ROWS][Board::COLS];
    int n = board->FindMatches(mark);
    if (n <= 0) { phase = Idle; return; }
    for (int r = 0; r < Board::ROWS; ++r)
        for (int c = 0; c < Board::COLS; ++c) dying[r][c] = mark[r][c];
    dyingCount = n;
    destroyQueue->Clear();
    for (int r = 0; r < Board::ROWS; ++r)
        for (int c = 0; c < Board::COLS; ++c)
            if (dying[r][c]) destroyQueue->Enqueue(CellPos(r, c));
    BeginPopping();
}

void GameplayState::BeginPopping()
{
    phase = Popping;
    popTimer = 0;
    // VFX: una rafaga por celda condenada (del pool, sin new).
    for (int r = 0; r < Board::ROWS; ++r)
        for (int c = 0; c < Board::COLS; ++c)
        {
            if (!dying[r][c]) continue;
            float x, y;
            CellToXY(r, c, x, y);
            int col = board->Get(r, c);
            if (col < 0 || col >= 6) col = 0;
            SpawnExplosion(x + CandyConfig::CELL * 0.5f, cellY[r][c] + CandyConfig::CELL * 0.5f, col);
        }
}

void GameplayState::FinishPopping()
{
    // Puntaje con multiplicador de cascada.
    int gained = dyingCount * CandyConfig::SCORE_PER_GEM * cascadeLevel;
    bool anyBombDying = false;
    for (int r = 0; r < Board::ROWS && !anyBombDying; ++r)
        for (int c = 0; c < Board::COLS; ++c)
            if (dying[r][c] && board->IsBomb(r, c)) { anyBombDying = true; break; }
    if (anyBombDying)
        gained = dyingCount * CandyConfig::BONUS_BOMB_DETONATED_PER_GEM * cascadeLevel;
    score += gained;

    // Bomba nueva si la linea fue de 4+ (y no fue la bomba explotando).
    int keepR = -1, keepC = -1, keepColor = 0;
    if (dyingCount >= 4 && !anyBombDying)
    {
        if (selR >= 0 && dying[selR][selC]) { keepR = selR; keepC = selC; }
        else
        {
            for (int r = 0; r < Board::ROWS && keepR < 0; ++r)
                for (int c = 0; c < Board::COLS; ++c)
                    if (dying[r][c]) { keepR = r; keepC = c; break; }
        }
        if (keepR >= 0) keepColor = board->Get(keepR, keepC);
    }

    for (int r = 0; r < Board::ROWS; ++r)
        for (int c = 0; c < Board::COLS; ++c)
            if (dying[r][c]) { board->SetCell(r, c, Board::EMPTY, false); dying[r][c] = false; }

    if (keepR >= 0)
    {
        board->SetCell(keepR, keepC, keepColor < 0 ? 0 : keepColor, true);
        score += CandyConfig::BONUS_BOMB_CREATED;
        SetMsg("Bomba creada! Intercambiala para detonar.");
    }
    else if (cascadeLevel > 1)
    {
        char b[64];
        snprintf(b, sizeof(b), "Cascada x%d! +%d", cascadeLevel, gained);
        SetMsg(b);
    }
    dyingCount = 0;
    destroyQueue->Clear();
    BeginFalling();
}

void GameplayState::BeginFalling()
{
    // Mapeo visual: cada gema superviviente conserva su Y visual; las
    // nuevas entran desde arriba. Luego ApplyGravity (logica) y fase Falling.
    float oldY[Board::ROWS][Board::COLS];
    for (int r = 0; r < Board::ROWS; ++r)
        for (int c = 0; c < Board::COLS; ++c) oldY[r][c] = cellY[r][c];

    int src[Board::COLS][Board::ROWS];
    int srcN[Board::COLS];
    for (int c = 0; c < Board::COLS; ++c)
    {
        srcN[c] = 0;
        for (int r = 0; r < Board::ROWS; ++r)
            if (!board->IsEmptyCell(r, c)) src[c][srcN[c]++] = r;
    }
    board->ApplyGravity();
    for (int c = 0; c < Board::COLS; ++c)
    {
        int n = srcN[c];
        for (int i = 0; i < n; ++i)
        {
            int dst = Board::ROWS - n + i;
            int s = src[c][i];
            cellY[dst][c] = oldY[s][c];
        }
        for (int r = 0; r < Board::ROWS - n; ++r)
            cellY[r][c] = (float)(CandyConfig::BOARD_Y - (Board::ROWS - n - r) * CandyConfig::CELL);
    }
    phase = Falling;
}

void GameplayState::SpawnExplosion(float cx, float cy, int colorIdx)
{
    if (particles == nullptr) return;
    if (colorIdx < 0 || colorIdx >= 6) colorIdx = 0;
    for (int i = 0; i < 14; ++i)
    {
        Particle* p = particles->Alloc();
        if (p == nullptr) return; // pool lleno: se dropea, no se traba
        // Abanico determinista sin libm: alterna X y sube con gravedades
        // distintas para que se vea organico.
        p->x = cx; p->y = cy;
        p->vx = ((i % 2 == 0) ? 1.0f : -1.0f) * (20.0f + (float)((i * 53) % 140));
        p->vy = -60.0f - (float)((i * 29) % 180);
        p->maxLife = 0.45f + (float)(i % 4) * 0.08f;
        p->life = p->maxLife;
        p->size = 5.0f + (float)(i % 3) * 3.0f;
        p->r = gemColor[colorIdx][0];
        p->g = gemColor[colorIdx][1];
        p->b = gemColor[colorIdx][2];
        p->gravity = true;
    }
}

void GameplayState::UpdateParticles(float dt)
{
    if (particles == nullptr) return;
    for (int i = 0; i < particles->Capacity(); ++i)
    {
        if (!particles->IsAlive(i)) continue;
        Particle& p = particles->Get(i);
        p.life -= dt;
        if (p.life <= 0) { particles->FreeAt(i); continue; }
        if (p.gravity) p.vy += 520.0f * dt;
        p.x += p.vx * dt;
        p.y += p.vy * dt;
    }
}

void GameplayState::DrawParticles()
{
    if (particles == nullptr) return;
    for (int i = 0; i < particles->Capacity(); ++i)
    {
        if (!particles->IsAlive(i)) continue;
        const Particle& p = particles->Get(i);
        float f = p.life / p.maxLife;
        if (f < 0) f = 0; if (f > 1) f = 1;
        unsigned char a = (unsigned char)(200 * f + 55);
        float s = p.size * (0.5f + 0.5f * f);
        platform->FillRect(p.x - s * 0.5f, p.y - s * 0.5f, s, s, p.r, p.g, p.b, a);
    }
}

void GameplayState::ComputeHintIfNeeded()
{
    if (hintReady || phase != Idle) return;
    if (idleTime < CandyConfig::HINT_IDLE_TIME) return;
    int a, b, cc, d;
    if (board->FindHint(a, b, cc, d))
    {
        hintR1 = a; hintC1 = b; hintR2 = cc; hintC2 = d;
        hintReady = true;
        hintPulse = 0;
    }
}

bool GameplayState::Input(ListaT<int>* keyDowns, ListaT<int>* keyUps, bool* leftclick, float* mouseX, float* mouseY, bool* quit)
{
    if (quit != nullptr && *quit) { manager->RequestQuit(); return true; }
    for (int i = 0; i < keyDowns->size; ++i)
    {
        int k = keyDowns->get_at(i)->value;
        if (k == SDLK_ESCAPE || k == SDLK_P) { manager->RequestPush(new PauseState()); return true; }
        if (k == SDLK_M) { manager->RequestPop(); return true; }
        if (k == SDLK_R) { manager->RequestReplace(new GameplayState()); return true; }
        if (k == SDLK_U)
        {
            if (phase == Idle && undosLeft > 0 && history != nullptr && history->Size() > 0)
            {
                BoardSnapshot snap;
                if (history->PopRecent(snap))
                {
                    board->LoadFrom(snap);
                    score = snap.score; movesLeft = snap.moves;
                    undosLeft--;
                    selR = selC = -1;
                    destroyQueue->Clear();
                    for (int r = 0; r < Board::ROWS; ++r)
                        for (int c = 0; c < Board::COLS; ++c)
                        {
                            dying[r][c] = false;
                            float x, y;
                            CellToXY(r, c, x, y);
                            cellY[r][c] = y;
                        }
                    cascadeLevel = 1;
                    SetMsg("Movimiento deshecho.");
                    RefreshTexts(true);
                }
            }
            else SetMsg(undosLeft <= 0 ? "Sin undos restantes." : "Undo solo en reposo.");
            return true;
        }
    }
    // Botones (siempre activos)
    if (leftclick != nullptr && *leftclick && mouseX != nullptr && mouseY != nullptr)
    {
        float mx = *mouseX, my = *mouseY;
        UiButton bP(1760, 20, 140, 54, btnPauseT, btnImg);
        UiButton bU(1760, 84, 140, 54, btnUndoT, btnImg);
        UiButton bM(1760, 148, 140, 54, btnMenuT, btnImg);
        if (bP.Contains(mx, my)) { manager->RequestPush(new PauseState()); return true; }
        if (bU.Contains(mx, my))
        {
            ListaT<int> fake; fake.push_back(SDLK_U);
            ListaT<int> fakeUp;
            bool lc = false; float x = 0, y = 0; bool q = false;
            // Reusa la logica de teclado para no duplicar:
            (void)fakeUp; (void)lc; (void)x; (void)y; (void)q;
            if (phase == Idle && undosLeft > 0 && history != nullptr && history->Size() > 0)
            {
                BoardSnapshot snap;
                if (history->PopRecent(snap))
                {
                    board->LoadFrom(snap);
                    score = snap.score; movesLeft = snap.moves;
                    undosLeft--;
                    selR = selC = -1;
                    destroyQueue->Clear();
                    for (int r = 0; r < Board::ROWS; ++r)
                        for (int c = 0; c < Board::COLS; ++c)
                        {
                            dying[r][c] = false;
                            float xx, yy;
                            CellToXY(r, c, xx, yy);
                            cellY[r][c] = yy;
                        }
                    cascadeLevel = 1;
                    SetMsg("Movimiento deshecho.");
                    RefreshTexts(true);
                }
            }
            else SetMsg(undosLeft <= 0 ? "Sin undos restantes." : "Undo solo en reposo.");
            return true;
        }
        if (bM.Contains(mx, my)) { manager->RequestPop(); return true; }

        // Tablero (solo en reposo)
        if (phase == Idle)
        {
            int r, c;
            if (PointToCell(mx, my, r, c))
            {
                idleTime = 0; hintReady = false;
                if (selR < 0) { selR = r; selC = c; }
                else if (selR == r && selC == c) { selR = selC = -1; }
                else
                {
                    int dr = selR - r; if (dr < 0) dr = -dr;
                    int dc = selC - c; if (dc < 0) dc = -dc;
                    if (dr + dc == 1)
                    {
                        if (!TrySwap(selR, selC, r, c))
                        {
                            selR = r; selC = c; // reselecciona si el swap fue ilegal
                        }
                    }
                    else { selR = r; selC = c; }
                }
                return true;
            }
            else { selR = selC = -1; }
        }
    }
    (void)keyUps;
    return false;
}

void GameplayState::Update(float dt)
{
    if (timeLeft > 0) timeLeft -= dt;
    if (timeLeft < 0) timeLeft = 0;
    idleTime += dt;
    hintPulse += dt;
    if (msgTimer > 0) { msgTimer -= dt; if (msgTimer <= 0) msgBuf[0] = '\0'; }
    ComputeHintIfNeeded();
    UpdateParticles(dt);

    // Caida suave: cellY -> objetivo logico.
    if (phase == Falling)
    {
        bool settled = true;
        float k = dt * CandyConfig::FALL_LERP;
        if (k > 1) k = 1;
        for (int r = 0; r < Board::ROWS; ++r)
            for (int c = 0; c < Board::COLS; ++c)
            {
                float target = (float)(CandyConfig::BOARD_Y + r * CandyConfig::CELL);
                float d = target - cellY[r][c];
                if (d < -2 || d > 2) settled = false;
                cellY[r][c] += d * k;
            }
        if (settled)
        {
            for (int r = 0; r < Board::ROWS; ++r)
                for (int c = 0; c < Board::COLS; ++c)
                    cellY[r][c] = (float)(CandyConfig::BOARD_Y + r * CandyConfig::CELL);
            // Cascadas con la Queue: si la gravedad formo otro match,
            // se encola con multiplicador x2, x3...
            bool mark[Board::ROWS][Board::COLS];
            if (board->FindMatches(mark) > 0)
            {
                cascadeLevel++;
                BeginResolving();
            }
            else
            {
                cascadeLevel = 1;
                phase = Idle;
                if (!board->HasPossibleMove())
                {
                    board->ShuffleNoMatch();
                    for (int r = 0; r < Board::ROWS; ++r)
                        for (int c = 0; c < Board::COLS; ++c)
                            cellY[r][c] = (float)(CandyConfig::BOARD_Y + r * CandyConfig::CELL);
                    SetMsg("Sin movimientos: tablero mezclado.");
                }
                if (movesLeft <= 0 || timeLeft <= 0)
                {
                    manager->RequestReplace(new GameOverState(score));
                }
            }
        }
    }
    else if (phase == Swapping)
    {
        swapTimer += dt;
        if (swapTimer >= CandyConfig::SWAP_TIME) BeginResolving();
    }
    else if (phase == Popping)
    {
        popTimer += dt;
        if (popTimer >= CandyConfig::POP_TIME) FinishPopping();
    }
    else // Idle
    {
        if (movesLeft <= 0 || timeLeft <= 0)
        {
            manager->RequestReplace(new GameOverState(score));
        }
    }
    RefreshTexts(false);
}

void GameplayState::Draw()
{
    platform->RenderClear();
    if (bg != nullptr && bg->IsValid())
        platform->RenderImageScaled(bg, 0, 0, (float)platform->width, (float)platform->height, 0);

    // Panel izquierdo
    platform->FillRect(60, 130, 440, 560, 20, 10, 35, 220);
    platform->FrameRect(60, 130, 440, 560, 255, 150, 210);
    if (scoreT) scoreT->Display(90, 150);
    if (timeT) timeT->Display(90, 210);
    if (movesT) movesT->Display(90, 270);
    if (multT) multT->Display(90, 330);
    if (undoT) undoT->Display(90, 390);
    if (msgT && msgBuf[0] != '\0') msgT->Display(90, 450);

    // Tablero
    platform->FillRect((float)(CandyConfig::BOARD_X - 12), (float)(CandyConfig::BOARD_Y - 12),
                       (float)(CandyConfig::BOARD_W + 24), (float)(CandyConfig::BOARD_H + 24),
                       35, 18, 55, 255);
    platform->FrameRect((float)(CandyConfig::BOARD_X - 12), (float)(CandyConfig::BOARD_Y - 12),
                        (float)(CandyConfig::BOARD_W + 24), (float)(CandyConfig::BOARD_H + 24),
                        255, 150, 210);

    float gemSize = (float)CandyConfig::CELL * 0.88f;
    for (int r = 0; r < Board::ROWS; ++r)
    {
        for (int c = 0; c < Board::COLS; ++c)
        {
            int v = board->Get(r, c);
            if (v < 0 || v >= 6) continue;
            Image* img = board->IsBomb(r, c) ? bombs[v] : gems[v];
            float x = (float)(CandyConfig::BOARD_X + c * CandyConfig::CELL);
            float y = cellY[r][c];
            float scale = 1.0f;
            if (phase == Popping && dying[r][c])
            {
                float f = popTimer / CandyConfig::POP_TIME;
                if (f > 1) f = 1;
                scale = 1.0f - f * 0.8f;
            }
            // las bombas palpitan para que se vean especiales
            if (board->IsBomb(r, c))
            {
                int tick = ((int)(hintPulse * 3.0f + r + c)) % 2;
                if (tick == 0) scale = scale * 1.07f;
            }
            float dw = gemSize * scale;
            float dx = x + (CandyConfig::CELL - dw) * 0.5f;
            float dy = y + (CandyConfig::CELL - dw) * 0.5f;
            if (img != nullptr && img->IsValid())
                platform->RenderImageScaled(img, dx, dy, dw, dw, 0);
            else
                platform->FillRect(dx, dy, dw, dw, gemColor[v][0], gemColor[v][1], gemColor[v][2], 255);
        }
    }
    // Resaltados
    if (selR >= 0 && phase == Idle)
    {
        float x = (float)(CandyConfig::BOARD_X + selC * CandyConfig::CELL);
        platform->FrameRect(x + 2, cellY[selR][selC] + 2, (float)CandyConfig::CELL - 4, (float)CandyConfig::CELL - 4, 255, 235, 120);
        platform->FrameRect(x + 5, cellY[selR][selC] + 5, (float)CandyConfig::CELL - 10, (float)CandyConfig::CELL - 10, 255, 235, 120);
    }
    if (hintReady && phase == Idle && idleTime >= CandyConfig::HINT_IDLE_TIME)
    {
        bool blink = ((int)(hintPulse * 3.0f) % 2) == 0;
        if (blink)
        {
            float x1 = (float)(CandyConfig::BOARD_X + hintC1 * CandyConfig::CELL);
            float x2 = (float)(CandyConfig::BOARD_X + hintC2 * CandyConfig::CELL);
            platform->FrameRect(x1 + 2, cellY[hintR1][hintC1] + 2, (float)CandyConfig::CELL - 4, (float)CandyConfig::CELL - 4, 120, 255, 170);
            platform->FrameRect(x2 + 2, cellY[hintR2][hintC2] + 2, (float)CandyConfig::CELL - 4, (float)CandyConfig::CELL - 4, 120, 255, 170);
        }
    }
    DrawParticles();

    // Botones derecha
    UiButton bP(1760, 20, 140, 54, btnPauseT, btnImg);
    UiButton bU(1760, 84, 140, 54, btnUndoT, btnImg);
    UiButton bM(1760, 148, 140, 54, btnMenuT, btnImg);
    float mx = platform->lastmouseX, my = platform->lastmouseY;
    bP.Draw(platform, bP.Contains(mx, my));
    bU.Draw(platform, bU.Contains(mx, my));
    bM.Draw(platform, bM.Contains(mx, my));

    platform->RenderPresent();
}
