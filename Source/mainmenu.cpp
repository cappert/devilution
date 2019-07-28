#include "diablo.h"
#include "storm/storm.h"

#include "ui/common.h"
#include "ui/diabloui.h"

char gszHero[16];

/* data */

int menu_music_track_id = TMUSIC_INTRO;

void mainmenu_refresh_music()
{
	music_start(menu_music_track_id);
	do {
		menu_music_track_id++;
		if (menu_music_track_id == NUM_MUSIC)
			menu_music_track_id = 0;
	} while (!menu_music_track_id || menu_music_track_id == 1);
}

void  mainmenu_change_name(int arg1, int arg2, int arg3, int arg4, char *name_1, char *name_2)
{
	if (UiValidPlayerName(name_2))
		pfile_rename_hero(name_1, name_2);
}

int  mainmenu_select_hero_dialog(
    const _SNETPROGRAMDATA *client_info,
    const _SNETPLAYERDATA *user_info,
    const _SNETUIDATA *ui_info,
    const _SNETVERSIONDATA *fileinfo,
    DWORD mode,
    char *cname, DWORD clen,
    char *cdesc, DWORD cdlen,
    BOOL *multi)
{
	BOOL hero_is_created = TRUE;
	int dlgresult = 0;
	if (gbMaxPlayers == 1) {
		if (!UiSelHeroSingDialog(
		        pfile_ui_set_hero_infos,
		        pfile_ui_save_create,
		        pfile_delete_save,
		        pfile_ui_set_class_stats,
		        &dlgresult,
		        gszHero,
		        &gnDifficulty))
			app_fatal("Unable to display SelHeroSing");

		if (dlgresult == SELHERO_CONTINUE)
			gbLoadGame = TRUE;
		else
			gbLoadGame = FALSE;

	} else if (!UiSelHeroMultDialog(
	               pfile_ui_set_hero_infos,
	               pfile_ui_save_create,
	               pfile_delete_save,
	               pfile_ui_set_class_stats,
	               &dlgresult,
	               &hero_is_created,
	               gszHero)) {
		app_fatal("Can't load multiplayer dialog");
	}
	if (dlgresult == SELHERO_PREVIOUS) {
		SErrSetLastError(1223);
		return 0;
	}

	pfile_create_player_description(cdesc, cdlen);
	if (multi) {
		if (mode == 'BNET')
			*multi = hero_is_created || !plr[myplr].pBattleNet;
		else
			*multi = hero_is_created;
	}
	if (cname && clen)
		SStrCopy(cname, gszHero, clen);

	return 1;
}

BOOL mainmenu_single_player()
{
	gbMaxPlayers = 1;
	return mainmenu_init_menu(SELHERO_NEW_DUNGEON);
}

BOOL mainmenu_init_menu(int type)
{
	BOOL success;

	if (type == SELHERO_PREVIOUS)
		return TRUE;

	music_stop();

	success = StartGame(type != SELHERO_CONTINUE, type != SELHERO_CONNECT);
	if (success)
		mainmenu_refresh_music();

	return success;
}

BOOL mainmenu_multi_player()
{
	gbMaxPlayers = MAX_PLRS;
	return mainmenu_init_menu(SELHERO_CONNECT);
}

void mainmenu_play_intro()
{
	music_stop();
	play_movie("gendata\\diablo1.smk", 1);
	mainmenu_refresh_music();
}
