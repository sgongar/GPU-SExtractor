/*
 				main.c

 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 *	Part of:	SExtractor
 *
 *	Author:		E.BERTIN (IAP)
 *
 *	Contents:	Command-line parsing.
 *
 *	Last modify:	12/11/2008
 *
 *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 */

#ifdef HAVE_CONFIG_H
#include        "config.h"
#endif

#include	<ctype.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"define.h"
#include	"globals.h"
#include	"prefs.h"
#include 	"pattern.h"
#define		SYNTAX \
		EXECUTABLE " <image> [<image2>][-c <configuration_file>][-<keyword> <value>]\n" \
		"> to dump a default configuration file:          " EXECUTABLE " -d \n" \
		"> to dump a default extended configuration file: " EXECUTABLE " -dd \n" \
		"> to dump a full list of measurement parameters: " EXECUTABLE " -dp \n"

extern const char   notokstr[];
extern keystruct	objkey[];

/********************************** main ************************************/
int	main(int argc, char *argv[])

{
	int		a, narg, nim, opt, opt2;
	char		**argkey, **argval, *str;

	if (argc<2)
	{
		fprintf(OUTPUT, "\n         %s  version %s (%s)\n", BANNER,MYVERSION,DATE);
		fprintf(OUTPUT, "\nby %s\n", COPYRIGHT);
		fprintf(OUTPUT, "visit %s\n", WEBSITE);
		error(EXIT_SUCCESS, "SYNTAX: ", SYNTAX);
	}
	QMALLOC(argkey, char *, argc);
	QMALLOC(argval, char *, argc);

	/*default parameters */
	prefs.command_line = argv;
	prefs.ncommand_line = argc;
	prefs.pipe_flag = 0;
	prefs.nimage_name = 1;
	prefs.image_name[0] = "image";
	strcpy(prefs.prefs_name, "default.sex");
	narg = nim = 0;

	for (a=1; a<argc; a++)
	{
		if (*(argv[a]) == '-')
		{
			opt = (int)argv[a][1];
			if (strlen(argv[a])<4 || opt == '-')
			{
				opt2 = (int)tolower((int)argv[a][2]);
				if (opt == '-')
				{
					opt = opt2;
					opt2 = (int)tolower((int)argv[a][3]);
				}
				switch(opt)
				{
				case 'c':
					if (a<(argc-1))
						strcpy(prefs.prefs_name, argv[++a]);
					break;
				case 'd':
					if (opt2=='d')
						dumpprefs(1);
					else if (opt2=='p')
						dumpparams();
					else
						dumpprefs(0);
					exit(EXIT_SUCCESS);
					break;
				case 'v':
					printf("%s version %s (%s)\n", BANNER,MYVERSION,DATE);
					exit(0);
					break;
				case 'h':
				default:
					error(EXIT_SUCCESS,"SYNTAX: ", SYNTAX);
				}
			}
			else
			{
				/*------ Config parameters */
				argkey[narg] = &argv[a][1];
				argval[narg++] = argv[++a];
			}
		}
		else
		{
			/*---- The input image filename(s) */
			for(; (a<argc) && (*argv[a]!='-'); a++)
				for (str=NULL;(str=strtok(str?NULL:argv[a], notokstr)); nim++)
					if (nim<MAXIMAGE)
						prefs.image_name[nim] = str;
					else
						error(EXIT_FAILURE, "*Error*: Too many input images: ", str);
			prefs.nimage_name = nim;
			a--;
		}
	}

	readprefs(prefs.prefs_name, argkey, argval, narg);
	preprefs();

	free(argkey);
	free(argval);

	makeit();

	endprefs();
	NFPRINTF(OUTPUT, "");
	//NPRINTF(OUTPUT, "> All done (in %.0f s)\n", prefs.time_diff);

	return EXIT_SUCCESS;
}

