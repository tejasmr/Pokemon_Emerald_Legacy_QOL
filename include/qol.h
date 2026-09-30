#ifndef GUARD_QOL_H
#define GUARD_QOL_H

#define CURRENT_QOL_OPTIONS_NUM 15
#define QOL_OPTIONS_PER_PAGE 5
#define QOL_MAX_PAGES 3
#define ALL_QOL_OPTIONS_PER_PAGE (QOL_OPTIONS_PER_PAGE + 2)

#define QOL_FONT_ID 1

enum QolOption
{
    // Page 1: Game Pacing
    QOL_PRESET,
    QOL_HOLD_A,
    QOL_BATTLE_SPEED,
    QOL_QUICK_ANIMS,
    QOL_FANFARES,

    // Page 2: Convenience & Items
    QOL_FAST_HEALING,
    QOL_WALLY_TUTORIAL,
    QOL_EARLY_RUN,
    QOL_INFINITE_TMS,
    QOL_MOD_ITEMS,

    // Page 3: Pokémon & Battles
    QOL_EXP_MULTIPLIER,
    QOL_CATCH_RATE,
    QOL_SHINY_RATE,
    QOL_PERFECT_IVS,
    QOL_PREFER_NATURE,

    // Navigation items
    QOL_PAGE,
    QOL_START_GAME
};

enum
{
    QOL_PAGE_FIRST,
    QOL_PAGE_LAST
};

// Presets
enum
{
    QOL_PRESET_DEFAULT,
    QOL_PRESET_VANILLA,
    QOL_PRESET_CUSTOM
};

// Hold A
enum
{
    QOL_HOLD_A_YES,
    QOL_HOLD_A_NO
};

// Battle Speed
enum
{
    QOL_BATTLE_SPEED_FAST,
    QOL_BATTLE_SPEED_VANILLA
};

// Quick Animations
enum
{
    QOL_ANIMS_SHORT,
    QOL_ANIMS_VANILLA
};

// Fanfares
enum
{
    QOL_FANFARES_SKIP,
    QOL_FANFARES_VANILLA
};

// Fast Healing
enum
{
    QOL_HEALING_FAST,
    QOL_HEALING_VANILLA
};

// Wally Tutorial
enum
{
    QOL_WALLY_SKIP,
    QOL_WALLY_SHORT,
    QOL_WALLY_VANILLA
};

// Early Running Shoes
enum
{
    QOL_EARLY_RUN_ON,
    QOL_EARLY_RUN_OFF
};

// Infinite TMs
enum
{
    QOL_INFINITE_TMS_ON,
    QOL_INFINITE_TMS_OFF
};

// Mod Items
enum
{
    QOL_MOD_ITEMS_ON,
    QOL_MOD_ITEMS_OFF
};

// EXP Multiplier
enum
{
    QOL_EXP_TRIPLE,
    QOL_EXP_VANILLA
};

// Catch Rate
enum
{
    QOL_CATCH_100,
    QOL_CATCH_VANILLA
};

// Shiny Rate
enum
{
    QOL_SHINY_ALL,
    QOL_SHINY_STARTER,
    QOL_SHINY_VANILLA
};

// Perfect IVs
enum
{
    QOL_IVS_MAX,
    QOL_IVS_RANDOM
};

// Prefer Nature
enum
{
    QOL_NATURE_ON,
    QOL_NATURE_OFF
};

struct QolSaveOptions
{
    u8 preset:2;
    u8 holdA:1;
    u8 battleSpeed:1;
    u8 quickAnims:1;
    u8 fanfares:1;
    u8 fastHealing:1;
    u8 wallyTutorial:2;
    u8 earlyRun:1;
    u8 infiniteTms:1;
    u8 modItems:1;
    u8 expMultiplier:1;
    u8 catchRate:1;
    u8 shinyRate:2;
    u8 perfectIvs:1;
    u8 preferNature:1;
};

void CB2_InitQolMenu(void);
void Task_InitQolMenu(u8 taskId);
bool8 CheckQolOption(u8 option, u8 selection);
u8 GetQolOption(u8 option);

#endif // GUARD_QOL_H
