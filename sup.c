/* pancake <nopcode.org> -- Copyleft 2009-2011 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HELP "sup [-hlv] [cmd ..]"
#define VERSION "sup 0.1 pancake <nopcode.org> copyleft 2011"

struct rule_t {
	int uid;
	int gid;
	const char *cmd;
	const char *path;
};

#include "config.h"

static int die(int ret, const char *org, const char *str) {
	fprintf (stderr, "%s%s%s\n", org?org:"", org?": ":"", str);
	return ret;
}

int main(int argc, char **argv) {
	const char *cmd;
	int i, uid, gid, ret;

	if (argc < 2 || !strcmp (argv[1], "-h"))
		return die (1, NULL, HELP);

	if (!strcmp (argv[1], "-v"))
		return die (1, NULL, VERSION);

	if (!strcmp (argv[1], "-l")) {
		for (i = 0; rules[i].cmd != NULL; i++)
			printf ("%d %d %10s %s\n", rules[i].uid, rules[i].gid,
				rules[i].cmd, rules[i].path);
		return 0;
	}

	uid = getuid ();
	gid = getgid ();

	for (i = 0; rules[i].cmd != NULL; i++) {
		if (uid != SETUID && rules[i].uid != -1 && rules[i].uid != uid)
			continue;
		if (gid != SETGID && rules[i].gid != -1 && rules[i].gid != gid)
			continue;
		if (*rules[i].cmd=='*' || !strcmp (argv[1], rules[i].cmd)) {
			cmd = (*rules[i].path=='*') ? argv[1] : rules[i].path;
			if (setuid (SETUID) || setgid (SETGID) ||
				seteuid (SETUID) || setegid (SETGID))
				return die (1, "set[e][ug]id", strerror (errno));
			ret = execvp (cmd, argv+1);
			return die (ret, "execv", strerror (errno));
		}
	}

	return die (1, NULL, "Sorry");
}
