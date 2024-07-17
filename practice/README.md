# My Cutie Dummy Notes cause i forget a lot

- Shell command handlers should be put in an array terminated with a command handlers with all NULL entries.

- RIOT's python terminal doesn't seem to work well with printf(), so instead use asprintf to make a 
	char* then puts()

- "ip tuntap add tap0 mode tap" to add a tunnel in general, can give it a user by adding "user $(name)"
