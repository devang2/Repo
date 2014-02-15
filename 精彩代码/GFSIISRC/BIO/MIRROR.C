#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <string.h>

#include <def.inc>
#include <func.inc>

static UC File1[64];				/* the 1st filespec */
static UC File2[64];				/* the 2nd filespec */
static UC Tmpfile[64];				/* the tempfile's filespec */
static UC Mirror_setted = FALSE;    /* TRUE if the two mirror files are defined */

/* ---------------------------------------------------------------------
	FUNCTION:	Check weither or not the two mirror files are lawful !
	CALLS:
	CALLED BY:
	INPUT:      None.
	OUTPUT:		None.
	RETURN:     0 -- Two files checked OK!
				1 -- Two files are both NOT exist.
				2 -- Two files are different size.
				3 -- Error occurred when accessing the two mirror files.
					 Including: copy failed, delete the <Tempfile> failed.
				4 -- File1 exist & File2 not.
				5 -- File2 exist & File1 not.
   --------------------------------------------------------------------- */
static UC chkfile(void)
{
UI sf1, sf2, hd1, hd2;

	sf1 = access(File1, 0);
	sf2 = access(File2, 0);

	if ( sf1 && sf2 )
		return 1;					/* The two files are all not exist.*/

	if ( (!sf1) && (!sf2) )
	{
		hd1 = open(File1, O_WRONLY|O_BINARY, S_IREAD|S_IWRITE);
		hd2 = open(File2, O_WRONLY|O_BINARY, S_IREAD|S_IWRITE);
		if ( filelength(hd1)!=filelength(hd2) )
			return 2;				/* two files has different sizes */
        close(hd1);
        close(hd2);
		if ( access(Tmpfile,0)==0 )
			if ( unlink(Tmpfile)!=0 )
				return 3;			/* delete file error */
		return 0;
	}

	if ( (!sf1)&&(sf2) )
	{
		if ( fcopy(File1, Tmpfile)==1 )
			return 3; 				/* copy error */
		return 4;	 				/* File1 exist, and File2 was rename to Tmpfile */
	}

	if((sf1)&&(!sf2))
	{
		if ( fcopy(File2,Tmpfile)==1 )
			return 3; 				/* copy error */
		return 5; 					/* File2 exist ! */
	}
}


/* ---------------------------------------------------------------------
	FUNCTION:	Check weither or not the two mirror files are lawful !
	CALLS:
	CALLED BY:
	INPUT:      None.
	OUTPUT:		None.
	RETURN:     0 -- Two files checked OK!
				1 -- Two files are both NOT exist.
				2 -- Two files are different size.
				3 -- Error occurred when accessing the two mirror files.
					 Including: copy failed, delete the <Tempfile> failed,
								rename the Tempfile to File1(2) failed.
   --------------------------------------------------------------------- */
UC check_mirror(void)
{
UC result;

	if (!Mirror_setted)
		exit_scr(0, "MIRROR/Chkfile(): The two mirror files NOT defined.");

	result = chkfile();
    switch(result)
	{
	case 4:
		if ( rename(Tmpfile, File2)!=0 )
			return 3; 			/* rename error */
		else
			return 0;
	case 5:
		if ( rename(Tmpfile, File1)!=0)
			return 3; 			/* rename error */
		else
			return 0;
	}
	return result;
}
/* --------------------------------------------------------------------
	FUNCTION:	Init the two mirror files.
				If the two files already exist, just delete them.
				Files were create in READWRITE attribe.
	CALLS:      Turbo C librarys.
	CALLED BY:  All-purpose routine.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		If create any of the two mirror files failed,
				return 1, else return 0.
   ----------------------------------------------------------------- */
UC init_mirror(void)
{
UI handle;

	if (!Mirror_setted)
		exit_scr(0, "MIRROR/init_mirror(): Mirror files NOT defined.");

	handle = creat( File1, S_IREAD|S_IWRITE );
	if ( handle==0xFFFF )
		return 1;
	close(handle);

	handle = creat( File2, S_IREAD|S_IWRITE );
	if ( handle==0xFFFF )
		return 1;
	close(handle);

    return 0;
}

/* ----------------------------------------------------------------------
	FUNCTION: 	Write date to a file using the stream mode (fopen, "r+b").
	CALLED BY:  This is a general-purposed routine.
	INPUT:		file	the filespec of the file.
				buf		the buffer of data to be written.
				offset	the offset of the file from beginning.
				size	the size of the data type to be written (Unit: byte).
				nitems	the items of such data type to be written.
	OUTPUT:		None.
	RETURN:     0 -- file written OK!
				1 -- file written failed.
					 including: fopen error, fseek error, fwrite error.
   ---------------------------------------------------------------------- */
UC stream_write(UC *file, UL offset, void *buf, UI size, UI nitems)
{
FILE *tmpfp;
UL fpos;

	tmpfp = fopen(file, "r+b");
	if (tmpfp==NULL)
		return 1; 				/* open file error */

	if ( fseek(tmpfp, offset, SEEK_SET)!=0 )
	{
		fclose(tmpfp);
		return 1;       		/* fseek error */
	}

	if ( fwrite(buf, size, nitems, tmpfp)!=nitems )
	{
		fclose(tmpfp);
		return 1;				/* fwrite error */
	}

	fclose(tmpfp);
	return 0;
}

/* ----------------------------------------------------------------------
	FUNCTION: 	Write date to a file using the handle mode (open, O_WRONLY|O_BINARY)
	CALLED BY:  This is a general-purposed routine.
	INPUT:		file	the filespec of the file.
				buf		the buffer of data to be written.
				offset	the offset of the file from beginning.
				nbytes	the number of bytes to be written (Unit: byte).
	OUTPUT:		None.
	RETURN:     0 -- file written OK!
				1 -- file written failed.
					 including: fopen error, fseek error, fwrite error, rename error.
   ---------------------------------------------------------------------- */
UC handle_write(UC *file, UL offset, void *buf, UI nbytes)
{
UI handle;

	handle = open(file, O_WRONLY|O_BINARY);
	if (handle==0xFFFF)
		return 1;							/* open file error */
	if ( lseek(handle, offset, SEEK_SET)==-1 )
	{
		close(handle);
		return 1;
	}										/* seek error */
	if ( write(handle, buf, nbytes)==-1 )
	{
		close(handle);
		return 1;
	}										/* write error */

	close(handle);
    return 0;
}

/* ---------------------------------------------------------------------
	FUNCTION:	Write the mirror files: File1 and File2, stream mode(fopen)
	CALLED BY:	This is a general-purposed routine.
	INPUT:		buf:	the buffer of the data to be written.
				offset:	the offset from the beginning of the mirror files.
				size:	the size of the data type to be written (Unit: byte).
				nitems: the items of such data type to be written.
	OUTPUT:		None.
	RETURN:     0 -- Two files checked OK!
				1 -- Two files are both NOT exist.
				2 -- Two files are different size.
				3 -- Error occurred when accessing the two mirror files.
					 Including: copy failed, delete the <Tempfile> failed,
								rename the Tempfile to File1(2) failed.
   -----------------------------------------------------------------------*/
UC mirror_fwrite(UL offset, void *buf, UI size, UI nitems)
{
UC result;

	if (!Mirror_setted)
		exit_scr(0, "MIRROR/mirror_fwrite(): Mirror files NOT defined.");

	result = chkfile();
	switch (result)
    {
	case 0:		/* File1, File2 all exist. */
		if ( rename(File1, Tmpfile)!=0 )
			return 3;
		if ( stream_write(Tmpfile, offset, buf, size, nitems)!=0 )
			return 3;
		if ( rename(Tmpfile ,File1)!=0 )
			return 3;
		if ( rename(File2, Tmpfile)!=0 )
			return 3;
		if ( stream_write(Tmpfile, offset, buf, size, nitems)!=0 )
			return 3;
		if ( rename(Tmpfile ,File2)!=0 )
			return 3;
		break;

	case 4:                     /* File1 exist, File2 in Tempfile mode */
		if ( stream_write(Tmpfile, offset, buf, size, nitems)!=0 )
			return 3;
		if ( rename(Tmpfile ,File2)!=0 )
			return 3;
		if ( rename(File1, Tmpfile)!=0 )
			return 3;
		if ( stream_write(Tmpfile, offset, buf, size, nitems)!=0 )
			return 3;
		if ( rename(Tmpfile ,File1)!=0 )
			return 3;
		break;

	case 5:                     /* File2 exist, File1 in Tempfile mode */
		if ( stream_write(Tmpfile, offset, buf, size, nitems)!=0 )
			return 3;
		if ( rename(Tmpfile ,File1)!=0 )
			return 3;
		if ( rename(File2, Tmpfile)!=0 )
			return 3;
		if ( stream_write(Tmpfile, offset, buf, size, nitems)!=0 )
			return 3;
		if ( rename(Tmpfile ,File2)!=0 )
			return 3;
		break;

	default:
		return result;
    }
    return 0;
}

/* ---------------------------------------------------------------------
	FUNCTION:	Write the mirror files: File1 and File2, handle mode(open)
	CALLED BY:	This is a general-purposed routine.
	INPUT:		buf:	the buffer of the data to be written.
				offset:	the offset from the beginning of the mirror files.
				nbytes:	the number of bytes to be written (Unit: byte).
	OUTPUT:		None.
	RETURN:     0 -- Two files checked OK!
				1 -- Two files are both NOT exist.
				2 -- Two files are different size.
				3 -- Error occurred when accessing the two mirror files.
					 Including: copy failed, delete the <Tempfile> failed,
								rename the Tempfile to File1(2) failed.
   -----------------------------------------------------------------------*/
UC mirror_write(UL offset, void *buf, UI nbytes)
{
UC result;

	result = chkfile();
	switch (result)
	{
	case 0:			/* File1,File2 all exist! */
		if ( rename(File1, Tmpfile)!=0 )
			return 3;
		if ( handle_write(Tmpfile, offset, buf, nbytes )!=0 )
			return 3;
		if ( rename(Tmpfile, File1)!=0 )
			return 3;
		if ( rename(File2, Tmpfile) )
			return 3;
		if ( handle_write(Tmpfile, offset, buf, nbytes )!=0 )
			return 3;
		if ( rename(Tmpfile, File2)!=0 )
			return 3;
		break;

	case 4:			/* File1 exist, and File2 in Tempfile mode */
		if ( handle_write(Tmpfile, offset, buf, nbytes )!=0 )
			return 3;
		if ( rename(Tmpfile, File2)!=0 )
			return 3;
		if ( rename(File1, Tmpfile) )
			return 3;
		if ( handle_write(Tmpfile, offset, buf, nbytes )!=0 )
			return 3;
		if ( rename(Tmpfile, File1)!=0 )
			return 3;
		break;

	case 5:			/* File2 exist, and File1 in Tempfile mode */
		if ( handle_write(Tmpfile, offset, buf, nbytes )!=0 )
			return 3;
		if ( rename(Tmpfile, File1)!=0 )
			return 3;
		if ( rename(File2, Tmpfile) )
			return 3;
		if ( handle_write(Tmpfile, offset, buf, nbytes )!=0 )
			return 3;
		if ( rename(Tmpfile, File2)!=0 )
			return 3;
		break;

	default:
		return result;
	}

	return 0;
}

/* --------------------------------------------------------------------
	FUNCTION:	Specify the two mirror files.
	CALLED BY:	This is a general-purposed routine.
	INPUT:		file1	the filespec of file 1st.
				file2	the filespec of file 2nd.
				tmpfile	the filespec of temp file.
	OUTPUT:		None.
	RETURN:		None.
   -------------------------------------------------------------------- */
void set_mirror(UC *file1, UC *file2, UC *tmpfile)
{
	strcpy(File1, file1);
	strcpy(File2, file2);
	strcpy(Tmpfile, tmpfile);
	Mirror_setted = TRUE;
}

/* --------------------------------------------------------------------
	FUNCTION:	Reset the flag of set_mirror().
	CALLED BY:	This is a general-purposed routine.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   -------------------------------------------------------------------- */
void reset_mirror(void)
{
	Mirror_setted = FALSE;
}
