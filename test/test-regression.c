/**
 * test-regression.c
 *
 * Copyright (c) 2012
 *      libchewing Core Team. See ChangeLog for details.
 *
 * See the file "COPYING" for information on usage and redistribution
 * of this file.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "chewing.h"
#include "plat_types.h"
#include "hash-private.h"
#include "testhelper.h"

void test_libchewing_googlecode_issue_472()
{
	/* FIXME: Add const cause gcc warning */
	static char *INPUT[] = {
		"<T>|&Wt<H>mrJY)G<C2>OqJ<H><H>Yl<R>p0<EE>QE[^<C1>k",
		"+F<C9>hQ$UIICMr!X8/9<C3>(N<T>yU2!-LUI<D>`CS<D>jShm9SF}<EN>[`QYu<C8>k",
		"hk4`2<D>jk",
	};
	int i;
	ChewingContext *ctx;

	ctx = chewing_new();
	chewing_set_maxChiSymbolLen( ctx, 16 );
	chewing_set_autoShiftCur( ctx, 1 );

	for (i = 0; i < ARRAY_SIZE(INPUT); ++i) {
		chewing_Reset( ctx );
		type_keystoke_by_string( ctx, INPUT[i] );
	}

	chewing_delete( ctx );
}

void test_libchewing_issue_30()
{
	ChewingContext *ctx;
	int cursor;

	remove( TEST_HASH_DIR PLAT_SEPARATOR HASH_FILE );

	chewing_Init( NULL, NULL );

	ctx = chewing_new();
	chewing_set_maxChiSymbolLen( ctx, 16 );
	chewing_set_autoShiftCur( ctx, 1 );
	chewing_set_spaceAsSelection( ctx, 1 );
	chewing_set_phraseChoiceRearward( ctx, 1 );

	type_keystoke_by_string( ctx, "hk4g4<H> 3 1" );
	cursor = chewing_cursor_Current( ctx );
	ok( cursor == 2, "cursor position `%d' shall be `2'", cursor);

	chewing_delete( ctx );
	chewing_Terminate();
}

void test_libchewing_data_issue_1()
{
	const TestData DATA = { "e03y.3", "\xE8\xB6\x95\xE8\xB5\xB0" /* 趕走*/ };
	ChewingContext *ctx;

	remove( TEST_HASH_DIR PLAT_SEPARATOR HASH_FILE );

	chewing_Init( NULL, NULL );

	ctx = chewing_new();
	chewing_set_maxChiSymbolLen( ctx, 16 );
	type_keystoke_by_string( ctx, DATA.token );
	ok_preedit_buffer( ctx, DATA.expected );

	chewing_delete( ctx );
	chewing_Terminate();
}

int main()
{
	putenv( "CHEWING_PATH=" CHEWING_DATA_PREFIX );
	putenv( "CHEWING_USER_PATH=" TEST_HASH_DIR );

	test_libchewing_data_issue_1();
	test_libchewing_issue_30();
	test_libchewing_googlecode_issue_472();

	return exit_status();
}
