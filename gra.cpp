// Gra_JPRO Hubert Klim.cpp : Ten plik zawiera funkcjê „main”. W nim rozpoczyna siê i koñczy wykonywanie programu.
//

#include <Windows.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <WinUser.h>
#include<dos.h>
using namespace std;

#define INDEX
int ITEM_ID = 1;
int ENEMY_COUNT = 0;
char** mapa = NULL;
char** origin_map = NULL;

int SPAWNER_COUNT = 0;

enum klasy
{
    warrior,
    mage,
    ranger,
    duelist,
    paladin,
    templar
};

enum klasa_przedmiotu {
    armor,
    weapon,
    potions,
    materials,
};

enum direction {
    up,
    down,
    right,
    left
};

enum colision_element {
    wall = 35,
    e = 101,
    //p1 = 60,
    //p2 = 62,
    //p3 = 94,
    //p4 = 118
};

enum entity_type {
    player,
    enemy
};

struct bohater {
    int HP;
    int strength;
    int dexterity;
    int intelligence;
    int movementspeed;
    int luck;
    int armor;
    int experience;
    enum entity_type type;

    int coord_x;
    int coord_y;
    enum direction direction;

    void (*move)(bohater*& character, bohater*& player);
};

struct spawner {
    int pos_x;
    int pos_y;
};
spawner* spawner_list = NULL;

struct przedmiot {
    int id;
    char name[15];
    enum klasa_przedmiotu klasa;
    int price;

    int height;
    int width;
    int weigth;

    int pos_x;
    int pos_y;

    float atk_speed;
    int dmg;

};

struct przeciwnik {
    int health;
    int dmg;

    int pos_x;
    int pos_y;

    char name[15];
};

bohater* getEnemy(int list_size, bohater* list[], int posX, int posY) {
    for (int i = 0; i < list_size; i++) {
        if (list[i]->coord_x == posX) {
            if (list[i]->coord_y == posY) {
                return list[i];
            }
        }
    }
    return 0;
}


//checks if map element entity wants to walk to is
// of type colision_element
int doesColide(char elem) {
    for (int i = wall; i <= e; i++) {
        if (i == elem) {
            return 1;
        }
    }
    return 0;
}

//indexToRemove ranges from 0 - arraySize
void removeArrayIndex(int arraySize, bohater* list[], int indexToRemove) {
    for (int i = indexToRemove; i < arraySize - 1; i++) {
        list[i] = list[i + 1];
    }
}

void startFight(bohater*& player, bohater*& enemy) {
    while (player->HP >= 0 && enemy->HP >= 0) {
        //player attack
        int crit;
        int num;

        srand(time(0));
        num = rand() % (100 / player->luck);
        printf("num is: %d\n", num);
        if (num == 1) {
            crit = 2;
            printf("PLAYER CRITICAL HIT\n");
        }
        else {
            crit = 1;
        }
        enemy->HP = enemy->HP - ((player->strength * crit) * (1 - enemy->armor / 20));

        //enemy attack
        srand(time(0));
        num = rand() % (100 / enemy->luck);
        printf("num is: %d\n", num);
        if (num == 1) {
            crit = 2;
            printf("ENEMY CRITICAL HIT\n");
        }
        else {
            crit = 1;
        }
        player->HP = player->HP - ((enemy->strength * crit) * (1 - player->armor / 20));
    }
}

void check_for_enemy(bohater*& character, bohater* list[]) {
    switch (character->direction) {
    case direction::right:
        if (doesColide(mapa[character->coord_y][character->coord_x + 1])) {
            for (int i = 0; i < ENEMY_COUNT - 1; i++) {
                if (list[i]->coord_x == character->coord_x + 1 && list[i]->coord_y == character->coord_y) {
                    //list[i]->HP = list[i]->HP - 10;
                    startFight(character, list[i]);
                    //printf("damage dealt");
                }
            }
        }
        break;
    case direction::left:
        if (doesColide(mapa[character->coord_y][character->coord_x - 1])) {
            for (int i = 0; i < ENEMY_COUNT - 1; i++) {
                if (list[i]->coord_x == character->coord_x - 1 && list[i]->coord_y == character->coord_y) {
                    startFight(character, list[i]);
                    //list[i]->HP = list[i]->HP - 10;
                    //printf("damage dealt");
                }
            }
        }
        break;

    case direction::down:
        if (mapa[character->coord_y + 1][character->coord_x] == 'e') {
            for (int i = 0; i < ENEMY_COUNT - 1; i++) {
                if (list[i]->coord_x == character->coord_x && list[i]->coord_y == character->coord_y + 1) {
                    startFight(character, list[i]);
                    //list[i]->HP = list[i]->HP - 10;
                    //printf("damage dealt");
                }
            }
        }
        break;

    case direction::up:
        if (mapa[character->coord_y - 1][character->coord_x] == 'e') {
            for (int i = 0; i < ENEMY_COUNT - 1; i++) {
                if (list[i]->coord_x == character->coord_x && list[i]->coord_y == character->coord_y - 1) {
                    startFight(character, list[i]);
                    //list[i]->HP = list[i]->HP - 10;
                    //printf("damage dealt");
                }
            }
        }
        break;
    }


}

void create_map(int width, int height, char** mapa) {
    // ----- MAPA -----
   /* mapa = (char**)malloc(width * sizeof(char*));
    for (int i = 0; i < width; i++) {
        mapa[i] = (char*)malloc(height * sizeof(char));
    }*/

    int column = 0;
    int row = 0;

    char map_element;
    FILE* map_file;
    errno_t err;

    err = fopen_s(&map_file, "./mapa.txt", "r");
    if (err != 0) {
        cout << "Coœ nie dziala";
    }
    else {
        while (!feof(map_file)) {
            map_element = getc(map_file);
            //cout << map_element;

            if (map_element == '\n') {
                //mapa[column][row] = '\n';
                column = 0;
                row++;
            }
            else if (map_element == '@') {
                spawner enemy_spawn;
                enemy_spawn.pos_x = column;
                enemy_spawn.pos_y = row;
                spawner_list[SPAWNER_COUNT] = enemy_spawn;
                SPAWNER_COUNT++;
                mapa[row][column] = ' ';
                column++;
            }
            else {
                mapa[row][column] = map_element;
                column++;
            }
        }
        cout << endl;
    }

    fclose(map_file);
}

void item_spawn(przedmiot& tworzony_przedmiot) {
    tworzony_przedmiot.id = ITEM_ID;
    ITEM_ID++;
    strcpy_s(tworzony_przedmiot.name, "axe");
    tworzony_przedmiot.klasa = weapon;
    tworzony_przedmiot.price = 30;


    tworzony_przedmiot.height = 2;
    tworzony_przedmiot.width = 1;
    //tworzony_przedmiot.pos_x
    //tworzony_przedmiot.pos_y

    tworzony_przedmiot.weigth = 3;

    tworzony_przedmiot.atk_speed = 1.0;
    tworzony_przedmiot.dmg = 5;
}

void insert_EQ(przedmiot* inventory[][10], int env[][10], przedmiot* inventory_order[100], przedmiot* item) {
    //dodanie wskaŸnika do listy EQ 
    inventory[0][0] = item;

    //Wype³nienie listy "equipment" id przedmiotu
    for (int i = 0; i < item->height; i++) {
        for (int j = 0; j < item->width; j++) {
            env[i][j] = item->id;
        }
    }

}

void SetCursorPosition(int XPos, int YPos) {
    COORD coord;
    coord.X = XPos; coord.Y = YPos;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void character_move(bohater*& character, bohater*& player) {
    if (character->type != entity_type::player) {
        if ((character->coord_x > player->coord_x) && (!doesColide(mapa[character->coord_y][character->coord_x - 1]))) {
            character->direction = direction::left;
        }
        else if ((character->coord_x < player->coord_x) && !doesColide(mapa[character->coord_y][character->coord_x + 1])) {
            character->direction = direction::right;
        }
        else if (character->coord_y < player->coord_y) {
            character->direction = direction::down;
        }
        else if (character->coord_y > player->coord_y) {
            character->direction = direction::up;
        }
    }
    switch (character->direction) {
    case direction::right:
        if (!doesColide(mapa[character->coord_y][character->coord_x + 1])) {
            character->coord_x = character->coord_x + 1;
        }
        else if (doesColide(mapa[character->coord_y][character->coord_x + 1])) {
            character->direction = direction::left;
        }
        break;
    case direction::left:
        if (!doesColide(mapa[character->coord_y][character->coord_x - 1])) {
            character->coord_x = character->coord_x - 1;
        }
        else if (doesColide(mapa[character->coord_y][character->coord_x - 1])) {
            character->direction = direction::right;
        }
        break;

    case direction::down:
        if (!doesColide(mapa[character->coord_y + 1][character->coord_x])) {
            character->coord_y = character->coord_y + 1;
        }
        else if (doesColide(mapa[character->coord_y + 1][character->coord_x])) {
            character->direction = direction::up;
        }
        break;

    case direction::up:
        if (!doesColide(mapa[character->coord_y - 1][character->coord_x])) {
            character->coord_y = character->coord_y - 1;
        }
        else if (doesColide(mapa[character->coord_y - 1][character->coord_x])) {
            character->direction = direction::down;
        }
        break;
    }


}

void enemy_spawn(bohater*& enemy) {
    ENEMY_COUNT++;
    enemy = (bohater*)malloc(sizeof(bohater));
    enemy->HP = 20;
    enemy->strength = 5;
    enemy->luck = 30;
    enemy->armor = 10;
    enemy->move = character_move;
    enemy->direction = direction::right;
    enemy->type = entity_type::enemy;
}

void bohater_spawn(bohater*& losowy_bohater) {
    //bohater losowy_bohater;
    losowy_bohater = (bohater*)malloc(sizeof(bohater));
    losowy_bohater->HP = 100;
    losowy_bohater->strength = 10;
    losowy_bohater->dexterity = 5;
    losowy_bohater->intelligence = 5;
    losowy_bohater->luck = 25;
    losowy_bohater->armor = 15;
    losowy_bohater->movementspeed = 10;
    losowy_bohater->experience = 0;
    losowy_bohater->move = character_move;
    losowy_bohater->type = entity_type::player;

};

void character_spawn(bohater*& new_character, void(*spawn_func)(bohater*&), int x_pos, int y_pos) {
    spawn_func(new_character);
    new_character->coord_x = x_pos;
    new_character->coord_y = y_pos;
}


int main()
{


    // ----- OTWIERANIE PLIKU Z MAP¥ -----
    //rozmiar
    int w = 22;
    int k = 11;

    int max_spawners = 5;

    spawner_list = (spawner*)malloc(max_spawners * sizeof(spawner));

    mapa = (char**)malloc(w * sizeof(char*));
    for (int i = 0; i < w; i++) {
        mapa[i] = (char*)malloc(k * sizeof(char));
    }
    origin_map = (char**)malloc(w * sizeof(char*));
    for (int i = 0; i < w; i++) {
        origin_map[i] = (char*)malloc(k * sizeof(char));

    }
    create_map(w, k, mapa);
    create_map(w, k, origin_map);

    ENEMY_COUNT = ENEMY_COUNT / 2;
    SPAWNER_COUNT = SPAWNER_COUNT / 2;
    // ----- EQUIPMENT -----

    przedmiot* EQ[10][10];
    int equipment[10][10];
    przedmiot* EQ_order[100];

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            equipment[i][j] = 0;
        }
    }

    bohater* nowy_bohater;
    character_spawn(nowy_bohater, bohater_spawn, 3, 5);

    //przedmiot nowy_przedmiot;
    //item_spawn(nowy_przedmiot);
    //insert_EQ(EQ, equipment, EQ_order, &nowy_przedmiot);

    //--------- CREATING ENEMYS ---------
    int max_enemy = 10;
    int enemy_amount = 0;
    bohater** enemy_list = (bohater**)malloc(max_enemy * sizeof(bohater*));

    for (int i = 0; i < SPAWNER_COUNT; i++) {
        bohater* enemy;
        character_spawn(enemy, enemy_spawn, spawner_list[i].pos_x, spawner_list[i].pos_y);
        enemy_list[i] = enemy;
        enemy_amount++;
    }


    //-------------MAIN LOOP---------------
    char pressed_key;
    int toggle_eq = 0;
    int loop_count = 0;
    int game_loop = 1;
    while (game_loop) {

        SetCursorPosition(0, 0);
        //clrscr();


        // i -> rzêdy j -> kolumny
        for (int i = 0; i < k - 1; i++) {
            for (int j = 0; j < w - 2; j++) {

                for (int e = 0; e <= ENEMY_COUNT - 1; e++) {
                    if (i == enemy_list[e]->coord_y && j == enemy_list[e]->coord_x) {
                        printf("e");
                        mapa[i][j] = 'e';
                        j++;
                    }
                }


                if (i == nowy_bohater->coord_y && j == nowy_bohater->coord_x) {


                    switch (nowy_bohater->direction)
                    {
                    case direction::up:
                        cout << '^';
                        mapa[i][j] = '^';
                        break;
                    case direction::down:
                        cout << 'v';
                        mapa[i][j] = 'v';
                        break;
                    case direction::left:
                        cout << '<';
                        mapa[i][j] = '<';
                        break;
                    case direction::right:
                        cout << '>';
                        mapa[i][j] = '>';
                        break;
                    default:
                        cout << '^';
                        break;
                    }
                }
                else {

                    mapa[i][j] = origin_map[i][j];
                    cout << mapa[i][j];
                }

            }
            printf("\n");
        }

        printf("Player has %d HP\n", nowy_bohater->HP);
        for (int i = 0; i <= ENEMY_COUNT - 1; i++) {
            printf("Enemy of id:%d has %d HP\n", i, enemy_list[i]->HP);
        }


        //--------- HANDLING ENEMY ---------
        for (int e = 0; e <= ENEMY_COUNT - 1; e++) {
            if (enemy_list[e]->HP <= 0) {
                mapa[enemy_list[e]->coord_y][enemy_list[e]->coord_x] = ' ';
                free(enemy_list[e]);
                removeArrayIndex(ENEMY_COUNT, enemy_list, e);
                ENEMY_COUNT = ENEMY_COUNT - 1;
            }
            else if (enemy_list[e]->coord_x == nowy_bohater->coord_x && enemy_list[e]->coord_y == nowy_bohater->coord_y) {
                startFight(nowy_bohater, enemy_list[e]);
            }
            else {
                enemy_list[e]->move(enemy_list[e], nowy_bohater);
            }
        }

        //--------- DISPLAYING EQUIPMENT ---------
        if (toggle_eq == 1) {

            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    cout << equipment[i][j];
                }
                std::cout << "\n";
            }

        }

        //?        if (GetKeyState('A') & 0x8000/*Check if high-order bit is set (1 << 15)*/)

              //{
                    //printf("A");
                //}


                //--------- READING KEY INPUT ---------
                /*
                    Jeœli gracz wciœnie spacje uruchamia siê funkcja fight_enemy()
                        jeœli na mapie na przeciwko gracza (w kierunku w którym patrzy)
                        znajdujê sie znak "e"
                        funkcja iterujê przez listê przeciwników
                            gdy natrafi na przeciwnika którego po³o¿enie pasujê do
                            miejsca przed graczem otrzymujemy wskaŸnik do struktury
                            przeciwnika
                        maj¹c strukture przeciwnika u¿ywaj¹c referencji zadajemy mu
                        obra¿enia oraz sami je otrzymujemy
                */
        Sleep(300);
        //pressed_key = _getch();
       /* if (nowy_bohater->HP > 0) {
            switch (pressed_key) {
            case 'w':
                nowy_bohater->direction = direction::up;
                nowy_bohater->move(nowy_bohater, nowy_bohater);
                break;
            case 's':
                nowy_bohater->direction = direction::down;
                nowy_bohater->move(nowy_bohater, nowy_bohater);
                break;
            case 'd':
                nowy_bohater->direction = direction::right;
                nowy_bohater->move(nowy_bohater, nowy_bohater);
                break;
            case 'a':
                nowy_bohater->direction = direction::left;
                nowy_bohater->move(nowy_bohater, nowy_bohater);
                break;
            case ' ':
                check_for_enemy(nowy_bohater, enemy_list);
                break;
            case 'e':
                if (toggle_eq == 1) {
                    toggle_eq = 0;
                }
                else {
                    toggle_eq = 1;
                }
                break;
            }
        }
        else {
            game_loop = 0;
        }*/
        if (GetKeyState('A') < 0/*Check if high-order bit is set (1 << 15)*/)
        {
            nowy_bohater->direction = direction::left;
            nowy_bohater->move(nowy_bohater, nowy_bohater);
            printf("A");
        }
        if (GetKeyState('D') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
        {
            nowy_bohater->direction = direction::right;
            nowy_bohater->move(nowy_bohater, nowy_bohater);
            printf("d");
        }
        if (GetKeyState('W') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
        {
            nowy_bohater->direction = direction::up;
            nowy_bohater->move(nowy_bohater, nowy_bohater);
            printf("w");
        }
        if (GetKeyState('S') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
        {
            nowy_bohater->direction = direction::down;
            nowy_bohater->move(nowy_bohater, nowy_bohater);
            printf("s");
        }



  }

  

    // ----- DEALOKACJA PAMIECI -----
    for (int i = 0; i < w; i++) {
        free(mapa[i]);
    }
    free(mapa);

    for (int i = 0; i < w; i++) {
        free(origin_map[i]);
    }
    free(origin_map);
    for (int i = 0; i < enemy_amount; i++) {
        free(enemy_list[i]);
    }
    free(enemy_list);
    free(nowy_bohater);
    //free(nowy_przedmiot);

    return 1;
}