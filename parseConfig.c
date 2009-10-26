/*
 * File descriptor.
 */
fdin = fopen (argv[1], "r");


/*
 * Stores lines in an array called config
 */
count = 0;
while (fgets (config[count], BUFSIZE, fdin) != NULL) {
  if (strlen (config[count] > 1) {
      if ((tmp_ptr = strchr (config[count], "\n")) != NULL)
	strcpy (tmp_ptr, "\0");
      i++;
    }
    }

  /*
   * Parse the config
   */
  j = 0;
  while (j < count) {
    strcpy (temp[j], config[j]);

    h = 0;
    argvec[j][h] = strtok (temp[j], ":\n");
    h++;
    while ((argvec[j][h] = strtok (NULL, ":\n")) != NULL)
      h++;

    j++;
  }
