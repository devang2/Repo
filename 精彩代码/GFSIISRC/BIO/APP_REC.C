#include <stdio.h>
#include <io.h>

#include <def.inc>

UC append_record(UC *filename, UC *buffer, UI length, UI number)
{
        FILE   *rec_file;
        UI	rec_head_p = 0, rec_end_p = 0;

	if ( access(filename, 0) != 0 )
	{
		rec_file = fopen(filename, "wb");
		if ( !rec_file )
			return 1;
		fwrite(&rec_head_p, sizeof(UI), 1, rec_file);
		fwrite(&rec_end_p, sizeof(UI), 1, rec_file);
		fwrite(buffer, length, number, rec_file);
	}
	else
	{
		rec_file = fopen(filename, "rb+");
		if ( !rec_file )
			return 2;
		fread(&rec_head_p, sizeof(UI), 1, rec_file);
		fread(&rec_end_p, sizeof(UI), 1, rec_file);
		if ( (rec_head_p>number-1) || (rec_end_p>number-1) )
			return 3;
		rec_end_p++;
		if ( rec_end_p == number)
			rec_end_p = 0;
		if ( rec_head_p == rec_end_p )
			rec_head_p++;
		if ( rec_head_p == number)
			rec_head_p = 0;

		fseek(rec_file, 0, SEEK_SET);
		fwrite(&rec_head_p, sizeof(UI), 1, rec_file);
		fwrite(&rec_end_p, sizeof(UI), 1, rec_file);
		fseek(rec_file, rec_end_p*length, SEEK_CUR);
		fwrite(buffer, length, 1, rec_file);
	}
	fclose(rec_file);
	return 0;
}

