#include "HighScores.h"
#include <cstdio>
#include <cstring>

HighScores::HighScores()
{
    _count = 0;
    for (int i = 0; i < MAX; ++i) { _entries[i].score = 0; _entries[i].name[0] = '\0'; }
}

void HighScores::Load(const std::string& path)
{
    _count = 0;
    FILE* f = nullptr;
    fopen_s(&f, path.c_str(), "r");
    if (!f) return;
    char name[16];
    int score = 0;
    while (_count < MAX && fscanf_s(f, "%d %15s", &score, name, (unsigned)_countof(name)) == 2)
    {
        _entries[_count].score = score;
        strncpy_s(_entries[_count].name, name, 15);
        _count++;
    }
    fclose(f);
}

void HighScores::Save(const std::string& path)
{
    FILE* f = nullptr;
    fopen_s(&f, path.c_str(), "w");
    if (!f) return;
    for (int i = 0; i < _count; ++i)
        fprintf(f, "%d %s\n", _entries[i].score, _entries[i].name);
    fclose(f);
}

int HighScores::Qualifies(int score) const
{
    if (score <= 0) return -1;
    if (_count < MAX) {
        for (int i = 0; i < _count; ++i)
            if (score > _entries[i].score) return i;
        return _count;
    }
    for (int i = 0; i < MAX; ++i)
        if (score > _entries[i].score) return i;
    return -1;
}

void HighScores::Insert(const std::string& name, int score)
{
    int pos = Qualifies(score);
    if (pos < 0 || pos >= MAX) return;
    int limit = (_count < MAX) ? _count : MAX - 1;
    for (int i = limit; i > pos; --i)
        _entries[i] = _entries[i - 1];
    _entries[pos].score = score;
    strncpy_s(_entries[pos].name, name.c_str(), 15);
    if (_count < MAX) _count++;
}
