#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "player.h"
#include "../main.h"
#include "structs.c"

#define SLEEP_TIME 2
#define MAX_RUN_TIME 100
#define Warrior &warrior
#define LVL_HEADER "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LEVEL ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
#define TURN_L "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~~ "
#define TURN_R " ~~ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
#define WALL 'W'

int isLevelOn = 1;
int timeBonus;
int aceScore;
int score = 0;
struct Object *field[10][20];

#include "methods.c"
#include "../towers/baby-steps/index.c"
#include "../data/Player.c"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#define HEART L"\u2665"
#define SCORE L"\u2666"
#define UP_LEFT L"\u2554"
#define UP_RIGHT L"\u2557"
#define DOWN_LEFT L"\u255a"
#define DOWN_RIGHT L"\u255d"
#define VER L"\u2550"
#define HOR L"\u2551"
#define __APPLE__ 0
#else
#define HEART "\u2665"
#define SCORE "\u2666"
#define UP_LEFT "\u2554"
#define UP_RIGHT "\u2557"
#define DOWN_LEFT "\u255a"
#define DOWN_RIGHT "\u255d"
#define VER "\u2550"
#define HOR "\u2551"
#define _WIN32 0
#endif

void setupField(int level);
void setupObjects();
void draw(int level);
void show(int level);
void check(int level);
void objectsRun(int level);

int play(int level)
{
    objConstructor();
    levelConstructor();
    setupObjects();
    setupField(level);
    puts(LVL_HEADER);

    printf("%s000%s\n", TURN_L, TURN_R);
    draw(level);
    sleep(SLEEP_TIME);
    int counter = 1;
    while (isLevelOn)
    {
        char turn[80];
        char step[3];
        int stp = counter++;
        snprintf(step, 4, "%03d", stp);
        strcpy(turn, TURN_L);
        strcat(turn, step);
        strcat(turn, TURN_R);
        puts(turn);
        playTurn();
        objectsRun(level);
        draw(level);
        check(level);
        sleep(SLEEP_TIME);
    }
}

void draw(int level)
{
    if (__APPLE__)
    {
        printf("%s %d\n", HEART, warrior.health);
        printf("%s %d\n", SCORE, score);
        printf("%s", UP_LEFT);
        for (int j = 0; j < levels[level].width; j++)
        {
            printf("%s", VER);
        }
        printf("%s\n", UP_RIGHT);
        for (int i = 0; i < levels[level].height; i++)
        {
            printf("%s", HOR);
            for (int j = 0; j < levels[level].width; j++)
            {
                printf("%c", field[i + 1][j + 1]->character);
            }
            printf("%s\n", HOR);
        }
        printf("%s", DOWN_LEFT);
        for (int j = 0; j < levels[level].width; j++)
        {
            printf("%s", VER);
        }
        printf("%s\n", DOWN_RIGHT);
    }
    else
    {
        _setmode(_fileno(stdout), 0x00020000);
        wprintf(HEART);
        _setmode(_fileno(stdout), 0x4000);
        printf(" %d\n", warrior.health);

        _setmode(_fileno(stdout), 0x00020000);
        wprintf(SCORE);
        _setmode(_fileno(stdout), 0x4000);
        printf(" %d\n", score);

        _setmode(_fileno(stdout), 0x00020000);
        wprintf(UP_LEFT);
        _setmode(_fileno(stdout), 0x4000);
        for (int j = 0; j < levels[level].width; j++)
        {
            _setmode(_fileno(stdout), 0x00020000);
            wprintf(VER);
            _setmode(_fileno(stdout), 0x4000);
        }

        _setmode(_fileno(stdout), 0x00020000);
        wprintf(UP_RIGHT);
        _setmode(_fileno(stdout), 0x4000);
        puts("");

        for (int i = 0; i < levels[level].height; i++)
        {
            _setmode(_fileno(stdout), 0x00020000);
            wprintf(HOR);
            _setmode(_fileno(stdout), 0x4000);
            for (int j = 0; j < levels[level].width; j++)
            {
                printf("%c", field[i + 1][j + 1]->character);
            }
            _setmode(_fileno(stdout), 0x00020000);
            wprintf(HOR);
            _setmode(_fileno(stdout), 0x4000);
            puts("");
        }

        _setmode(_fileno(stdout), 0x00020000);
        wprintf(DOWN_LEFT);
        _setmode(_fileno(stdout), 0x4000);
        for (int j = 0; j < levels[level].width; j++)
        {
            _setmode(_fileno(stdout), 0x00020000);
            wprintf(VER);
            _setmode(_fileno(stdout), 0x4000);
        }
        _setmode(_fileno(stdout), 0x00020000);
        wprintf(DOWN_RIGHT);
        _setmode(_fileno(stdout), 0x4000);
        puts("");
    }
}

void setupField(int level)
{

    for (int i = 0; i <= levels[level].width + 1; i++)
    {
        field[0][i] = &wall;
    }

    for (int i = 1; i <= levels[level].height; i++)
    {
        field[i][0] = &wall;

        for (int j = 1; j <= levels[level].width; j++)
        {
            field[i][j] = &space;
        }
        field[i][levels[level].width + 1] = &wall;
    }

    puts("yellow");

    for (int i = 0; i <= levels[level].width + 1; i++)
    {
        field[levels[level].height + 1][i] = &wall;
    }

    field[levels[level].stairY][levels[level].stairX] = &stairs;

    for (int i = 0; i < levels[level].objsLength; i++)
    {
        field[levels[level].thisLevelsObjs[i]->locY][levels[level].thisLevelsObjs[i]->locX] = levels[level].thisLevelsObjs[i];
    }

    field[warrior.locY][warrior.locX] = &warrior;
}

void show(int level)
{
    for (int j = 0; j <= levels[level].height + 1; j++)
    {
        for (int i = 0; i <= levels[level].width + 1; i++)
        {
            printf("%s ,", field[j][i]->name);
        }
        puts("");
    }
}

void check(int level)
{
    if (warrior.locX == levels[level].stairX && warrior.locY == levels[level].stairY)
    {
        isLevelOn = 0;
        printf("%s", "LEVEL Done!");
        generateLevel(level + 2, 0);
    }
    else if (warrior.health == 0)
    {
        isLevelOn = 0;
        printf("Warrior Died!, you failed level %d", level + 1);
    }
}

void setupObjects()
{
    warrior.character = '@';
    warrior.health = 20;
    warrior.locX = 1;
    warrior.locY = 1;
    warrior.enemy = 0;
    warrior.face = EAST;
    strcpy(warrior.name, "Warrior");

    wall.character = 'W';
    wall.enemy = 0;
    strcpy(wall.name, "Wall");

    space.character = ' ';
    space.enemy = 0;
    strcpy(space.name, "Space");

    stairs.character = '>';
    stairs.enemy = 0;
    strcpy(stairs.name, "Stairs");
}

void objectsRun(int level)
{
    for (int i = 0; i < levels[level].objsLength; i++)
    {
        levels[level].thisLevelsObjs[0]->run(levels[level].thisLevelsObjs[0]);
    }
}