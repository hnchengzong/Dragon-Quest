#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LENGTH 60
#define MAX_INVENTORY 30
#define MAX_SKILLS 20
#define MAX_LOCATIONS 30
#define MAX_ENEMIES 30
#define MAX_NPCS 50
#define MAX_SHOP_ITEMS 30

typedef struct
{
    char name[MAX_NAME_LENGTH];
    long hp;
    long max_hp;
    long mp;
    long max_mp;
    long exp;
    long level;
    long gold;
    long attack;
    long defense;
    long agility;      // 敏捷，影响闪避和先攻
    long intelligence; // 智力，影响魔法攻击和魔法值
} Player;

typedef struct
{
    char name[MAX_NAME_LENGTH];
    int type;  // 0=武器, 1=防具, 2=消耗品
    int value; // 根据类型表示攻击力、防御力、恢复量的数值等
    int price;
} Item;

// 技能
typedef struct
{
    char name[MAX_NAME_LENGTH];
    int mp_cost;
    int damage;
    int heal;
    int required_level; // 学习所需等级
} Skill;

// 地点
typedef struct
{
    char name[MAX_NAME_LENGTH];
    char description[200];
    int type; // 0=城镇, 1=野外, 2=洞穴等
} Location;

// 敌人
typedef struct
{
    char name[MAX_NAME_LENGTH];
    int hp;
    int max_hp;
    int attack;
    int defense;
    int exp_reward;
    int gold_reward;
} Enemy;

// 任务
typedef struct
{
    int id;
    char name[MAX_NAME_LENGTH];
    char description[200];
    int completed;  // 是否完成 (0=未完成, 1=完成)
    int reward_exp; // 奖励
    int reward_gold;
    int reward_item;
} Quest;

// NPC
typedef struct
{
    char name[MAX_NAME_LENGTH];
    char dialog[200];
    char additional_dialogs[5][200]; // 添加额外对话
    int additional_dialogs_count;
    int item_to_sell; // -1表示不卖物品
    int item_price;
    int shop_items[MAX_SHOP_ITEMS];
    int shop_item_count;
} Npc;

// 游戏数据
typedef struct
{
    Player player;
    Item inventory[MAX_INVENTORY];
    Skill skills[MAX_SKILLS];
    Location locations[MAX_LOCATIONS];
    Enemy enemies[MAX_ENEMIES];
    Npc npcs[MAX_NPCS];
    Item items[MAX_INVENTORY];
    Quest quests[10];
    int dragon_defeated; // 恶龙是否被击败
    int current_location;
    int inventory_count;
    int learned_skills[MAX_SKILLS]; // 已学习技能
    int learned_skill_count;
} GameData;

void main_menu(GameData *game);


void init_game(GameData *game);
void init_player(Player *player);
void init_world(GameData *game);
void show_status(GameData *game);
void travel(GameData *game);
void battle(GameData *game);
void rest(GameData *game);
void talk_to_npc(GameData *game);
void show_inventory(GameData *game);
void use_item(GameData *game);
void level_up(GameData *game);
int calculate_damage(int attacker_attack, int defender_defense);
void save_game(GameData *game);
void load_game(GameData *game);
int file_exists(const char *filename);
void shop_menu(GameData *game, int npc_index);
void learn_skills(GameData *game);
int estimate_enemy_level(Enemy *enemy);
void cheat_game(GameData *game);

// 结局
void show_ending(GameData *game)
{
    printf("\n=====================================\n");
    printf("                结局\n");
    printf("=====================================\n");
    printf("经过一番激烈的战斗，你终于击败了恶龙！\n");
    printf("王国重新恢复了和平，人民不再生活在恐惧中。\n");
    printf("你的名字将被永远铭记在历史中，成为传说中的英雄！\n");
    printf("感谢您的游玩！\n");
    printf("=====================================\n");
}

int main()
{
    GameData game;
    srand(time(NULL));

    printf("=====================================\n");
    printf("      勇者斗恶龙\n");
    printf("=====================================\n\n");

    printf("是否有存档要加载？(y/n): ");
    char choice;
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y')
    {
        if (file_exists("savegame.dat"))
        {
            load_game(&game);
            printf("欢迎回来，%s！\n", game.player.name);
        }
        else
        {
            printf("未找到存档文件，开始新游戏。\n");
            init_game(&game);
            printf("欢迎来到勇者斗恶龙的世界，%s！\n", game.player.name);
            printf("和平与繁荣在这片土地上已持续了数百年，\n但这份宁静被一头突然出现的恶龙打破。\n恶龙所到之处，生灵涂炭，横尸遍野\n无数勇者前去讨伐它，却化作龙巢前的累累白骨。\n而你作为一名勇敢的战士，义无反顾地踏上了解救世界的旅程。");
        }
    }
    else
    {
        init_game(&game);
        printf("欢迎来到勇者斗恶龙的世界，%s！\n", game.player.name);
        printf("和平与繁荣在这片土地上已持续了数百年，\n但这份宁静被一头突然出现的恶龙打破。\n恶龙所到之处，生灵涂炭，横尸遍野\n无数勇者前去讨伐它，却化作龙巢前的累累白骨。\n而你作为一名勇敢的战士，义无反顾地踏上了解救世界的旅程。");
    }

    main_menu(&game);

    return 0;
}


void init_game(GameData *game)
{
    init_player(&game->player);
    init_world(game);
    game->current_location = 0;
    game->inventory_count = 0;
    game->learned_skill_count = 2; // 已学习技能
    game->learned_skills[0] = 0;
    game->learned_skills[1] = 1;

    strcpy(game->inventory[0].name, "铁剑");
    game->inventory[0].type = 0;
    game->inventory[0].value = 10;
    game->inventory_count++;

    strcpy(game->inventory[1].name, "皮甲");
    game->inventory[1].type = 1;
    game->inventory[1].value = 5;
    game->inventory_count++;

    strcpy(game->inventory[2].name, "生命药水");
    game->inventory[2].type = 2;
    game->inventory[2].value = 50;
    game->inventory_count++;
}

void init_player(Player *player)
{
    printf("请输入你的名字: ");
    scanf("%s", player->name);

    player->hp = 120;
    player->max_hp = 120;
    player->mp = 60;
    player->max_mp = 60;
    player->exp = 0;
    player->level = 1;
    player->gold = 20;
    player->attack = 15;
    player->defense = 5;
    player->intelligence = 8;
    player->agility = 5;
}

// 世界
void init_world(GameData *game)
{
    strcpy(game->locations[0].name, "瓦纳卡村");
    strcpy(game->locations[0].description, "一个宁静的小村庄，承载了你儿时的记忆，是你冒险旅程的起点。");
    game->locations[0].type = 0;

    strcpy(game->locations[1].name, "野外森林");
    strcpy(game->locations[1].description, "野外的森林，经常有怪物出没。");
    game->locations[1].type = 1;

    strcpy(game->locations[2].name, "洞穴");
    strcpy(game->locations[2].description, "阴森的洞穴。");
    game->locations[2].type = 2;

    strcpy(game->locations[3].name, "龙巢");
    strcpy(game->locations[3].description, "恶龙的巢穴，最终决战的地方。");
    game->locations[3].type = 3;

    strcpy(game->locations[4].name, "王城");
    strcpy(game->locations[4].description, "王国的首都。");
    game->locations[4].type = 4;

    strcpy(game->locations[5].name, "沙漠绿洲");
    strcpy(game->locations[5].description, "沙漠中的绿洲，可以休息。");
    game->locations[5].type = 5;

    strcpy(game->locations[6].name, "雪山");
    strcpy(game->locations[6].description, "寒冷的雪山，据说藏着宝藏，但有雪怪出没。");
    game->locations[6].type = 6;

    strcpy(game->locations[7].name, "地下城");
    strcpy(game->locations[7].description, "古老的地下城，充满了危险与机遇。");
    game->locations[7].type = 7;

    strcpy(game->locations[8].name, "精灵之森");
    strcpy(game->locations[8].description, "精灵居住的森林。也潜伏着黑暗");
    game->locations[8].type = 8;

    strcpy(game->locations[9].name, "海盗港湾");
    strcpy(game->locations[9].description, "海盗聚集的港湾。");
    game->locations[9].type = 9;

    strcpy(game->locations[10].name, "火山口");
    strcpy(game->locations[10].description, "炽热的火山地带。");
    game->locations[10].type = 10;

    strcpy(game->locations[11].name, "古代遗迹");
    strcpy(game->locations[11].description, "失落文明的遗迹，（难度较高）");
    game->locations[11].type = 11;

    strcpy(game->locations[12].name, "黑暗沼泽");
    strcpy(game->locations[12].description, "阴暗潮湿的沼泽地。");
    game->locations[12].type = 12;

    strcpy(game->locations[13].name, "魔法学院");
    strcpy(game->locations[13].description, "学习高级魔法的学府。");
    game->locations[13].type = 13;

    strcpy(game->locations[14].name, "幽灵之地");
    strcpy(game->locations[14].description, "充满了幽灵的森林。");
    game->locations[14].type = 14;

    strcpy(game->locations[15].name, "决斗场");
    strcpy(game->locations[15].description, "与英雄决斗的场地。（难度较高）");
    game->locations[15].type = 15;

    // 物品
    strcpy(game->items[0].name, "铁剑");
    game->items[0].type = 0;
    game->items[0].value = 10;
    game->items[0].price = 50;

    strcpy(game->items[1].name, "皮甲");
    game->items[1].type = 1;
    game->items[1].value = 5;
    game->items[1].price = 30;

    strcpy(game->items[2].name, "生命药水");
    game->items[2].type = 2;
    game->items[2].value = 50;
    game->items[2].price = 20;

    strcpy(game->items[3].name, "钢剑");
    game->items[3].type = 0;
    game->items[3].value = 320;
    game->items[3].price = 3200;

    strcpy(game->items[4].name, "锁子甲");
    game->items[4].type = 1;
    game->items[4].value = 100;
    game->items[4].price = 1000;

    strcpy(game->items[5].name, "高级生命药水");
    game->items[5].type = 2;
    game->items[5].value = 2000;
    game->items[5].price = 400;

    strcpy(game->items[6].name, "魔法药水");
    game->items[6].type = 2;
    game->items[6].value = 400;
    game->items[6].price = 800;

    strcpy(game->items[7].name, "双手剑");
    game->items[7].type = 0;
    game->items[7].value = 78;
    game->items[7].price = 800;

    strcpy(game->items[8].name, "板甲");
    game->items[8].type = 1;
    game->items[8].value = 40;
    game->items[8].price = 500;

    strcpy(game->items[9].name, "超级生命药水");
    game->items[9].type = 2;
    game->items[9].value = 8000;
    game->items[9].price = 1000;

    strcpy(game->items[10].name, "传说之剑");
    game->items[10].type = 0;
    game->items[10].value = 1000;
    game->items[10].price = 8000;

    strcpy(game->items[11].name, "龙鳞甲");
    game->items[11].type = 1;
    game->items[11].value = 1000;
    game->items[11].price = 8000;

    strcpy(game->items[12].name, "短剑");
    game->items[12].type = 0;
    game->items[12].value = 13;
    game->items[12].price = 100;

    strcpy(game->items[13].name, "长矛");
    game->items[13].type = 0;
    game->items[13].value = 40;
    game->items[13].price = 400;

    strcpy(game->items[14].name, "战斧");
    game->items[14].type = 0;
    game->items[14].value = 80;
    game->items[14].price = 800;

    strcpy(game->items[15].name, "精灵弓");
    game->items[15].type = 0;
    game->items[15].value = 100;
    game->items[15].price = 1000;

    strcpy(game->items[16].name, "法杖");
    game->items[16].type = 0;
    game->items[16].value = 120;
    game->items[16].price = 1200;

    strcpy(game->items[17].name, "布衣");
    game->items[17].type = 1;
    game->items[17].value = 3;
    game->items[17].price = 60;

    strcpy(game->items[18].name, "布甲");
    game->items[18].type = 1;
    game->items[18].value = 12;
    game->items[18].price = 120;

    strcpy(game->items[19].name, "链甲");
    game->items[19].type = 1;
    game->items[19].value = 40;
    game->items[19].price = 500;

    strcpy(game->items[20].name, "骑士铠甲");
    game->items[20].type = 1;
    game->items[20].value = 60;
    game->items[20].price = 800;

    strcpy(game->items[21].name, "法师之袍");
    game->items[21].type = 1;
    game->items[21].value = 85;
    game->items[21].price = 1000;

    strcpy(game->items[23].name, "中级生命药水");
    game->items[23].type = 2;
    game->items[23].value = 800;
    game->items[23].price = 200;

    strcpy(game->items[24].name, "高级魔法药水");
    game->items[24].type = 2;
    game->items[24].value = 1000;
    game->items[24].price = 1800;

    strcpy(game->items[26].name, "力量药剂");
    game->items[26].type = 2;
    game->items[26].value = 0;
    game->items[26].price = 800;

    strcpy(game->items[27].name, "敏捷药剂");
    game->items[27].type = 2;
    game->items[27].value = 0;
    game->items[27].price = 800;

    strcpy(game->items[28].name, "智力药剂");
    game->items[28].type = 2;
    game->items[28].value = 0;
    game->items[28].price = 800;

    strcpy(game->items[29].name, "狼皮");
    game->items[29].type = 2; // 任务物品
    game->items[29].value = 0;
    game->items[29].price = 10;

    strcpy(game->skills[0].name, "重击");
    game->skills[0].mp_cost = 10;
    game->skills[0].damage = 27;
    game->skills[0].heal = 0;
    game->skills[0].required_level = 1;

    strcpy(game->skills[1].name, "治疗");
    game->skills[1].mp_cost = 15;
    game->skills[1].damage = 0;
    game->skills[1].heal = 33;
    game->skills[1].required_level = 1;

    strcpy(game->skills[2].name, "火焰术");
    game->skills[2].mp_cost = 20;
    game->skills[2].damage = 38;
    game->skills[2].heal = 0;
    game->skills[2].required_level = 3;

    strcpy(game->skills[3].name, "冰霜术");
    game->skills[3].mp_cost = 25;
    game->skills[3].damage = 47;
    game->skills[3].heal = 0;
    game->skills[3].required_level = 5;

    strcpy(game->skills[4].name, "惊雷");
    game->skills[4].mp_cost = 55;
    game->skills[4].damage = 93;
    game->skills[4].heal = 0;
    game->skills[4].required_level = 8;

    strcpy(game->skills[5].name, "高效治疗");
    game->skills[5].mp_cost = 60;
    game->skills[5].damage = 0;
    game->skills[5].heal = 300;
    game->skills[5].required_level = 10;

    strcpy(game->skills[6].name, "旋风斩");
    game->skills[6].mp_cost = 60;
    game->skills[6].damage = 155;
    game->skills[6].heal = 0;
    game->skills[6].required_level = 15;

    strcpy(game->skills[7].name, "沐浴");
    game->skills[7].mp_cost = 80;
    game->skills[7].damage = 0;
    game->skills[7].heal = 800;
    game->skills[7].required_level = 25;

    strcpy(game->skills[8].name, "审判");
    game->skills[8].mp_cost = 800;
    game->skills[8].damage = 1800;
    game->skills[8].heal = 0;
    game->skills[8].required_level = 50;

    strcpy(game->skills[9].name, "神之祝福");
    game->skills[9].mp_cost = 200;
    game->skills[9].damage = 0;
    game->skills[9].heal = 1600;
    game->skills[9].required_level = 60;

    strcpy(game->skills[10].name, "突袭");
    game->skills[10].mp_cost = 120;
    game->skills[10].damage = 480;
    game->skills[10].heal = 0;
    game->skills[10].required_level = 60;

    strcpy(game->skills[11].name, "生命汲取");
    game->skills[11].mp_cost = 80;
    game->skills[11].damage = 300;
    game->skills[11].heal = 300;
    game->skills[11].required_level = 30;

    strcpy(game->skills[12].name, "冻结之魔弹");
    game->skills[12].mp_cost = 140;
    game->skills[12].damage = 800;
    game->skills[12].heal = 0;
    game->skills[12].required_level = 65;

    strcpy(game->skills[13].name, "雷霆之威严");
    game->skills[13].mp_cost = 300;
    game->skills[13].damage = 2000;
    game->skills[13].heal = 0;
    game->skills[13].required_level = 70;

    strcpy(game->skills[14].name, "旋风斩");
    game->skills[14].mp_cost = 300;
    game->skills[14].damage = 600;
    game->skills[14].heal = 0;
    game->skills[14].required_level = 40;

    strcpy(game->skills[15].name, "回溯");
    game->skills[15].mp_cost = 1200;
    game->skills[15].damage = 0;
    game->skills[15].heal = 18000;
    game->skills[15].required_level = 75;

    strcpy(game->skills[16].name, "风暴");
    game->skills[16].mp_cost = 700;
    game->skills[16].damage = 3300;
    game->skills[16].heal = 0;
    game->skills[16].required_level = 80;

    strcpy(game->skills[17].name, "万剑归一");
    game->skills[17].mp_cost = 800;
    game->skills[17].damage = 3900;
    game->skills[17].heal = 0;
    game->skills[17].required_level = 90;

    strcpy(game->skills[18].name, "造物惩击");
    game->skills[18].mp_cost = 1600;
    game->skills[18].damage = 15000;
    game->skills[18].heal = 0;
    game->skills[18].required_level = 100;

    // 敌人
    strcpy(game->enemies[0].name, "哥布林");
    game->enemies[0].hp = 30;
    game->enemies[0].max_hp = 30;
    game->enemies[0].attack = 10;
    game->enemies[0].defense = 2;
    game->enemies[0].exp_reward = 20;
    game->enemies[0].gold_reward = 10;

    strcpy(game->enemies[1].name, "狼");
    game->enemies[1].hp = 40;
    game->enemies[1].max_hp = 40;
    game->enemies[1].attack = 15;
    game->enemies[1].defense = 5;
    game->enemies[1].exp_reward = 30;
    game->enemies[1].gold_reward = 15;

    strcpy(game->enemies[2].name, "骷髅战士");
    game->enemies[2].hp = 80;
    game->enemies[2].max_hp = 80;
    game->enemies[2].attack = 25;
    game->enemies[2].defense = 8;
    game->enemies[2].exp_reward = 50;
    game->enemies[2].gold_reward = 40;

    strcpy(game->enemies[3].name, "恶龙");
    game->enemies[3].hp = 32000;
    game->enemies[3].max_hp = 32000;
    game->enemies[3].attack = 800;
    game->enemies[3].defense = 160;
    game->enemies[3].exp_reward = 5000;
    game->enemies[3].gold_reward = 5000;

    strcpy(game->enemies[4].name, "沙漠蝎子");
    game->enemies[4].hp = 140;
    game->enemies[4].max_hp = 140;
    game->enemies[4].attack = 40;
    game->enemies[4].defense = 25;
    game->enemies[4].exp_reward = 70;
    game->enemies[4].gold_reward = 40;

    strcpy(game->enemies[5].name, "雪怪");
    game->enemies[5].hp = 800;
    game->enemies[5].max_hp = 800;
    game->enemies[5].attack = 60;
    game->enemies[5].defense = 48;
    game->enemies[5].exp_reward = 90;
    game->enemies[5].gold_reward = 40;

    strcpy(game->enemies[6].name, "海盗");
    game->enemies[6].hp = 680;
    game->enemies[6].max_hp = 680;
    game->enemies[6].attack = 112;
    game->enemies[6].defense = 29;
    game->enemies[6].exp_reward = 120;
    game->enemies[6].gold_reward = 80;

    strcpy(game->enemies[7].name, "精灵法师");
    game->enemies[7].hp = 460;
    game->enemies[7].max_hp = 460;
    game->enemies[7].attack = 90;
    game->enemies[7].defense = 15;
    game->enemies[7].exp_reward = 100;
    game->enemies[7].gold_reward = 90;

    strcpy(game->enemies[8].name, "石像鬼");
    game->enemies[8].hp = 990;
    game->enemies[8].max_hp = 990;
    game->enemies[8].attack = 32;
    game->enemies[8].defense = 95;
    game->enemies[8].exp_reward = 120;
    game->enemies[8].gold_reward = 100;

    strcpy(game->enemies[9].name, "恶魔");
    game->enemies[9].hp = 650;
    game->enemies[9].max_hp = 650;
    game->enemies[9].attack = 175;
    game->enemies[9].defense = 60;
    game->enemies[9].exp_reward = 666;
    game->enemies[9].gold_reward = 666;

    strcpy(game->enemies[10].name, "火焰巨人");
    game->enemies[10].hp = 400;
    game->enemies[10].max_hp = 400;
    game->enemies[10].attack = 120;
    game->enemies[10].defense = 30;
    game->enemies[10].exp_reward = 460;
    game->enemies[10].gold_reward = 180;

    strcpy(game->enemies[11].name, "毒蛇");
    game->enemies[11].hp = 20;
    game->enemies[11].max_hp = 20;
    game->enemies[11].attack = 30;
    game->enemies[11].defense = 7;
    game->enemies[11].exp_reward = 30;
    game->enemies[11].gold_reward = 20;

    strcpy(game->enemies[12].name, "幽灵");
    game->enemies[12].hp = 200;
    game->enemies[12].max_hp = 200;
    game->enemies[12].attack = 50;
    game->enemies[12].defense = 30;
    game->enemies[12].exp_reward = 180;
    game->enemies[12].gold_reward = 160;

    strcpy(game->enemies[13].name, "石头人");
    game->enemies[13].hp = 1000;
    game->enemies[13].max_hp = 1000;
    game->enemies[13].attack = 20;
    game->enemies[13].defense = 150;
    game->enemies[13].exp_reward = 460;
    game->enemies[13].gold_reward = 150;

    strcpy(game->enemies[14].name, "黑暗法师");
    game->enemies[14].hp = 250;
    game->enemies[14].max_hp = 250;
    game->enemies[14].attack = 480;
    game->enemies[14].defense = 25;
    game->enemies[14].exp_reward = 450;
    game->enemies[14].gold_reward = 230;

    strcpy(game->enemies[15].name, "地狱犬");
    game->enemies[15].hp = 400;
    game->enemies[15].max_hp = 400;
    game->enemies[15].attack = 85;
    game->enemies[15].defense = 48;
    game->enemies[15].exp_reward = 480;
    game->enemies[15].gold_reward = 160;

    strcpy(game->enemies[16].name, "木乃伊");
    game->enemies[16].hp = 100;
    game->enemies[16].max_hp = 100;
    game->enemies[16].attack = 35;
    game->enemies[16].defense = 35;
    game->enemies[16].exp_reward = 120;
    game->enemies[16].gold_reward = 40;

    strcpy(game->enemies[17].name, "冰霜巨龙");
    game->enemies[17].hp = 1200;
    game->enemies[17].max_hp = 1200;
    game->enemies[17].attack = 120;
    game->enemies[17].defense = 70;
    game->enemies[17].exp_reward = 1000;
    game->enemies[17].gold_reward = 750;

    strcpy(game->enemies[18].name, "刺客");
    game->enemies[18].hp = 440;
    game->enemies[18].max_hp = 440;
    game->enemies[18].attack = 200;
    game->enemies[18].defense = 20;
    game->enemies[18].exp_reward = 500;
    game->enemies[18].gold_reward = 780;

    strcpy(game->enemies[19].name, "熔岩元素");
    game->enemies[19].hp = 350;
    game->enemies[19].max_hp = 350;
    game->enemies[19].attack = 75;
    game->enemies[19].defense = 50;
    game->enemies[19].exp_reward = 320;
    game->enemies[19].gold_reward = 300;

    strcpy(game->enemies[20].name, "远古巨魔");
    game->enemies[20].hp = 1000;
    game->enemies[20].max_hp = 1000;
    game->enemies[20].attack = 150;
    game->enemies[20].defense = 80;
    game->enemies[20].exp_reward = 1000;
    game->enemies[20].gold_reward = 900;

    strcpy(game->enemies[21].name, "堕天使");
    game->enemies[21].hp = 1500;
    game->enemies[21].max_hp = 1500;
    game->enemies[21].attack = 200;
    game->enemies[21].defense = 100;
    game->enemies[21].exp_reward = 1500;
    game->enemies[21].gold_reward = 1400;

    strcpy(game->enemies[22].name, "混沌体");
    game->enemies[22].hp = 3800;
    game->enemies[22].max_hp = 3800;
    game->enemies[22].attack = 300;
    game->enemies[22].defense = 300;
    game->enemies[22].exp_reward = 4800;
    game->enemies[22].gold_reward = 0;

    strcpy(game->enemies[23].name, "虚空行者");
    game->enemies[23].hp = 1200;
    game->enemies[23].max_hp = 1200;
    game->enemies[23].attack = 150;
    game->enemies[23].defense = 300;
    game->enemies[23].exp_reward = 1200;
    game->enemies[23].gold_reward = 600;

    strcpy(game->enemies[24].name, "奥赛罗");
    game->enemies[24].hp = 2400;
    game->enemies[24].max_hp = 2400;
    game->enemies[24].attack = 360;
    game->enemies[24].defense = 100;
    game->enemies[24].exp_reward = 2000;
    game->enemies[24].gold_reward = 900;

    // NPC
    strcpy(game->npcs[0].name, "武器商人");
    strcpy(game->npcs[0].dialog, "欢迎光临！看看我的武器吧。");
    game->npcs[0].item_to_sell = -1;
    game->npcs[0].item_price = 0;
    game->npcs[0].shop_items[0] = 0;  // 铁剑
    game->npcs[0].shop_items[1] = 3;  // 钢剑
    game->npcs[0].shop_items[2] = 7;  // 双手剑
    game->npcs[0].shop_items[3] = 12; // 短剑
    game->npcs[0].shop_items[4] = 13; // 院长矛
    game->npcs[0].shop_items[5] = 14; // 战斧
    game->npcs[0].shop_items[6] = 15; // 精灵弓
    game->npcs[0].shop_items[7] = 16; // 法杖
    game->npcs[0].shop_item_count = 8;

    strcpy(game->npcs[1].name, "村长");
    strcpy(game->npcs[1].dialog, "勇士，感谢你为我们挺身而出。你一定能击败恶龙！");
    game->npcs[1].item_to_sell = -1;
    game->npcs[1].item_price = 0;
    game->npcs[1].shop_item_count = 0;

    strcpy(game->npcs[2].name, "防具商人");
    strcpy(game->npcs[2].dialog, "高质量的防具能让你在战斗中生存更久。");
    game->npcs[2].item_to_sell = -1;
    game->npcs[2].item_price = 0;
    game->npcs[2].shop_items[0] = 1;  // 皮甲
    game->npcs[2].shop_items[1] = 4;  // 锁子甲
    game->npcs[2].shop_items[2] = 8;  // 板甲
    game->npcs[2].shop_items[3] = 11; // 龙鳞甲
    game->npcs[2].shop_items[4] = 17; // 布衣
    game->npcs[2].shop_items[5] = 18; // 鳞甲
    game->npcs[2].shop_items[6] = 19; // 链甲
    game->npcs[2].shop_items[7] = 20; // 骑士铠甲
    game->npcs[2].shop_items[8] = 21; // 法师袍
    game->npcs[2].shop_item_count = 9;

    strcpy(game->npcs[3].name, "药剂师");
    strcpy(game->npcs[3].dialog, "生命药水和魔法药水，冒险必备！");
    game->npcs[3].item_to_sell = -1;
    game->npcs[3].item_price = 0;
    game->npcs[3].shop_items[0] = 2;  // 生命药水
    game->npcs[3].shop_items[1] = 5;  // 高级生命药水
    game->npcs[3].shop_items[2] = 9;  // 超级生命药水
    game->npcs[3].shop_items[3] = 6;  // 魔法药水
    game->npcs[3].shop_items[4] = 23; // 中级生命药水
    game->npcs[3].shop_items[5] = 24; // 高级魔法药水
    game->npcs[3].shop_items[6] = 26; // 力量药剂
    game->npcs[3].shop_items[7] = 27; // 敏捷药剂
    game->npcs[3].shop_items[8] = 28; // 智力药剂
    game->npcs[3].shop_item_count = 9;

    strcpy(game->npcs[4].name, "技能导师");
    strcpy(game->npcs[4].dialog, "我可以教你更强大的技能，但需要足够的等级。");
    game->npcs[4].item_to_sell = -1;
    game->npcs[4].item_price = 0;
    game->npcs[4].shop_item_count = 0;

    strcpy(game->npcs[5].name, "国王");
    strcpy(game->npcs[5].dialog, "无畏的勇者，希望你能成功讨伐恶龙！");
    game->npcs[5].item_to_sell = -1;
    game->npcs[5].item_price = 0;
    game->npcs[5].shop_item_count = 0;

    strcpy(game->npcs[6].name, "船长");
    strcpy(game->npcs[6].dialog, "想要出海探险吗？这片海域非常危险。");
    game->npcs[6].item_to_sell = -1;
    game->npcs[6].item_price = 0;
    game->npcs[6].shop_item_count = 0;

    strcpy(game->npcs[7].name, "精灵长老");
    strcpy(game->npcs[7].dialog, "古老的魔法正在消失，我们需要你的帮助。");
    strcpy(game->npcs[7].additional_dialogs[0], "很久以前，这片土地上充满了魔法的力量。");
    strcpy(game->npcs[7].additional_dialogs[1], "但随着时光流逝，魔法逐渐衰弱，我们需要你的力量来恢复它。");
    game->npcs[7].additional_dialogs_count = 2;
    game->npcs[7].item_to_sell = -1;
    game->npcs[7].item_price = 0;
    game->npcs[7].shop_item_count = 0;

    strcpy(game->npcs[8].name, "铁匠");
    strcpy(game->npcs[8].dialog, "我可以用最好的材料为你打造武器和防具。");
    strcpy(game->npcs[8].additional_dialogs[0], "我曾经为国王打造过武器，如果你有足够的金币，我可以为你打造任何武器。");
    strcpy(game->npcs[8].additional_dialogs[1], "最近，我找到了一些稀有的矿石，可以制作出非常强大的装备。");
    game->npcs[8].additional_dialogs_count = 2;
    game->npcs[8].item_to_sell = -1;
    game->npcs[8].item_price = 0;
    game->npcs[8].shop_items[0] = 7;  // 双手剑
    game->npcs[8].shop_items[1] = 8;  // 板甲
    game->npcs[8].shop_items[2] = 10; // 传说之剑
    game->npcs[8].shop_items[3] = 11; // 龙鳞甲
    game->npcs[8].shop_items[4] = 14; // 战斧
    game->npcs[8].shop_items[5] = 16; // 法杖
    game->npcs[8].shop_items[6] = 20; // 骑士铠甲
    game->npcs[8].shop_item_count = 7;

    strcpy(game->npcs[9].name, "神秘商人");
    strcpy(game->npcs[9].dialog, "我这里有一些奇特的商品，但价格不菲。");
    strcpy(game->npcs[9].additional_dialogs[0], "这些商品是从世界各地收集来的，每一件都有独特的用途。");
    strcpy(game->npcs[9].additional_dialogs[1], "如果你有足够的金币，我可以卖给你真正强大的物品。");
    game->npcs[9].additional_dialogs_count = 2;
    game->npcs[9].item_to_sell = -1;
    game->npcs[9].item_price = 0;
    game->npcs[9].shop_items[0] = 9;  // 超级生命药水
    game->npcs[9].shop_items[1] = 10; // 传说之剑
    game->npcs[9].shop_items[2] = 11; // 龙鳞甲
    game->npcs[9].shop_items[4] = 26; // 力量药剂
    game->npcs[9].shop_items[5] = 27; // 敏捷药剂
    game->npcs[9].shop_items[6] = 28; // 智力药剂
    game->npcs[9].shop_item_count = 7;

    strcpy(game->npcs[10].name, "老渔夫");
    strcpy(game->npcs[10].dialog, "这片海域隐藏着许多秘密。");
    strcpy(game->npcs[10].additional_dialogs[0], "我在这片海上打渔几十年了，见过许多奇怪的事情。");
    strcpy(game->npcs[10].additional_dialogs[1], "据说在深海中有一座沉没的城市，但到现在都没人能找到它。");
    game->npcs[10].additional_dialogs_count = 2;
    game->npcs[10].item_to_sell = -1;
    game->npcs[10].item_price = 0;
    game->npcs[10].shop_item_count = 0;

    strcpy(game->npcs[11].name, "图书管理员");
    strcpy(game->npcs[11].dialog, "书籍是知识的源泉。");
    strcpy(game->npcs[11].additional_dialogs[0], "在这些古老的书籍中，记录着许多失传的法术和秘密。");
    strcpy(game->npcs[11].additional_dialogs[1], "如果你愿意花时间学习，我可以教你一些有用的技能。");
    game->npcs[11].additional_dialogs_count = 2;
    game->npcs[11].item_to_sell = -1;
    game->npcs[11].item_price = 0;
    game->npcs[11].shop_item_count = 0;

    strcpy(game->npcs[12].name, "赏金猎人");
    strcpy(game->npcs[12].dialog, "我正在追踪一个危险的罪犯。");
    strcpy(game->npcs[12].additional_dialogs[0], "就不必劳烦你了，我自己会找到他的。");
    strcpy(game->npcs[12].additional_dialogs[1], "他最后一次出现在黑暗沼泽附近，小心点。");
    game->npcs[12].additional_dialogs_count = 2;
    game->npcs[12].item_to_sell = -1;
    game->npcs[12].item_price = 0;
    game->npcs[12].shop_item_count = 0;

    strcpy(game->npcs[13].name, "炼金术士");
    strcpy(game->npcs[13].dialog, "我可以将材料转化为珍贵的药水和物品。");
    strcpy(game->npcs[13].additional_dialogs[0], "炼金术是一门深奥的学问，需要精确的配方和技巧。");
    strcpy(game->npcs[13].additional_dialogs[1], "如果你能用等价的金钱交易，我可以为你制作强大的药水。");
    game->npcs[13].additional_dialogs_count = 2;
    game->npcs[13].item_to_sell = -1;
    game->npcs[13].item_price = 0;
    game->npcs[13].shop_items[0] = 5;  // 高级生命药水
    game->npcs[13].shop_items[1] = 6;  // 魔法药水
    game->npcs[13].shop_items[2] = 9;  // 超级生命药水
    game->npcs[13].shop_items[3] = 23; // 中级生命药水
    game->npcs[13].shop_items[4] = 24; // 高级魔法药水
    game->npcs[13].shop_items[5] = 26; // 力量药剂
    game->npcs[13].shop_items[6] = 27; // 敏捷药剂
    game->npcs[13].shop_items[7] = 28; // 智力药剂
    game->npcs[13].shop_item_count = 8;

    strcpy(game->npcs[14].name, "占卜师");
    strcpy(game->npcs[14].dialog, "我能预见未来，虽然命运往往难以改变。");
    strcpy(game->npcs[14].additional_dialogs[0], "我看到了恶龙的爪牙正在集结，世界只有你才能拯救。");
    strcpy(game->npcs[14].additional_dialogs[1], "小心前方的道路，危险正等着你。");
    game->npcs[14].additional_dialogs_count = 2;
    game->npcs[14].item_to_sell = -1;
    game->npcs[14].item_price = 0;
    game->npcs[14].shop_item_count = 0;

    strcpy(game->npcs[16].name, "村民");
    strcpy(game->npcs[16].dialog, "最近我听说在迷雾森林里出现了很多狼。");
    strcpy(game->npcs[16].additional_dialogs[1], "如果你需要补给，村里的商人们会提供帮助。");
    game->npcs[16].additional_dialogs_count = 2;
    game->npcs[16].item_to_sell = -1;
    game->npcs[16].item_price = 0;
    game->npcs[16].shop_item_count = 0;

    strcpy(game->npcs[17].name, "老者");
    strcpy(game->npcs[17].dialog, "年轻人，这个世界比你想象的更加复杂。");
    strcpy(game->npcs[17].additional_dialogs[0], "我年轻时也曾像你一样勇敢，但岁月不饶人。");
    game->npcs[17].additional_dialogs_count = 1;
    game->npcs[17].item_to_sell = -1;
    game->npcs[17].item_price = 0;
    game->npcs[17].shop_item_count = 0;

    strcpy(game->npcs[19].name, "神秘女子");
    strcpy(game->npcs[19].dialog, "我能感受到你身上的特殊气息...");
    strcpy(game->npcs[19].additional_dialogs[0], "命运正引导着你，年轻的勇者。");
    strcpy(game->npcs[19].additional_dialogs[1], "小心隐藏在阴影中的敌人。");
    game->npcs[19].additional_dialogs_count = 2;
    game->npcs[19].item_to_sell = -1;
    game->npcs[19].item_price = 0;
    game->npcs[19].shop_item_count = 0;

    game->dragon_defeated = 0; // 恶龙未被击败
}

// 估算敌人等级
int estimate_enemy_level(Enemy *enemy)
{
    // 基于敌人的属性估算
    int level_by_hp = enemy->hp / 30;
    int level_by_attack = enemy->attack / 5;

    int estimated_level = (level_by_hp + level_by_attack) / 2;

    if (estimated_level < 1)
        estimated_level = 1;

    return estimated_level;
}

void main_menu(GameData *game)
{
    int choice;

    while (1)
    {
        printf("\n========== 主菜单 ==========\n");
        printf("当前地点：%s\n", game->locations[game->current_location].name);
        printf("1. 查看状态\n");
        printf("2. 移动\n");
        printf("3. 寻找敌人\n");
        printf("4. 与NPC交谈\n");
        printf("5. 查看背包\n");
        printf("6. 使用物品\n");
        printf("7. 休息\n");
        printf("8. 学习技能\n");
        printf("9. 保存游戏\n");
        printf("0. 退出游戏\n");
        printf("请选择: ");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            show_status(game);
            break;
        case 2:
            travel(game);
            break;
        case 3:
            battle(game);
            break;
        case 4:
            talk_to_npc(game);
            break;
        case 5:
            show_inventory(game);
            break;
        case 6:
            use_item(game);
            break;
        case 7:
            rest(game);
            break;
        case 8:
            learn_skills(game);
            break;
        case 9:
            save_game(game);
            break;
        case 0:
            printf("感谢游玩！再见！\n");
            exit(0);
        case 666:
            cheat_game(game);
            break;
        case 114514:
            printf("哼哼哼啊啊啊啊啊啊啊啊啊啊！！！！！！\n");
        default:
            printf("无效选择，请重新输入。\n");
        }
    }
}

// 状态
void show_status(GameData *game)
{
    printf("\n========== 角色状态 ==========\n");
    printf("姓名: %s\n", game->player.name);
    printf("等级: %d\n", game->player.level);
    printf("经验值: %d/%d\n", game->player.exp, game->player.level * 100);
    printf("生命值: %d/%d\n", game->player.hp, game->player.max_hp);
    printf("魔法值: %d/%d\n", game->player.mp, game->player.max_mp);
    printf("攻击力: %d\n", game->player.attack);
    printf("防御力: %d\n", game->player.defense);
    printf("敏捷: %d\n", game->player.agility);
    printf("智力: %d\n", game->player.intelligence);
    printf("金币: %d\n", game->player.gold);
    printf("=============================\n");
}

void travel(GameData *game)
{
    int i, choice;

    printf("\n========== 可去地点 ==========\n");
    for (i = 0; i < 14; i++)
    {
        if (i != game->current_location)
        {
            printf("%d. %s - %s\n", i + 1, game->locations[i].name, game->locations[i].description);
        }
    }
    printf("请选择目的地 (输入对应数字): ");

    scanf("%d", &choice);
    choice--;

    if (choice >= 0 && choice < 14 && choice != game->current_location)
    {
        game->current_location = choice;
        printf("你来到了%s。\n", game->locations[game->current_location].name);
    }
    else if (choice == 666)
    {
        cheat_game(game);
    }
    else
    {
        printf("无效的选择。\n");
    }
}

// 战斗
void battle(GameData *game)
{
    // 如果恶龙被击败
    if (game->dragon_defeated && game->current_location == 3)
    {
        printf("恶龙已经被你击败了，龙之城堡现在是一片废墟。\n");
        return;
    }

    int enemy_type;
    switch (game->current_location)
    {
    case 0: // 村庄
        printf("在村庄里很安全，没有敌人。\n");
        return;
    case 1: // 森林 - 哥布林、狼或毒蛇
        enemy_type = rand() % 3;
        if (enemy_type == 2)
            enemy_type = 11; // 毒蛇
        break;
    case 2: // 洞穴 - 骷髅战士或木乃伊
        enemy_type = (rand() % 2) ? 2 : 16;
        break;
    case 3: // 城堡 - 恶龙
        enemy_type = 3;
        break;
    case 4: // 王城 - 安全区域
        printf("在王城里很安全，没有敌人。\n");
        return;
    case 5: // 沙漠绿洲 - 沙漠蝎子
        enemy_type = 4;
        break;
    case 6: // 雪山之巅 - 雪怪或冰霜巨龙
        enemy_type = (rand() % 2) ? 5 : 17;
        break;
    case 7: // 地下城 - 石像鬼或恶魔
        enemy_type = 8 + (rand() % 2);
        break;
    case 8: // 精灵之森 - 精灵法师
        enemy_type = (rand() % 2) ? 7 : 16;
        break;
    case 9: // 海盗港湾 - 海盗
        enemy_type = 6;
        break;
    case 10: // 火山口 - 火焰巨人或熔岩元素
        enemy_type = (rand() % 2) ? 10 : 19;
        break;
    case 11: // 古代遗迹 - 堕天使或混沌体或虚空行者
        enemy_type = (rand() % 3) + 21;
        break;
    case 12: // 黑暗沼泽 - 幽灵或石头人或黑暗法师
        enemy_type = (rand() % 2) ? 11 : 12;
        break;
    case 13: // 魔法学院
        printf("在魔法学院里很安全，没有敌人。\n");
        return;
    case 14: // 幽灵船 - 幽灵或刺客
        enemy_type = (rand() % 2) ? 12 : 18;
    case 15: // 决斗场 - 奥赛罗
        enemy_type = 24;
        break;
    default:
        enemy_type = rand() % 10;
    }

    Enemy enemy = game->enemies[enemy_type];
    printf("\n遭遇了%s！\n", enemy.name);

    while (game->player.hp > 0 && enemy.hp > 0)
    {
        int choice, damage;

        printf("\n---------- 战斗信息 ----------\n");
        printf("%s 生命值: %d/%d\n", enemy.name, enemy.hp, enemy.max_hp);
        printf("%s 生命值: %d/%d\n", game->player.name, game->player.hp, game->player.max_hp);
        printf("魔法值: %d/%d\n", game->player.mp, game->player.max_mp);
        printf("-----------------------------\n");

        printf("1. 普通攻击\n");
        printf("2. 使用技能\n");
        printf("3. 逃跑\n");
        printf("请选择行动: ");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1: // 普通攻击
            damage = calculate_damage(game->player.attack, enemy.defense);
            enemy.hp -= damage;
            printf("你对%s造成了%d点伤害！\n", enemy.name, damage);
            // 击败恶龙
            if (enemy_type == 3 && enemy.hp <= 0 && !game->dragon_defeated)
            {
                printf("你击败了%s！\n", enemy.name);
                game->player.exp += enemy.exp_reward;
                game->player.gold += enemy.gold_reward;
                printf("获得了%d经验值和%d金币！\n", enemy.exp_reward, enemy.gold_reward);

                game->dragon_defeated = 1;

                show_ending(game);

                if (game->player.exp >= game->player.level * 100)
                {
                    level_up(game);
                }

                return;
            }

            if (enemy.hp <= 0)
            {
                printf("你击败了%s！\n", enemy.name);
                game->player.exp += enemy.exp_reward;
                game->player.gold += enemy.gold_reward;
                printf("获得了%d经验值和%d金币！\n", enemy.exp_reward, enemy.gold_reward);

                if (game->player.exp >= game->player.level * 100)
                {
                    level_up(game);
                }
                return;
            }
            break;

        case 2: // 使用技能
        {
            printf("\n可用技能:\n");
            int skill_count = 0;
            int available_skills[20];

            for (int i = 0; i < game->learned_skill_count; i++)
            {
                int skill_index = game->learned_skills[i];
                Skill *skill = &game->skills[skill_index];

                // 检查玩家等级是否满足技能要求
                if (game->player.level >= skill->required_level)
                {
                    if (game->player.mp >= skill->mp_cost)
                    {
                        printf("%d. %s (消耗%d MP)\n", skill_count + 1, skill->name, skill->mp_cost);
                    }
                    else
                    {
                        printf("%d. %s (消耗%d MP) [MP不足]\n", skill_count + 1, skill->name, skill->mp_cost);
                    }
                    available_skills[skill_count] = skill_index;
                    skill_count++;
                }
            }

            if (skill_count == 0)
            {
                printf("你目前没有可以使用的技能！\n");
                continue;
            }

            printf("请选择技能 (0返回): ");
            int skill_choice;
            scanf("%d", &skill_choice);

            if (skill_choice == 0)
                continue;

            skill_choice--;

            if (skill_choice >= 0 && skill_choice < skill_count)
            {
                Skill *skill = &game->skills[available_skills[skill_choice]];

                if (game->player.mp >= skill->mp_cost)
                {
                    game->player.mp -= skill->mp_cost;

                    int base_damage = skill->damage + game->player.attack;  // 技能伤害+玩家攻击
                    int intelligence_bonus = game->player.intelligence / 2; // 智力每2点增加1点技能伤害
                    damage = base_damage + intelligence_bonus;

                    enemy.hp -= damage;
                    printf("你使用%s对%s造成了%d点伤害！(技能伤害%d + 攻击力%d + 智力加成%d)\n",
                           skill->name, enemy.name, damage, skill->damage, game->player.attack, intelligence_bonus);

                    if (skill->heal > 0)
                    {
                        int heal_amount = skill->heal;
                        game->player.hp += heal_amount;
                        if (game->player.hp > game->player.max_hp)
                        {
                            game->player.hp = game->player.max_hp;
                        }
                        printf("你使用%s恢复了%d点生命值！\n", skill->name, heal_amount);
                    }

                    // 击败恶龙
                    if (enemy_type == 3 && enemy.hp <= 0 && !game->dragon_defeated)
                    {
                        printf("你击败了%s！\n", enemy.name);
                        game->player.exp += enemy.exp_reward;
                        game->player.gold += enemy.gold_reward;
                        printf("获得了%d经验值和%d金币！\n", enemy.exp_reward, enemy.gold_reward);

                        game->dragon_defeated = 1;

                        show_ending(game);

                        if (game->player.exp >= game->player.level * 100)
                        {
                            level_up(game);
                        }

                        return;
                    }

                    if (enemy.hp <= 0)
                    {
                        printf("你击败了%s！\n", enemy.name);
                        game->player.exp += enemy.exp_reward;
                        game->player.gold += enemy.gold_reward;
                        printf("获得了%d经验值和%d金币！\n", enemy.exp_reward, enemy.gold_reward);

                        if (game->player.exp >= game->player.level * 100)
                        {
                            level_up(game);
                        }
                        return;
                    }
                }
                else
                {
                    printf("MP不足，无法使用此技能！\n");
                    continue;
                }
            }
            else
            {
                printf("无效的技能选择。\n");
                continue;
            }
        }
        break;

        case 3: // 逃跑
            if (enemy_type == 3)
            {
                printf("恶龙的强大气息让你无法移动！\n");
            }
            else
            {
                int enemy_level = estimate_enemy_level(&enemy);

                // 根据等级与敌人等级差计算逃跑率
                int escape_chance = 50 + (game->player.level - enemy_level) * 5;

                escape_chance += (game->player.agility / 10) * 5;

                if (escape_chance < 10)
                    escape_chance = 10;
                if (escape_chance > 90)
                    escape_chance = 90;

                if (rand() % 100 < escape_chance)
                {
                    printf("你成功逃跑了！(逃跑率: %d%%)\n", escape_chance);
                    return;
                }
                else
                {
                    printf("逃跑失败！(逃跑率: %d%%)\n", escape_chance);
                }
            }
            break;

        default:
            printf("无效的选择。\n");
            continue;
        }

        if (enemy.hp > 0)
        {
            int dodge_chance = (game->player.agility / 10) * 5;
            if (dodge_chance > 90)
                dodge_chance = 90;

            if (rand() % 100 < dodge_chance)
            {
                printf("%s试图攻击你，但你敏捷地闪避开了！(闪避率: %d%%)\n", enemy.name, dodge_chance);
            }
            else
            {
                int damage = calculate_damage(enemy.attack, game->player.defense);
                game->player.hp -= damage;
                printf("%s对你造成了%d点伤害！\n", enemy.name, damage);
            }

            if (game->player.hp <= 0)
            {
                printf("你被%s击败了...\n", enemy.name);
                printf("游戏结束！\n");
                exit(0);
            }
        }
    }
}

void rest(GameData *game)
{

    if (game->locations[game->current_location].type == 0 ||
        game->locations[game->current_location].type == 4)
    {
        int restore_hp = game->player.max_hp - game->player.hp;
        int restore_mp = game->player.max_mp - game->player.mp;

        game->player.hp = game->player.max_hp;
        game->player.mp = game->player.max_mp;

        printf("你在这里好好休息了一番...\n");
        printf("恢复了%d点生命值和%d点魔法值！\n", restore_hp, restore_mp);
    }
    else
    {
        printf("只有在城镇或安全地点才能安全地休息！\n");
    }
}

void level_up(GameData *game)
{
    while (game->player.exp >= game->player.level * 100)
    {
        game->player.level++;
        game->player.max_hp += 20;
        game->player.hp = game->player.max_hp;
        game->player.max_mp += 10;
        game->player.mp = game->player.max_mp;
        game->player.attack += 5;
        game->player.defense += 2;
        game->player.agility += 3;
        game->player.intelligence += 2;

        printf("恭喜升级到 %d 级！\n", game->player.level);
        printf("生命值 +%d，魔法值 +%d  ", 20, 10);
        printf("攻击力 +%d，防御力 +%d  ", 5, 2);
        printf("敏捷 +%d，智力 +%d\n", 3, 2);
    }
}

int calculate_damage(int attacker_attack, int defender_defense)
{
    int base_damage = attacker_attack - (defender_defense / 2);
    if (base_damage < 1)
        base_damage = 1;

    int variance = base_damage / 4;
    if (variance < 1)
        variance = 1;

    int final_damage = base_damage + (rand() % (variance * 2)) - variance;
    if (final_damage < 1)
        final_damage = 1;

    return final_damage;
}

void load_game(GameData *game)
{
    FILE *file = fopen("savegame.dat", "rb");
    if (file == NULL)
    {
        printf("无法加载游戏。\n");
        return;
    }

    fread(game, sizeof(GameData), 1, file);
    fclose(file);
    printf("游戏已加载。\n");
}

void talk_to_npc(GameData *game)
{
    int npc_count = 0;
    int npc_indices[15];

    switch (game->current_location)
    {
    case 0:                  // 瓦纳卡村
        npc_indices[0] = 0;  // 武器商人
        npc_indices[1] = 1;  // 村长
        npc_indices[2] = 2;  // 防具商人
        npc_indices[3] = 3;  // 药剂师
        npc_indices[4] = 16; // 村民
        npc_indices[5] = 17; // 老者
        npc_indices[6] = 19; // 神秘女子
        npc_count = 7;
        printf("==========可以交谈的NPC==========\n");
        for (int i = 0; i < npc_count; i++)
        {
            printf("%d. %s\n", i + 1, game->npcs[npc_indices[i]].name);
        }
        printf("请选择要交谈的NPC：\n");
        break;
    case 4:                  // 王城
        npc_indices[0] = 2;  // 防具商人
        npc_indices[1] = 3;  // 药剂师
        npc_indices[2] = 4;  // 技能导师
        npc_indices[3] = 5;  // 国王
        npc_indices[4] = 8;  // 铁匠
        npc_indices[5] = 11; // 图书管理员
        npc_count = 6;
        printf("==========可以交谈的NPC==========\n");
        for (int i = 0; i < npc_count; i++)
        {
            printf("%d. %s\n", i + 1, game->npcs[npc_indices[i]].name);
        }
        printf("请选择要交谈的NPC：\n");
        break;
    case 9:                  // 海盗港湾
        npc_indices[0] = 0;  // 武器商人
        npc_indices[1] = 6;  // 船长
        npc_indices[2] = 10; // 老渔夫
        npc_count = 3;
        break;
        printf("==========可以交谈的NPC==========\n");
        for (int i = 0; i < npc_count; i++)
        {
            printf("%d. %s\n", i + 1, game->npcs[npc_indices[i]].name);
        }
        printf("请选择要交谈的NPC：\n");
    case 8:                 // 精灵之森
        npc_indices[0] = 4; // 技能导师
        npc_indices[1] = 7; // 精灵长老
        npc_count = 2;
        break;
        printf("==========可以交谈的NPC==========\n");
        for (int i = 0; i < npc_count; i++)
        {
            printf("%d. %s\n", i + 1, game->npcs[npc_indices[i]].name);
        }
        printf("请选择要交谈的NPC：\n");
    case 13:                 // 魔法学院
        npc_indices[0] = 4;  // 技能导师
        npc_indices[1] = 11; // 图书管理员
        npc_indices[2] = 13; // 炼金术士
        npc_indices[3] = 14; // 占卜师
        npc_count = 4;
        printf("==========可以交谈的NPC==========\n");
        for (int i = 0; i < npc_count; i++)
        {
            printf("%d. %s\n", i + 1, game->npcs[npc_indices[i]].name);
        }
        printf("请选择要交谈的NPC：\n");
        break;
    case 10:                 // 火山口
        npc_indices[0] = 12; // 赏金猎人
        npc_count = 1;
        break;
    case 12:                 // 黑暗沼泽
        npc_indices[0] = 12; // 赏金猎人
        npc_count = 1;
        printf("==========可以交谈的NPC==========\n");
        for (int i = 0; i < npc_count; i++)
        {
            printf("%d. %s\n", i + 1, game->npcs[npc_indices[i]].name);
        }
        printf("请选择要交谈的NPC：\n");
        break;
    default:
        printf("这里没有可以交谈的NPC。\n");
        return;
    }

    int choice;
    scanf("%d", &choice);

    if (choice == 0)
        return;

    choice--;

    if (choice >= 0 && choice < npc_count)
    {
        int npc_index = npc_indices[choice];

        // 根据恶龙是否被击败显示不同对话
        if (game->dragon_defeated &&
            (npc_index == 1 || npc_index == 5 || npc_index == 16 || npc_index == 19))
        {
            // 恶龙被击败后
            switch (npc_index)
            {
            case 1: // 村长
                printf("\n%s: \"伟大的勇者！你拯救了我们所有人！\"", game->npcs[npc_index].name);
                printf("\n%s: \"整个村庄都在庆祝你的胜利！\"", game->npcs[npc_index].name);
                break;
            case 5: // 国王
                printf("\n%s: \"伟大的英雄！您拯救了整个王国！人民将永远铭记你的功绩。\"", game->npcs[npc_index].name);
                printf("\n%s: \"王国的和平与繁荣都归功于你！\"", game->npcs[npc_index].name);
                printf("\n%s: \"你的事迹将被各地传颂。\"", game->npcs[npc_index].name);
                break;
            case12: // 赏金猎人
                printf("\n%s:恭喜！你我都圆满完成各自的使命！", game->npcs[npc_index].name);
            case 14: // 占卜师
                printf("\n%s: \"你果然做到了，打破了既定的命运！\"", game->npcs[npc_index].name);
                printf("\n%s: \"但你仍需小心前方的道路。\"", game->npcs[npc_index].name);
            case 16: // 村民
                printf("\n%s: \"英雄！感谢你拯救了我们的村庄！\"", game->npcs[npc_index].name);
                printf("\n%s: \"你将是我们传说中永远的英雄！\"", game->npcs[npc_index].name);
                break;
            case 17: // 老者
                printf("\n%s: \"力量会随岁月流逝，但勇气不会。！\"", game->npcs[npc_index].name);
            case 19: // 神秘女子
                printf("\n%s: \"命运的轨迹已经改变，光明重新回到了这个世界。\"", game->npcs[npc_index].name);
                printf("\n%s: \"你的勇气将被永远铭记\"", game->npcs[npc_index].name);
                break;
            }
        }
        else
        {
            printf("\n%s: \"%s\"\n", game->npcs[npc_index].name, game->npcs[npc_index].dialog);

            for (int i = 0; i < game->npcs[npc_index].additional_dialogs_count; i++)
            {
                printf("%s: \"%s\"\n", game->npcs[npc_index].name, game->npcs[npc_index].additional_dialogs[i]);
            }
        }

        if (npc_index == 18)
        {
            printf("\n%s: \"在我的旅店里休息一晚，就可以完全恢复你的全部状态。\"");
            printf("\n是否要休息一晚？(y/n): ");
            char rest_choice;
            scanf(" %c", &rest_choice);
            if (rest_choice == 'y' || rest_choice == 'Y')
            {
                if (game->player.gold >= game->npcs[npc_index].item_price)
                {
                    game->player.gold -= game->npcs[npc_index].item_price;
                    int restore_hp = game->player.max_hp - game->player.hp;
                    int restore_mp = game->player.max_mp - game->player.mp;
                    game->player.hp = game->player.max_hp;
                    game->player.mp = game->player.max_mp;
                    printf("你在旅店里好好休息了一番...\n");
                    printf("恢复了%d点生命值和%d点魔法值！\n", restore_hp, restore_mp);
                }
            }
        }

        if (npc_index == 0 || npc_index == 2 || npc_index == 3 || npc_index == 8 || npc_index == 9 || npc_index == 13)
        {
            printf("\n%s愿意与你交易。\n", game->npcs[npc_index].name);
            printf("是否要看看他的商品？(y/n): ");
            char shop_choice;
            scanf(" %c", &shop_choice);
            if (shop_choice == 'y' || shop_choice == 'Y')
            {
                shop_menu(game, npc_index);
            }
        }
        else if (npc_index == 4)
        {
            printf("\n%s可以教你新技能。\n", game->npcs[npc_index].name);
            printf("是否要学习新技能？(y/n): ");
            char learn_choice;
            scanf(" %c", &learn_choice);
            if (learn_choice == 'y' || learn_choice == 'Y')
            {
                learn_skills(game);
            }
        }
    }
    else
    {
        printf("无效的选择。\n");
    }
}

void shop_menu(GameData *game, int npc_index)
{
    Npc *npc = &game->npcs[npc_index];

    printf("\n========== %s的商店 ==========\n", npc->name);
    for (int i = 0; i < npc->shop_item_count; i++)
    {
        int item_index = npc->shop_items[i];
        Item *item = &game->items[item_index];
        printf("%d. %s - %d金币 ", i + 1, item->name, item->price);
        switch (item->type)
        {
        case 0:
            printf("(武器: +%d攻击)", item->value);
            break;
        case 1:
            printf("(防具: +%d防御)", item->value);
            break;
        case 2:
            printf("(消耗品: 恢复%d HP)", item->value);
            break;
        }
        printf("\n");
    }
    printf("你有%d金币。\n", game->player.gold);
    printf("请选择要购买的物品 (0返回): ");

    int choice;
    scanf("%d", &choice);

    if (choice == 0)
        return;

    choice--;

    if (choice >= 0 && choice < npc->shop_item_count)
    {
        int item_index = npc->shop_items[choice];
        Item *item = &game->items[item_index];

        if (game->player.gold >= item->price)
        {
            if (game->inventory_count < MAX_INVENTORY)
            {
                game->player.gold -= item->price;
                game->inventory[game->inventory_count] = *item;
                game->inventory_count++;
                printf("你购买了%s！\n", item->name);
            }
            else
            {
                printf("背包已满！\n");
            }
        }
        else
        {
            printf("金币不足！\n");
        }
    }
    else
    {
        printf("无效的选择。\n");
    }
}

void show_inventory(GameData *game)
{
    printf("\n========== 背包 ==========\n");
    if (game->inventory_count == 0)
    {
        printf("背包是空的。\n");
    }
    else
    {
        for (int i = 0; i < game->inventory_count; i++)
        {
            printf("%d. %s", i + 1, game->inventory[i].name);
            switch (game->inventory[i].type)
            {
            case 0:
                printf(" (武器: +%d攻击)", game->inventory[i].value);
                break;
            case 1:
                printf(" (防具: +%d防御)", game->inventory[i].value);
                break;
            case 2:
                printf(" (消耗品: 恢复%d HP)", game->inventory[i].value);
                break;
            }
            printf("\n");
        }
    }
    printf("========================\n");
}

void use_item(GameData *game)
{
    if (game->inventory_count == 0)
    {
        printf("背包是空的。\n");
        return;
    }

    show_inventory(game);
    printf("请选择要使用的物品 (输入编号，0取消): ");

    int choice;
    scanf("%d", &choice);

    if (choice == 0)
        return;

    choice--;

    if (choice >= 0 && choice < game->inventory_count)
    {
        Item *item = &game->inventory[choice];

        switch (item->type)
        {
        case 0: // 武器
            printf("你装备了%s，增加了%d点攻击力！\n", item->name, item->value);
            // 移除物品
            for (int i = choice; i < game->inventory_count - 1; i++)
            {
                game->inventory[i] = game->inventory[i + 1];
            }
            game->inventory_count--;
            break;
        case 1: // 防具
            printf("你装备了%s，增加了%d点防御力！\n", item->name, item->value);

            for (int i = choice; i < game->inventory_count - 1; i++)
            {
                game->inventory[i] = game->inventory[i + 1];
            }
            game->inventory_count--;
            break;
        case 2: // 消耗品
            if (strcmp(item->name, "力量药剂") == 0)
            {
                game->player.attack += 5;
                printf("你使用了%s，永久增加了5点攻击力！\n", item->name);
            }
            else if (strcmp(item->name, "敏捷药剂") == 0)
            {
                game->player.agility += 5;
                printf("你使用了%s，永久增加了5点敏捷！\n", item->name);
            }
            else if (strcmp(item->name, "智力药剂") == 0)
            {
                game->player.intelligence += 5;
                game->player.max_mp += 20;
                game->player.mp += 20;
                if (game->player.mp > game->player.max_mp)
                {
                    game->player.mp = game->player.max_mp;
                }
                printf("你使用了%s，永久增加了5点智力和20点最大魔法值！\n", item->name);
            }
            else
            {
                game->player.hp += item->value;
                if (game->player.hp > game->player.max_hp)
                {
                    game->player.hp = game->player.max_hp;
                }
                printf("你使用了%s，恢复了%d点生命值！\n", item->name, item->value);
            }

            //移除已使用的消耗品
            for (int i = choice; i < game->inventory_count - 1; i++)
            {
                game->inventory[i] = game->inventory[i + 1];
            }
            game->inventory_count--;
            break;
        }
    }
    else
    {
        printf("无效的选择。\n");
    }
}

void save_game(GameData *game)
{
    FILE *file = fopen("savegame.dat", "wb");
    if (file == NULL)
    {
        printf("无法保存游戏！\n");
        return;
    }

    fwrite(game, sizeof(GameData), 1, file);
    fclose(file);
    printf("游戏已保存。\n");
}

int file_exists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

// 学习新技能
void learn_skills(GameData *game)
{
    printf("\n========== 可学习的技能 ==========\n");
    int available_skills = 0;
    int available_skill_indices[MAX_SKILLS];

    for (int i = 0; i < MAX_SKILLS && i < 19; i++) // 限制在实际定义的范围内
    {
        int learned = 0;
        // 检查技能是否已学会
        for (int j = 0; j < game->learned_skill_count; j++)
        {
            if (game->learned_skills[j] == i)
            {
                learned = 1;
                break;
            }
        }

        // 检查等级是否满足要求
        if (!learned && game->player.level >= game->skills[i].required_level)
        {
            printf("%d. %s (需要等级: %d)",
                   available_skills + 1,
                   game->skills[i].name,
                   game->skills[i].required_level);

            if (game->skills[i].damage > 0)
            {
                printf(" - 造成%d点伤害", game->skills[i].damage);
            }
            if (game->skills[i].heal > 0)
            {
                printf(" - 恢复%d点生命", game->skills[i].heal);
            }
            printf("\n");

            available_skill_indices[available_skills] = i;
            available_skills++;
        }
    }

    if (available_skills == 0)
    {
        printf("当前没有可学习的新技能。\n");
        return;
    }

    printf("请选择要学习的技能 (0返回): ");
    int choice;
    scanf("%d", &choice);

    if (choice == 0)
        return;

    if (choice > 0 && choice <= available_skills)
    {
        int skill_index = available_skill_indices[choice - 1];

        if (game->learned_skill_count < MAX_SKILLS)
        {
            game->learned_skills[game->learned_skill_count] = skill_index;
            game->learned_skill_count++;
            printf("你学会了新技能：%s！\n", game->skills[skill_index].name);
        }
        else
        {
            printf("技能栏已满！\n");
        }
    }
    else
    {
        printf("无效的选择。\n");
    }
}

void cheat_game(GameData *game)
{
    printf("========== 作弊列表 ==========\n");
    printf("1. 添加2000点经验\n");
    printf("2. 添加2000点金币\n");
    printf("3. 添加100点生命值\n");
    printf("4. 添加100点魔法值\n");
    printf("5. 添加100点攻击力\n");
    printf("6. 添加100点防御力\n");
    printf("7. 添加100点敏捷\n");
    printf("8. 添加100点智力\n");
    printf("请选择要使用的作弊 (0返回): ");
    int cheat_choice;
    scanf("%d", &cheat_choice);

    switch (cheat_choice)
    {
    case 1:
        game->player.exp += 2000;
        printf("已添加2000点经验！\n");
        if (game->player.exp >= game->player.level * 100)
        {
            level_up(game);
        }
        break;
    case 2:
        game->player.gold += 2000;
        printf("已添加2000点金币！\n");
        break;
    case 3:
        game->player.max_hp += 100;
        printf("已添加100点生命值！\n");
        break;
    case 4:
        game->player.max_mp += 100;
        printf("已添加100点魔法值！\n");
        break;
    case 5:
        game->player.attack += 100;
        printf("已添加100点攻击力！\n");
        break;
    case 6:
        game->player.defense += 100;
        printf("已添加100点防御力！\n");
        break;
    case 7:
        game->player.agility += 100;
        printf("已添加100点敏捷！\n");
        break;
    case 8:
        game->player.intelligence += 100;
        printf("已添加100点智力！\n");
        break;
    case 0:
        main_menu(game);
        break;
    default:
        printf("无效的选择。\n");
        main_menu(game);
    }

    return;
}


