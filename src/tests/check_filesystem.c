/*
 * Copyright(c) 1997-2001 Id Software, Inc.
 * Copyright(c) 2002 The Quakeforge Project.
 * Copyright(c) 2006 Quake2World.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "tests.h"
#include "filesystem.h"

static char *argv0;

/*
 * @brief Setup fixture.
 */
void setup(void) {

	Z_Init();

	Fs_Init(argv0);
}

/*
 * @brief Teardown fixture.
 */
void teardown(void) {

	Fs_Shutdown();

	Z_Shutdown();
}

START_TEST(check_filesystem_Fs_OpenRead)
	{
		const char *filenames[] = { "quake2world.cfg", "maps/torn.bsp", NULL };

		const char **filename = filenames;
		while (*filename) {
			ck_assert_msg(Fs_Exists(*filename), "%s does not exist", *filename);

			file_t *f = Fs_OpenRead(*filename);

			ck_assert_msg(f != NULL, "Failed to open %s", *filename);
			ck_assert_msg(Fs_Close(f), "Failed to close %s", *filename);

			filename++;
		}
	}END_TEST

START_TEST(check_filesystem_Fs_OpenWrite)
	{
		file_t *f = Fs_OpenWrite(__FUNCTION__);

		ck_assert_msg(f != NULL, "Failed to open %s", __FUNCTION__);

		const char *testing = "testing";
		int64_t len = Fs_Write(f, (void *) testing, strlen(testing), 1);

		ck_assert_msg(len = strlen(testing), "Failed to write %s", __FUNCTION__);
		ck_assert_msg(Fs_Close(f), "Failed to close %s", __FUNCTION__);

	}END_TEST

START_TEST(check_filesystem_Fs_LoadFile)
	{
		void *buffer;
		int64_t len = Fs_Load("quake2world.cfg", &buffer);

		ck_assert_msg(len > 0, "Failed to load quake2world.cfg");

		const char *prefix = "// generated by Quake2World, do not modify\n";
		ck_assert(g_str_has_prefix((const char *) buffer, prefix));

		Fs_Free(buffer);

	}END_TEST

/*
 * @brief Test entry point.
 */
int32_t main(int32_t argc, char **argv) {

	Test_Init(argc, argv);

	TCase *tcase = tcase_create("check_filesystem");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, check_filesystem_Fs_OpenRead);
	tcase_add_test(tcase, check_filesystem_Fs_OpenWrite);
	tcase_add_test(tcase, check_filesystem_Fs_LoadFile);

	Suite *suite = suite_create("check_filesystem");
	suite_add_tcase(suite, tcase);

	int32_t failed = Test_Run(suite);

	Test_Shutdown();
	return failed;
}
