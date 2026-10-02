#include "global.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "sound.h"
#include "constants/songs.h"
#include "constants/rgb.h"
#include "string_util.h"
#include "text.h"
#include "qol.h"
#include "gpu_regs.h"
#include "bg.h"
#include "scanline_effect.h"
#include "text_window.h"
#include "main_menu.h"
#include "item.h"
#include "overworld.h"
#include "event_data.h"
#include "constants/items.h"

enum
{
    QOL_WIN_HEADER,
    QOL_WIN_OPTIONS,
    QOL_WIN_TOOLTIP,
    QOL_WIN_YESNO,
};

static const struct BgTemplate sQolMenuBgTemplates[] =
{
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 0,
        .charBaseIndex = 1,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 1,
        .mapBaseIndex = 29,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    }
};

static const struct WindowTemplate sQolMenuWinTemplates[] =
{
    [QOL_WIN_HEADER] = {
        .bg = 1,
        .tilemapLeft = 2,
        .tilemapTop = 1,
        .width = 26,
        .height = 2,
        .paletteNum = 1,
        .baseBlock = 2
    },
    [QOL_WIN_OPTIONS] = {
        .bg = 0,
        .tilemapLeft = 2,
        .tilemapTop = 5,
        .width = 26,
        .height = 14,
        .paletteNum = 1,
        .baseBlock = 0x36
    },
    [QOL_WIN_TOOLTIP] = {
        .bg = 2,
        .tilemapLeft = 2,
        .tilemapTop = 15,
        .width = 26,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 427
    },
    [QOL_WIN_YESNO] = {
        .bg = 2,
        .tilemapLeft = 23,
        .tilemapTop = 9,
        .width = 4,
        .height = 4,
        .paletteNum = 15,
        .baseBlock = 531
    },
    DUMMY_WIN_TEMPLATE
};

static const u16 sQolMenuBg_Pal[] = {RGB(17, 18, 31)};
static const u16 sQolMenuText_Pal[] = INCBIN_U16("graphics/interface/option_menu_text.gbapal");

/* ----------------------------------------------- */
/* QOL MENU TEXT (Header & System Text)            */
/* ----------------------------------------------- */
static const u8 sText_Header[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}QOL CONFIGURATION MENU");
static const u8 sText_Version[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}v1.2.3");

/* ----------------------------------------------- */
/* QOL MENU TEXT (Option Choices)                  */
/* ----------------------------------------------- */
static const u8 sText_Default[]  = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}DEFAULT");
static const u8 sText_Vanilla[]  = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}VANILLA");
static const u8 sText_Custom[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}CUSTOM");

static const u8 sText_On[]       = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}ON");
static const u8 sText_Off[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}OFF");
static const u8 sText_Yes[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}YES");
static const u8 sText_No[]       = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}NO");

static const u8 sText_Fast[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}FAST");
static const u8 sText_Short[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}SHORT");
static const u8 sText_Skip[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}SKIP");
static const u8 sText_Easy[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EASY");

static const u8 sText_100Pct[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}100%");
static const u8 sText_All[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}ALL");
static const u8 sText_Starter[]  = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}STARTER");

static const u8 sText_Max31[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}MAX 31");
static const u8 sText_Random[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}RANDOM");

static const u8 sText_Triple[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}TRIPLE");

static const u8 sText_Page1[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}1/4");
static const u8 sText_Page2[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}2/4");
static const u8 sText_Page3[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}3/4");
static const u8 sText_Page4[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}4/4");

/* ----------------------------------------------- */
/* QOL MENU TEXT (Option Names)                    */
/* ----------------------------------------------- */
static const u8 sOption_Preset[]        = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}PRESET");
static const u8 sOption_HoldA[]         = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}HOLD A BUTTON");
static const u8 sOption_BattleSpeed[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}BATTLE SPEED");
static const u8 sOption_QuickAnims[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}QUICK ANIMS");
static const u8 sOption_RunTrainer[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}TRAINER RUN");

static const u8 sOption_Fanfares[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}FANFARES");
static const u8 sOption_FastHealing[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}FAST HEALING");
static const u8 sOption_WallyTutorial[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}WALLY TUTORIAL");
static const u8 sOption_EarlyRun[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EARLY SHOES");
static const u8 sOption_InfiniteTms[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}INFINITE TMS");

static const u8 sOption_ModItems[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}MOD ITEMS");
static const u8 sOption_EasyFishing[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EASY FISHING");
static const u8 sOption_ExpMultiplier[] = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}EXP RATE");
static const u8 sOption_CatchRate[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}CATCH RATE");
static const u8 sOption_ShinyRate[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}SHINIES");

static const u8 sOption_PerfectIvs[]    = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}PERFECT IVS");
static const u8 sOption_PreferNature[]  = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}PREFER NATURE");
static const u8 sOption_FreeHms[]       = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}FREE HMS");
static const u8 sOption_AlwaysFlash[]   = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}ALWAYS FLASH");

static const u8 sOption_Page[]          = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}PAGE");
static const u8 sOption_StartGame[]     = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}START GAME");
static const u8 sOption_SaveExit[]      = _("{COLOR GREEN}{SHADOW LIGHT_GREEN}SAVE & EXIT");

static const u8 sOption_LeftArrow[]     = _("{COLOR RED}{SHADOW LIGHT_RED}{LEFT_ARROW}");
static const u8 sOption_RightArrow[]    = _("{COLOR RED}{SHADOW LIGHT_RED}{RIGHT_ARROW}");

/* ----------------------------------------------- */
/* QOL MENU TEXT (Tooltips)                        */
/* ----------------------------------------------- */
static const u8 sTooltip_Explanation[]    = _("Configure Quality of Life options.\nPress LEFT/RIGHT to change values.\nPress SELECT for option explanation.\nSelect START GAME when ready.");
static const u8 sTooltip_Preset[]         = _("Quickly select between preset configurations.\nDEFAULT: All QOL features enabled.\nVANILLA: Original Emerald mechanics.\nCUSTOM: Customized settings.");
static const u8 sTooltip_HoldA[]          = _("YES: Holding A accelerates text printing\ninstantly.\nNO: Text prints at standard option speed.");
static const u8 sTooltip_BattleSpeed[]    = _("FAST: Quick intro & send-out animations.\nVANILLA: Standard battle speed.");
static const u8 sTooltip_QuickAnims[]     = _("SHORT: Fast intro fades, trainer sprints,\nevolution, eggs & instant map warps.\nVANILLA: Standard animation flow.");
static const u8 sTooltip_RunTrainer[]     = _("ON: Allows running from regular trainer\nbattles without registering a loss or defeat.\nOFF: Standard trainer battle rules.");

static const u8 sTooltip_Fanfares[]       = _("SKIP: Silence level-up, item, and badge\nfanfares for faster pacing.\nVANILLA: Play all standard fanfares.");
static const u8 sTooltip_FastHealing[]    = _("FAST: Quick Poké Center healing without\nlengthy Nurse Joy dialogues.\nVANILLA: Standard healing sequence.");
static const u8 sTooltip_WallyTutorial[]  = _("SKIP: Completely skip catching tutorial.\nSHORT: Faster in-battle pacing only.\nVANILLA: Original tutorial pacing.");
static const u8 sTooltip_EarlyRun[]       = _("ON: Start game with Running Shoes &\nB-Button Dash enabled.\nOFF: Receive shoes normally.");
static const u8 sTooltip_InfiniteTms[]    = _("ON: TMs are reusable and not consumed.\nOFF: TMs are single use.");

static const u8 sTooltip_ModItems[]       = _("ON: Receive Porta Heal, EV Editor, Porta\nFly, etc., after choosing starter.\nOFF: Do not receive mod items.");
static const u8 sTooltip_EasyFishing[]    = _("EASY: Fast dots and 1-round bite hook.\n100%: Fast 1-round bites with 100% bite rate.\nVANILLA: Standard fishing mechanics.");
static const u8 sTooltip_ExpMultiplier[]  = _("TRIPLE: 3x Experience points gained.\nVANILLA: Normal experience rate.");
static const u8 sTooltip_CatchRate[]      = _("100%: All Poké Balls have guaranteed\ncatch rate.\nVANILLA: Standard catch calculation.");
static const u8 sTooltip_ShinyRate[]      = _("ALL: All wild & starter Pokémon shiny.\nSTARTER: Starter Pokémon is shiny.\nVANILLA: 1/8192 shiny odds.");

static const u8 sTooltip_PerfectIvs[]     = _("MAX 31: All caught and hatched Pokémon\nhave 31 IVs across all stats.\nRANDOM: Standard random IVs.");
static const u8 sTooltip_PreferNature[]   = _("ON: Pokémon automatically receive optimal\nnature (Adamant/Modest/etc.)\nOFF: Standard random natures.");
static const u8 sTooltip_FreeHms[]        = _("ON: Use overworld HMs (Cut, Rock Smash)\nwithout learning move if badge is obtained.\nOFF: Must know the HM move.");
static const u8 sTooltip_AlwaysFlash[]    = _("ON: Dark caves are automatically lit up\nwith Flash without using the move.\nOFF: Must use Flash to light up caves.");
static const u8 sTooltip_Page[]           = _("Switch between QOL configuration pages.\nPress LEFT/RIGHT or L/R triggers to flip\npages.");
static const u8 sTooltip_StartGame[]      = _("Save configured Quality of Life options\nand proceed to begin your adventure!");
static const u8 sTooltip_SaveExit[]       = _("Save configured Quality of Life options\nand return to the game.");

/* ----------------------------------------------- */
/* OPTION DEFINITION STRUCTS                       */
/* ----------------------------------------------- */
struct QolOptionData
{
    const u8 *name;
    const u8 *const *choices;
    u8 numChoices;
    const u8 *tooltip;
};

static const u8 *const sChoices_Preset[]        = { sText_Default, sText_Vanilla, sText_Custom };
static const u8 *const sChoices_HoldA[]         = { sText_Yes, sText_No };
static const u8 *const sChoices_BattleSpeed[]   = { sText_Fast, sText_Vanilla };
static const u8 *const sChoices_QuickAnims[]    = { sText_Short, sText_Vanilla };
static const u8 *const sChoices_RunTrainer[]    = { sText_On, sText_Off };

static const u8 *const sChoices_Fanfares[]      = { sText_Skip, sText_Vanilla };
static const u8 *const sChoices_FastHealing[]   = { sText_Fast, sText_Vanilla };
static const u8 *const sChoices_WallyTutorial[] = { sText_Skip, sText_Short, sText_Vanilla };
static const u8 *const sChoices_EarlyRun[]      = { sText_On, sText_Off };
static const u8 *const sChoices_InfiniteTms[]   = { sText_On, sText_Off };

static const u8 *const sChoices_ModItems[]      = { sText_On, sText_Off };
static const u8 *const sChoices_EasyFishing[]   = { sText_Easy, sText_100Pct, sText_Vanilla };
static const u8 *const sChoices_ExpMultiplier[] = { sText_Triple, sText_Vanilla };
static const u8 *const sChoices_CatchRate[]     = { sText_100Pct, sText_Vanilla };
static const u8 *const sChoices_ShinyRate[]     = { sText_All, sText_Starter, sText_Vanilla };

static const u8 *const sChoices_PerfectIvs[]    = { sText_Max31, sText_Random };
static const u8 *const sChoices_PreferNature[]  = { sText_On, sText_Off };
static const u8 *const sChoices_FreeHms[]       = { sText_On, sText_Off };
static const u8 *const sChoices_AlwaysFlash[]   = { sText_On, sText_Off };

static const u8 *const sChoices_Page[]          = { sText_Page1, sText_Page2, sText_Page3, sText_Page4 };

static const struct QolOptionData sQolOptions[CURRENT_QOL_OPTIONS_NUM + 1] =
{
    // Page 1
    [QOL_PRESET]             = { sOption_Preset,         sChoices_Preset,        3, sTooltip_Preset },
    [QOL_HOLD_A]             = { sOption_HoldA,          sChoices_HoldA,         2, sTooltip_HoldA },
    [QOL_BATTLE_SPEED]       = { sOption_BattleSpeed,    sChoices_BattleSpeed,   2, sTooltip_BattleSpeed },
    [QOL_QUICK_ANIMS]        = { sOption_QuickAnims,     sChoices_QuickAnims,    2, sTooltip_QuickAnims },
    [QOL_RUN_TRAINER_BATTLE] = { sOption_RunTrainer,     sChoices_RunTrainer,    2, sTooltip_RunTrainer },

    // Page 2
    [QOL_FANFARES]           = { sOption_Fanfares,       sChoices_Fanfares,      2, sTooltip_Fanfares },
    [QOL_FAST_HEALING]       = { sOption_FastHealing,    sChoices_FastHealing,   2, sTooltip_FastHealing },
    [QOL_WALLY_TUTORIAL]     = { sOption_WallyTutorial,  sChoices_WallyTutorial, 3, sTooltip_WallyTutorial },
    [QOL_EARLY_RUN]          = { sOption_EarlyRun,       sChoices_EarlyRun,      2, sTooltip_EarlyRun },
    [QOL_INFINITE_TMS]       = { sOption_InfiniteTms,    sChoices_InfiniteTms,   2, sTooltip_InfiniteTms },

    // Page 3
    [QOL_MOD_ITEMS]          = { sOption_ModItems,       sChoices_ModItems,      2, sTooltip_ModItems },
    [QOL_EASY_FISHING]       = { sOption_EasyFishing,    sChoices_EasyFishing,   3, sTooltip_EasyFishing },
    [QOL_EXP_MULTIPLIER]     = { sOption_ExpMultiplier,  sChoices_ExpMultiplier, 2, sTooltip_ExpMultiplier },
    [QOL_CATCH_RATE]         = { sOption_CatchRate,      sChoices_CatchRate,     2, sTooltip_CatchRate },
    [QOL_SHINY_RATE]         = { sOption_ShinyRate,      sChoices_ShinyRate,     3, sTooltip_ShinyRate },

    // Page 4
    [QOL_PERFECT_IVS]        = { sOption_PerfectIvs,     sChoices_PerfectIvs,    2, sTooltip_PerfectIvs },
    [QOL_PREFER_NATURE]      = { sOption_PreferNature,   sChoices_PreferNature,  2, sTooltip_PreferNature },
    [QOL_FREE_HMS]           = { sOption_FreeHms,        sChoices_FreeHms,       2, sTooltip_FreeHms },
    [QOL_ALWAYS_FLASH]       = { sOption_AlwaysFlash,    sChoices_AlwaysFlash,   2, sTooltip_AlwaysFlash },

    [QOL_PAGE]               = { sOption_Page,           sChoices_Page,          4, sTooltip_Page },
};

/* ----------------------------------------------- */
/* PRESET CONFIGURATIONS                           */
/* ----------------------------------------------- */
static const u8 sPresetDefault[CURRENT_QOL_OPTIONS_NUM] =
{
    [QOL_PRESET]             = QOL_PRESET_DEFAULT,
    [QOL_HOLD_A]             = QOL_HOLD_A_YES,
    [QOL_BATTLE_SPEED]       = QOL_BATTLE_SPEED_FAST,
    [QOL_QUICK_ANIMS]        = QOL_ANIMS_SHORT,
    [QOL_RUN_TRAINER_BATTLE] = QOL_RUN_TRAINER_ON,
    [QOL_FANFARES]           = QOL_FANFARES_SKIP,
    [QOL_FAST_HEALING]       = QOL_HEALING_FAST,
    [QOL_WALLY_TUTORIAL]     = QOL_WALLY_SKIP,
    [QOL_EARLY_RUN]          = QOL_EARLY_RUN_ON,
    [QOL_INFINITE_TMS]       = QOL_INFINITE_TMS_ON,
    [QOL_MOD_ITEMS]          = QOL_MOD_ITEMS_ON,
    [QOL_EASY_FISHING]       = QOL_FISHING_EASY,
    [QOL_EXP_MULTIPLIER]     = QOL_EXP_TRIPLE,
    [QOL_CATCH_RATE]         = QOL_CATCH_100,
    [QOL_SHINY_RATE]         = QOL_SHINY_ALL,
    [QOL_PERFECT_IVS]        = QOL_IVS_MAX,
    [QOL_PREFER_NATURE]      = QOL_NATURE_ON,
    [QOL_FREE_HMS]           = QOL_FREE_HMS_ON,
    [QOL_ALWAYS_FLASH]       = QOL_ALWAYS_FLASH_ON,
};

static const u8 sPresetVanilla[CURRENT_QOL_OPTIONS_NUM] =
{
    [QOL_PRESET]             = QOL_PRESET_VANILLA,
    [QOL_HOLD_A]             = QOL_HOLD_A_NO,
    [QOL_BATTLE_SPEED]       = QOL_BATTLE_SPEED_VANILLA,
    [QOL_QUICK_ANIMS]        = QOL_ANIMS_VANILLA,
    [QOL_RUN_TRAINER_BATTLE] = QOL_RUN_TRAINER_OFF,
    [QOL_FANFARES]           = QOL_FANFARES_VANILLA,
    [QOL_FAST_HEALING]       = QOL_HEALING_VANILLA,
    [QOL_WALLY_TUTORIAL]     = QOL_WALLY_VANILLA,
    [QOL_EARLY_RUN]          = QOL_EARLY_RUN_OFF,
    [QOL_INFINITE_TMS]       = QOL_INFINITE_TMS_OFF,
    [QOL_MOD_ITEMS]          = QOL_MOD_ITEMS_OFF,
    [QOL_EASY_FISHING]       = QOL_FISHING_VANILLA,
    [QOL_EXP_MULTIPLIER]     = QOL_EXP_VANILLA,
    [QOL_CATCH_RATE]         = QOL_CATCH_VANILLA,
    [QOL_SHINY_RATE]         = QOL_SHINY_VANILLA,
    [QOL_PERFECT_IVS]        = QOL_IVS_RANDOM,
    [QOL_PREFER_NATURE]      = QOL_NATURE_OFF,
    [QOL_FREE_HMS]           = QOL_FREE_HMS_OFF,
    [QOL_ALWAYS_FLASH]       = QOL_ALWAYS_FLASH_OFF,
};

/* ----------------------------------------------- */
/* MENU STATE & TASK                               */
/* ----------------------------------------------- */
static EWRAM_DATA struct
{
    u8 optionConfig[CURRENT_QOL_OPTIONS_NUM];
    u8 pageNum;
    u8 pageIndex;
    u8 trueIndex;
    bool8 tooltipActive;
} sLocalQolConfig = {0};

static EWRAM_DATA int sQolTaskId = 0;
static EWRAM_DATA u8 sStoredPageNum = 0;

static void CB2_QolMenu(void);
static void VblankCB_QolMenu(void);
static void Task_QolMenuProcessInput(u8 taskId);
static void Task_QolMenuFadeIn(u8 taskId);
static void Task_QolMenuFadeOut(u8 taskId);
static void DrawHeaderWindow(void);
static void DrawPageOptions(u8 page);
static void DrawTooltip(u8 taskId, const u8 *str);
static void HideTooltip(void);
static void HighlightOptionMenuItem(u8 index);
static const u8 *GetCurrentOptionTooltip(void);
static void UpdateTooltipIfActive(u8 taskId);
static void LoadQolOptions(void);

static u8 GetPageOptionTrueIndex(u8 pos, u8 page)
{
    if (page == 0)
        return 0;

    if (pos == QOL_PAGE_FIRST)
        return (page - 1) * QOL_OPTIONS_PER_PAGE;
    else
    {
        u8 last = page * QOL_OPTIONS_PER_PAGE - 1;
        if (last >= CURRENT_QOL_OPTIONS_NUM)
            return CURRENT_QOL_OPTIONS_NUM - 1;
        return last;
    }
}

static void ApplyPreset(const u8 *preset)
{
    u8 i;
    for (i = 0; i < CURRENT_QOL_OPTIONS_NUM; i++)
        sLocalQolConfig.optionConfig[i] = preset[i];
}

void Task_InitQolMenu(u8 taskId)
{
    gMain.savedCallback = NULL;
    SetMainCallback2(CB2_InitQolMenu);
    DestroyTask(taskId);
}

void CB2_InitQolMenu(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        sQolTaskId = -1;
        gMain.state++;
        break;
    case 1:
    {
        DmaClearLarge16(3, (void *)(VRAM), VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        CpuFill16(0, (void *)(BG_PLTT), BG_PLTT_SIZE);
        FillPalette(RGB_BLACK, 0, PLTT_SIZE);
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sQolMenuBgTemplates, ARRAY_COUNT(sQolMenuBgTemplates));
        ChangeBgX(0, 0, BG_COORD_SET);
        ChangeBgY(0, 0, BG_COORD_SET);
        ChangeBgX(1, 0, BG_COORD_SET);
        ChangeBgY(1, 0, BG_COORD_SET);
        ChangeBgX(2, 0, BG_COORD_SET);
        ChangeBgY(2, 0, BG_COORD_SET);
        InitWindows(sQolMenuWinTemplates);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_WIN0H, 0);
        SetGpuReg(REG_OFFSET_WIN0V, 0);
        SetGpuReg(REG_OFFSET_WININ, WININ_WIN0_BG0);
        SetGpuReg(REG_OFFSET_WINOUT, WINOUT_WIN01_BG0 | WINOUT_WIN01_BG1 | WINOUT_WIN01_CLR);
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_DARKEN);
        SetGpuReg(REG_OFFSET_BLDALPHA, 0);
        SetGpuReg(REG_OFFSET_BLDY, 4);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON | DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        ShowBg(0);
        ShowBg(1);
        ShowBg(2);
        gMain.state++;
        break;
    }
    case 2:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        gMain.state++;
        break;
    case 3:
        LoadBgTiles(1, GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->tiles, 0x120, 0x1A2);
        gMain.state++;
        break;
    case 4:
        LoadPalette(sQolMenuBg_Pal, BG_PLTT_ID(0), sizeof(sQolMenuBg_Pal));
        LoadPalette(GetWindowFrameTilesPal(gSaveBlock2Ptr->optionsWindowFrameType)->pal, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
        LoadPalette(sQolMenuText_Pal, BG_PLTT_ID(1), sizeof(sQolMenuText_Pal));
        LoadUserWindowBorderGfx(QOL_WIN_TOOLTIP, 0x1D5, BG_PLTT_ID(13));
        LoadPalette(GetOverworldTextboxPalettePtr(), BG_PLTT_ID(14), PLTT_SIZE_4BPP);
        gMain.state++;
        break;
    case 5:
        PutWindowTilemap(QOL_WIN_HEADER);
        DrawHeaderWindow();
        PutWindowTilemap(QOL_WIN_OPTIONS);
        if (gMain.savedCallback != NULL)
            LoadQolOptions();
        else
            ApplyPreset(sPresetDefault);
        sLocalQolConfig.pageNum = 1;
        sLocalQolConfig.pageIndex = 0;
        sLocalQolConfig.trueIndex = 0;
        sLocalQolConfig.tooltipActive = FALSE;
        sStoredPageNum = 1;
        DrawPageOptions(sLocalQolConfig.pageNum);
        HighlightOptionMenuItem(0);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        SetVBlankCallback(VblankCB_QolMenu);
        SetMainCallback2(CB2_QolMenu);
        sQolTaskId = CreateTask(Task_QolMenuFadeIn, 0);
        break;
    }
}

static void CB2_QolMenu(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VblankCB_QolMenu(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void Task_QolMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        gTasks[taskId].func = Task_QolMenuProcessInput;
        DrawTooltip(taskId, sTooltip_Explanation);
    }
}

static void HighlightOptionMenuItem(u8 index)
{
    SetGpuReg(REG_OFFSET_WIN0H, WIN_RANGE(16, DISPLAY_WIDTH - 16));
    SetGpuReg(REG_OFFSET_WIN0V, WIN_RANGE(index * 16 + 40, index * 16 + 56));
}

static void SaveQolOptions(void)
{
    gSaveBlock2Ptr->qolConfig.preset         = sLocalQolConfig.optionConfig[QOL_PRESET];
    gSaveBlock2Ptr->qolConfig.holdA          = sLocalQolConfig.optionConfig[QOL_HOLD_A];
    gSaveBlock2Ptr->qolConfig.battleSpeed    = sLocalQolConfig.optionConfig[QOL_BATTLE_SPEED];
    gSaveBlock2Ptr->qolConfig.quickAnims     = sLocalQolConfig.optionConfig[QOL_QUICK_ANIMS];
    gSaveBlock2Ptr->qolConfig.runTrainer     = sLocalQolConfig.optionConfig[QOL_RUN_TRAINER_BATTLE];
    gSaveBlock2Ptr->qolConfig.fanfares       = sLocalQolConfig.optionConfig[QOL_FANFARES];
    gSaveBlock2Ptr->qolConfig.fastHealing    = sLocalQolConfig.optionConfig[QOL_FAST_HEALING];
    gSaveBlock2Ptr->qolConfig.wallyTutorial  = sLocalQolConfig.optionConfig[QOL_WALLY_TUTORIAL];
    gSaveBlock2Ptr->qolConfig.earlyRun       = sLocalQolConfig.optionConfig[QOL_EARLY_RUN];
    gSaveBlock2Ptr->qolConfig.infiniteTms    = sLocalQolConfig.optionConfig[QOL_INFINITE_TMS];
    gSaveBlock2Ptr->qolConfig.modItems       = sLocalQolConfig.optionConfig[QOL_MOD_ITEMS];
    gSaveBlock2Ptr->qolConfig.easyFishing    = sLocalQolConfig.optionConfig[QOL_EASY_FISHING];
    gSaveBlock2Ptr->qolConfig.expMultiplier  = sLocalQolConfig.optionConfig[QOL_EXP_MULTIPLIER];
    gSaveBlock2Ptr->qolConfig.catchRate      = sLocalQolConfig.optionConfig[QOL_CATCH_RATE];
    gSaveBlock2Ptr->qolConfig.shinyRate      = sLocalQolConfig.optionConfig[QOL_SHINY_RATE];
    gSaveBlock2Ptr->qolConfig.perfectIvs     = sLocalQolConfig.optionConfig[QOL_PERFECT_IVS];
    gSaveBlock2Ptr->qolConfig.preferNature   = sLocalQolConfig.optionConfig[QOL_PREFER_NATURE];
    gSaveBlock2Ptr->qolConfig.freeHms        = sLocalQolConfig.optionConfig[QOL_FREE_HMS];
    gSaveBlock2Ptr->qolConfig.alwaysFlash    = sLocalQolConfig.optionConfig[QOL_ALWAYS_FLASH];

    if (gMain.savedCallback != NULL)
    {
        SetDefaultFlashLevel();

        if (VarGet(VAR_LITTLEROOT_TOWN_STATE) < 4)
        {
            if (sLocalQolConfig.optionConfig[QOL_EARLY_RUN] == QOL_EARLY_RUN_OFF)
            {
                FlagClear(FLAG_SYS_B_DASH);
                FlagClear(FLAG_RECEIVED_RUNNING_SHOES);
            }
        }

        if (sLocalQolConfig.optionConfig[QOL_MOD_ITEMS] == QOL_MOD_ITEMS_OFF)
        {
            if (gSaveBlock1Ptr->registeredItem >= ITEM_PORTA_HEAL && gSaveBlock1Ptr->registeredItem <= ITEM_PORTA_PC)
                gSaveBlock1Ptr->registeredItem = ITEM_NONE;
            ClearItemSlots(gSaveBlock1Ptr->bagPocket_Mods, BAG_MODS_COUNT);
        }
        else if (sLocalQolConfig.optionConfig[QOL_MOD_ITEMS] == QOL_MOD_ITEMS_ON && FlagGet(FLAG_SYS_POKEMON_GET))
        {
            if (CheckBagHasItem(ITEM_INF_REPEL, 1) == FALSE)
                AddBagItem(ITEM_INF_REPEL, 1);
            if (CheckBagHasItem(ITEM_PORTA_HEAL, 1) == FALSE)
                AddBagItem(ITEM_PORTA_HEAL, 1);
            if (CheckBagHasItem(ITEM_PORTA_FLY, 1) == FALSE)
                AddBagItem(ITEM_PORTA_FLY, 1);
            if (CheckBagHasItem(ITEM_PORTA_PC, 1) == FALSE)
                AddBagItem(ITEM_PORTA_PC, 1);
            if (CheckBagHasItem(ITEM_EV_EDITOR, 1) == FALSE)
                AddBagItem(ITEM_EV_EDITOR, 1);
            if (CheckBagHasItem(ITEM_ABILITY_CAPSULE, 1) == FALSE)
                AddBagItem(ITEM_ABILITY_CAPSULE, 1);
            if (CheckBagHasItem(ITEM_MOVE_RELEARNER, 1) == FALSE)
                AddBagItem(ITEM_MOVE_RELEARNER, 1);
        }
    }
}

static void Task_QolMenuFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        FreeAllWindowBuffers();
        if (gMain.savedCallback != NULL)
        {
            DestroyTask(taskId);
            SetMainCallback2(gMain.savedCallback);
        }
        else
        {
            SetGpuReg(REG_OFFSET_DISPCNT, 0);
            SetGpuReg(REG_OFFSET_BG2CNT, 0);
            SetGpuReg(REG_OFFSET_BG1CNT, 0);
            SetGpuReg(REG_OFFSET_BG0CNT, 0);
            SetGpuReg(REG_OFFSET_BG2HOFS, 0);
            SetGpuReg(REG_OFFSET_BG2VOFS, 0);
            SetGpuReg(REG_OFFSET_BG1HOFS, 0);
            SetGpuReg(REG_OFFSET_BG1VOFS, 0);
            SetGpuReg(REG_OFFSET_BG0HOFS, 0);
            SetGpuReg(REG_OFFSET_BG0VOFS, 0);
            DmaClearLarge16(3, (void *)(VRAM), VRAM_SIZE, 0x1000);
            DmaClear32(3, OAM, OAM_SIZE);
            DmaClear16(3, PLTT, PLTT_SIZE);
            gPlttBufferUnfaded[0] = 0;
            gPlttBufferFaded[0] = 0;
            ResetBgsAndClearDma3BusyFlags(0);
            InitBgsFromTemplates(0, sMainMenuBgTemplates, 2);
            gTasks[taskId].func = Task_NewGameBirchSpeech_Init;
        }
    }
}

static void Task_QolMenuProcessInput(u8 taskId)
{
    if (gMain.newKeys & A_BUTTON)
    {
        if (sLocalQolConfig.trueIndex == QOL_START_GAME)
        {
            SaveQolOptions();
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
            gTasks[taskId].func = Task_QolMenuFadeOut;
            PlaySE(SE_SELECT);
            return;
        }
        else if (sLocalQolConfig.trueIndex == QOL_PRESET)
        {
            u8 preset = sLocalQolConfig.optionConfig[QOL_PRESET];
            if (preset == QOL_PRESET_DEFAULT)
                ApplyPreset(sPresetDefault);
            else if (preset == QOL_PRESET_VANILLA)
                ApplyPreset(sPresetVanilla);

            DrawPageOptions(sLocalQolConfig.pageNum);
            PlaySE(SE_SELECT);
        }
    }
    else if (gMain.newKeys & B_BUTTON)
    {
        if (sLocalQolConfig.tooltipActive)
        {
            HideTooltip();
            PlaySE(SE_SELECT);
        }
        else if (gMain.savedCallback != NULL)
        {
            SaveQolOptions();
            BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
            gTasks[taskId].func = Task_QolMenuFadeOut;
            PlaySE(SE_SELECT);
            return;
        }
    }
    else if (gMain.newKeys & SELECT_BUTTON)
    {
        if (sLocalQolConfig.tooltipActive)
        {
            HideTooltip();
            PlaySE(SE_SELECT);
        }
        else
        {
            const u8 *str = GetCurrentOptionTooltip();
            if (str != NULL)
            {
                DrawTooltip(taskId, str);
                PlaySE(SE_SELECT);
            }
        }
    }
    else if (gMain.newKeys & (L_BUTTON | R_BUTTON))
    {
        bool8 isR = (gMain.newKeys & R_BUTTON) != 0;

        if (isR)
            sLocalQolConfig.pageNum = (sLocalQolConfig.pageNum % QOL_MAX_PAGES) + 1;
        else
            sLocalQolConfig.pageNum = (sLocalQolConfig.pageNum == 1) ? QOL_MAX_PAGES : sLocalQolConfig.pageNum - 1;

        if (sLocalQolConfig.pageIndex < QOL_OPTIONS_PER_PAGE)
        {
            sLocalQolConfig.trueIndex = (sLocalQolConfig.pageNum - 1) * QOL_OPTIONS_PER_PAGE + sLocalQolConfig.pageIndex;
            if (sLocalQolConfig.trueIndex >= CURRENT_QOL_OPTIONS_NUM)
                sLocalQolConfig.trueIndex = CURRENT_QOL_OPTIONS_NUM - 1;
        }

        DrawPageOptions(sLocalQolConfig.pageNum);
        UpdateTooltipIfActive(taskId);
        PlaySE(SE_SELECT);
    }
    else if (gMain.newKeys & DPAD_UP)
    {
        if (sLocalQolConfig.trueIndex == QOL_PAGE)
            sLocalQolConfig.trueIndex = GetPageOptionTrueIndex(QOL_PAGE_LAST, sLocalQolConfig.pageNum);
        else if (sLocalQolConfig.trueIndex > GetPageOptionTrueIndex(QOL_PAGE_FIRST, sLocalQolConfig.pageNum))
            sLocalQolConfig.trueIndex--;
        else
            sLocalQolConfig.trueIndex = QOL_START_GAME;

        if (sLocalQolConfig.trueIndex == QOL_PAGE)
            sLocalQolConfig.pageIndex = QOL_OPTIONS_PER_PAGE;
        else if (sLocalQolConfig.trueIndex == QOL_START_GAME)
            sLocalQolConfig.pageIndex = QOL_OPTIONS_PER_PAGE + 1;
        else
            sLocalQolConfig.pageIndex = sLocalQolConfig.trueIndex % QOL_OPTIONS_PER_PAGE;

        HighlightOptionMenuItem(sLocalQolConfig.pageIndex);
        UpdateTooltipIfActive(taskId);
        PlaySE(SE_SELECT);
    }
    else if (gMain.newKeys & DPAD_DOWN)
    {
        if (sLocalQolConfig.trueIndex == GetPageOptionTrueIndex(QOL_PAGE_LAST, sLocalQolConfig.pageNum))
            sLocalQolConfig.trueIndex = QOL_PAGE;
        else if (sLocalQolConfig.trueIndex == QOL_START_GAME)
            sLocalQolConfig.trueIndex = GetPageOptionTrueIndex(QOL_PAGE_FIRST, sLocalQolConfig.pageNum);
        else
            sLocalQolConfig.trueIndex++;

        if (sLocalQolConfig.trueIndex == QOL_PAGE)
            sLocalQolConfig.pageIndex = QOL_OPTIONS_PER_PAGE;
        else if (sLocalQolConfig.trueIndex == QOL_START_GAME)
            sLocalQolConfig.pageIndex = QOL_OPTIONS_PER_PAGE + 1;
        else
            sLocalQolConfig.pageIndex = sLocalQolConfig.trueIndex % QOL_OPTIONS_PER_PAGE;

        HighlightOptionMenuItem(sLocalQolConfig.pageIndex);
        UpdateTooltipIfActive(taskId);
        PlaySE(SE_SELECT);
    }
    else if (gMain.newKeys & (DPAD_LEFT | DPAD_RIGHT))
    {
        bool8 isRight = (gMain.newKeys & DPAD_RIGHT) != 0;

        if (sLocalQolConfig.trueIndex < CURRENT_QOL_OPTIONS_NUM)
        {
            u8 numChoices = sQolOptions[sLocalQolConfig.trueIndex].numChoices;
            u8 cur = sLocalQolConfig.optionConfig[sLocalQolConfig.trueIndex];

            if (isRight)
                cur = (cur + 1) % numChoices;
            else
                cur = (cur == 0) ? numChoices - 1 : cur - 1;

            sLocalQolConfig.optionConfig[sLocalQolConfig.trueIndex] = cur;

            if (sLocalQolConfig.trueIndex == QOL_PRESET)
            {
                if (cur == QOL_PRESET_DEFAULT)
                    ApplyPreset(sPresetDefault);
                else if (cur == QOL_PRESET_VANILLA)
                    ApplyPreset(sPresetVanilla);
            }
            else
            {
                sLocalQolConfig.optionConfig[QOL_PRESET] = QOL_PRESET_CUSTOM;
            }

            DrawPageOptions(sLocalQolConfig.pageNum);
            PlaySE(SE_SELECT);
        }
        else if (sLocalQolConfig.trueIndex == QOL_PAGE)
        {
            if (isRight)
                sLocalQolConfig.pageNum = (sLocalQolConfig.pageNum % QOL_MAX_PAGES) + 1;
            else
                sLocalQolConfig.pageNum = (sLocalQolConfig.pageNum == 1) ? QOL_MAX_PAGES : sLocalQolConfig.pageNum - 1;

            DrawPageOptions(sLocalQolConfig.pageNum);
            UpdateTooltipIfActive(taskId);
            PlaySE(SE_SELECT);
        }
    }
}

static void DrawHeaderWindow(void)
{
    s32 width;
    FillWindowPixelBuffer(QOL_WIN_HEADER, PIXEL_FILL(1));
    AddTextPrinterParameterized(QOL_WIN_HEADER, QOL_FONT_ID, sText_Header, 4, 1, TEXT_SKIP_DRAW, NULL);
    width = GetStringWidth(QOL_FONT_ID, sText_Version, GetFontAttribute(QOL_FONT_ID, FONTATTR_LETTER_SPACING));
    AddTextPrinterParameterized(QOL_WIN_HEADER, QOL_FONT_ID, sText_Version, 204 - width, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(QOL_WIN_HEADER, COPYWIN_FULL);
}

#define QOL_CHOICE_LEFT_ARROW_X  108
#define QOL_CHOICE_RIGHT_ARROW_X 200
#define QOL_CHOICE_CENTER_X      ((QOL_CHOICE_LEFT_ARROW_X + QOL_CHOICE_RIGHT_ARROW_X) / 2)

static void DrawPageOptions(u8 page)
{
    u8 i;
    u8 startIdx = (page - 1) * QOL_OPTIONS_PER_PAGE;
    u8 count = QOL_OPTIONS_PER_PAGE;
    s32 pageWidth;
    const u8 *bottomOptionStr;

    FillWindowPixelBuffer(QOL_WIN_OPTIONS, PIXEL_FILL(1));

    for (i = 0; i < count; i++)
    {
        u8 optIdx = startIdx + i;
        if (optIdx < CURRENT_QOL_OPTIONS_NUM)
        {
            u8 choice = sLocalQolConfig.optionConfig[optIdx];
            const u8 *choiceStr = sQolOptions[optIdx].choices[choice];
            s32 choiceWidth = GetStringWidth(QOL_FONT_ID, choiceStr, GetFontAttribute(QOL_FONT_ID, FONTATTR_LETTER_SPACING));

            AddTextPrinterParameterized(QOL_WIN_OPTIONS, QOL_FONT_ID, sQolOptions[optIdx].name, 4, i * 16 + 1, TEXT_SKIP_DRAW, NULL);

            AddTextPrinterParameterized(QOL_WIN_OPTIONS, QOL_FONT_ID, sOption_LeftArrow, QOL_CHOICE_LEFT_ARROW_X, i * 16 + 1, TEXT_SKIP_DRAW, NULL);
            AddTextPrinterParameterized(QOL_WIN_OPTIONS, QOL_FONT_ID, choiceStr, QOL_CHOICE_CENTER_X - (choiceWidth / 2), i * 16 + 1, TEXT_SKIP_DRAW, NULL);
            AddTextPrinterParameterized(QOL_WIN_OPTIONS, QOL_FONT_ID, sOption_RightArrow, QOL_CHOICE_RIGHT_ARROW_X, i * 16 + 1, TEXT_SKIP_DRAW, NULL);
        }
    }

    // Fixed bottom options (PAGE & START GAME)
    pageWidth = GetStringWidth(QOL_FONT_ID, sChoices_Page[page - 1], GetFontAttribute(QOL_FONT_ID, FONTATTR_LETTER_SPACING));
    AddTextPrinterParameterized(QOL_WIN_OPTIONS, QOL_FONT_ID, sOption_Page, 4, QOL_OPTIONS_PER_PAGE * 16 + 1, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(QOL_WIN_OPTIONS, QOL_FONT_ID, sOption_LeftArrow, QOL_CHOICE_LEFT_ARROW_X, QOL_OPTIONS_PER_PAGE * 16 + 1, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(QOL_WIN_OPTIONS, QOL_FONT_ID, sChoices_Page[page - 1], QOL_CHOICE_CENTER_X - (pageWidth / 2), QOL_OPTIONS_PER_PAGE * 16 + 1, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(QOL_WIN_OPTIONS, QOL_FONT_ID, sOption_RightArrow, QOL_CHOICE_RIGHT_ARROW_X, QOL_OPTIONS_PER_PAGE * 16 + 1, TEXT_SKIP_DRAW, NULL);

    bottomOptionStr = (gMain.savedCallback != NULL) ? sOption_SaveExit : sOption_StartGame;
    AddTextPrinterParameterized(QOL_WIN_OPTIONS, QOL_FONT_ID, bottomOptionStr, 4, (QOL_OPTIONS_PER_PAGE + 1) * 16 + 1, TEXT_SKIP_DRAW, NULL);

    CopyWindowToVram(QOL_WIN_OPTIONS, COPYWIN_FULL);
}

static void DrawTooltip(u8 taskId, const u8 *str)
{
    DrawStdWindowFrame(QOL_WIN_TOOLTIP, FALSE);
    FillWindowPixelBuffer(QOL_WIN_TOOLTIP, PIXEL_FILL(1));
    AddTextPrinterParameterized(QOL_WIN_TOOLTIP, QOL_FONT_ID, str, 0, 1, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(QOL_WIN_TOOLTIP, COPYWIN_FULL);
    sLocalQolConfig.tooltipActive = TRUE;
}

static void HideTooltip(void)
{
    ClearStdWindowAndFrameToTransparent(QOL_WIN_TOOLTIP, FALSE);
    ClearWindowTilemap(QOL_WIN_TOOLTIP);
    CopyWindowToVram(QOL_WIN_TOOLTIP, COPYWIN_FULL);
    sLocalQolConfig.tooltipActive = FALSE;
}

static const u8 *GetCurrentOptionTooltip(void)
{
    if (sLocalQolConfig.trueIndex < CURRENT_QOL_OPTIONS_NUM)
        return sQolOptions[sLocalQolConfig.trueIndex].tooltip;
    else if (sLocalQolConfig.trueIndex == QOL_PAGE)
        return sTooltip_Page;
    else if (sLocalQolConfig.trueIndex == QOL_START_GAME)
        return (gMain.savedCallback != NULL) ? sTooltip_SaveExit : sTooltip_StartGame;
    return sTooltip_Explanation;
}

static void UpdateTooltipIfActive(u8 taskId)
{
    if (sLocalQolConfig.tooltipActive)
    {
        const u8 *str = GetCurrentOptionTooltip();
        if (str != NULL)
            DrawTooltip(taskId, str);
        else
            HideTooltip();
    }
}

static void LoadQolOptions(void)
{
    sLocalQolConfig.optionConfig[QOL_PRESET]             = gSaveBlock2Ptr->qolConfig.preset;
    sLocalQolConfig.optionConfig[QOL_HOLD_A]             = gSaveBlock2Ptr->qolConfig.holdA;
    sLocalQolConfig.optionConfig[QOL_BATTLE_SPEED]       = gSaveBlock2Ptr->qolConfig.battleSpeed;
    sLocalQolConfig.optionConfig[QOL_QUICK_ANIMS]        = gSaveBlock2Ptr->qolConfig.quickAnims;
    sLocalQolConfig.optionConfig[QOL_RUN_TRAINER_BATTLE] = gSaveBlock2Ptr->qolConfig.runTrainer;
    sLocalQolConfig.optionConfig[QOL_FANFARES]           = gSaveBlock2Ptr->qolConfig.fanfares;
    sLocalQolConfig.optionConfig[QOL_FAST_HEALING]       = gSaveBlock2Ptr->qolConfig.fastHealing;
    sLocalQolConfig.optionConfig[QOL_WALLY_TUTORIAL]     = gSaveBlock2Ptr->qolConfig.wallyTutorial;
    sLocalQolConfig.optionConfig[QOL_EARLY_RUN]          = gSaveBlock2Ptr->qolConfig.earlyRun;
    sLocalQolConfig.optionConfig[QOL_INFINITE_TMS]       = gSaveBlock2Ptr->qolConfig.infiniteTms;
    sLocalQolConfig.optionConfig[QOL_MOD_ITEMS]          = gSaveBlock2Ptr->qolConfig.modItems;
    sLocalQolConfig.optionConfig[QOL_EASY_FISHING]       = gSaveBlock2Ptr->qolConfig.easyFishing;
    sLocalQolConfig.optionConfig[QOL_EXP_MULTIPLIER]     = gSaveBlock2Ptr->qolConfig.expMultiplier;
    sLocalQolConfig.optionConfig[QOL_CATCH_RATE]         = gSaveBlock2Ptr->qolConfig.catchRate;
    sLocalQolConfig.optionConfig[QOL_SHINY_RATE]         = gSaveBlock2Ptr->qolConfig.shinyRate;
    sLocalQolConfig.optionConfig[QOL_PERFECT_IVS]        = gSaveBlock2Ptr->qolConfig.perfectIvs;
    sLocalQolConfig.optionConfig[QOL_PREFER_NATURE]      = gSaveBlock2Ptr->qolConfig.preferNature;
    sLocalQolConfig.optionConfig[QOL_FREE_HMS]           = gSaveBlock2Ptr->qolConfig.freeHms;
    sLocalQolConfig.optionConfig[QOL_ALWAYS_FLASH]       = gSaveBlock2Ptr->qolConfig.alwaysFlash;
}

bool8 CheckQolOption(u8 option, u8 selection)
{
    if (gSaveBlock2Ptr == NULL)
        return FALSE;

    switch (option)
    {
    case QOL_PRESET:
        return gSaveBlock2Ptr->qolConfig.preset == selection;
    case QOL_HOLD_A:
        return gSaveBlock2Ptr->qolConfig.holdA == selection;
    case QOL_BATTLE_SPEED:
        return gSaveBlock2Ptr->qolConfig.battleSpeed == selection;
    case QOL_QUICK_ANIMS:
        return gSaveBlock2Ptr->qolConfig.quickAnims == selection;
    case QOL_RUN_TRAINER_BATTLE:
        return gSaveBlock2Ptr->qolConfig.runTrainer == selection;
    case QOL_FANFARES:
        return gSaveBlock2Ptr->qolConfig.fanfares == selection;
    case QOL_FAST_HEALING:
        return gSaveBlock2Ptr->qolConfig.fastHealing == selection;
    case QOL_WALLY_TUTORIAL:
        return gSaveBlock2Ptr->qolConfig.wallyTutorial == selection;
    case QOL_EARLY_RUN:
        return gSaveBlock2Ptr->qolConfig.earlyRun == selection;
    case QOL_INFINITE_TMS:
        return gSaveBlock2Ptr->qolConfig.infiniteTms == selection;
    case QOL_MOD_ITEMS:
        return gSaveBlock2Ptr->qolConfig.modItems == selection;
    case QOL_EASY_FISHING:
        return gSaveBlock2Ptr->qolConfig.easyFishing == selection;
    case QOL_EXP_MULTIPLIER:
        return gSaveBlock2Ptr->qolConfig.expMultiplier == selection;
    case QOL_CATCH_RATE:
        return gSaveBlock2Ptr->qolConfig.catchRate == selection;
    case QOL_SHINY_RATE:
        return gSaveBlock2Ptr->qolConfig.shinyRate == selection;
    case QOL_PERFECT_IVS:
        return gSaveBlock2Ptr->qolConfig.perfectIvs == selection;
    case QOL_PREFER_NATURE:
        return gSaveBlock2Ptr->qolConfig.preferNature == selection;
    case QOL_FREE_HMS:
        return gSaveBlock2Ptr->qolConfig.freeHms == selection;
    case QOL_ALWAYS_FLASH:
        return gSaveBlock2Ptr->qolConfig.alwaysFlash == selection;
    default:
        return FALSE;
    }
}

u8 GetQolOption(u8 option)
{
    if (gSaveBlock2Ptr == NULL)
        return 0;

    switch (option)
    {
    case QOL_PRESET:
        return gSaveBlock2Ptr->qolConfig.preset;
    case QOL_HOLD_A:
        return gSaveBlock2Ptr->qolConfig.holdA;
    case QOL_BATTLE_SPEED:
        return gSaveBlock2Ptr->qolConfig.battleSpeed;
    case QOL_QUICK_ANIMS:
        return gSaveBlock2Ptr->qolConfig.quickAnims;
    case QOL_RUN_TRAINER_BATTLE:
        return gSaveBlock2Ptr->qolConfig.runTrainer;
    case QOL_FANFARES:
        return gSaveBlock2Ptr->qolConfig.fanfares;
    case QOL_FAST_HEALING:
        return gSaveBlock2Ptr->qolConfig.fastHealing;
    case QOL_WALLY_TUTORIAL:
        return gSaveBlock2Ptr->qolConfig.wallyTutorial;
    case QOL_EARLY_RUN:
        return gSaveBlock2Ptr->qolConfig.earlyRun;
    case QOL_INFINITE_TMS:
        return gSaveBlock2Ptr->qolConfig.infiniteTms;
    case QOL_MOD_ITEMS:
        return gSaveBlock2Ptr->qolConfig.modItems;
    case QOL_EASY_FISHING:
        return gSaveBlock2Ptr->qolConfig.easyFishing;
    case QOL_EXP_MULTIPLIER:
        return gSaveBlock2Ptr->qolConfig.expMultiplier;
    case QOL_CATCH_RATE:
        return gSaveBlock2Ptr->qolConfig.catchRate;
    case QOL_SHINY_RATE:
        return gSaveBlock2Ptr->qolConfig.shinyRate;
    case QOL_PERFECT_IVS:
        return gSaveBlock2Ptr->qolConfig.perfectIvs;
    case QOL_PREFER_NATURE:
        return gSaveBlock2Ptr->qolConfig.preferNature;
    case QOL_FREE_HMS:
        return gSaveBlock2Ptr->qolConfig.freeHms;
    case QOL_ALWAYS_FLASH:
        return gSaveBlock2Ptr->qolConfig.alwaysFlash;
    default:
        return 0;
    }
}
