/*
 * File: alias_builtins.c
 * Auth: Ebenezer Kissiedu
 *       Hanae
 */

#include "shell.h"

int shellby_alias(char **args, char __attribute__((__unused__)) **front);
void set_alias(char *alias_name, char *alias_value);
void print_alias(alias_t *alias);

/**
 * shellby_alias - Builtin command that either prints all aliases, specific
 * aliases, or sets an alias.
 * @args: An array of arguments.
 * @front: A double pointer to the beginning of args.
 *
 * Return: If an error occurs - -1.
 *         Otherwise - 0.
 */
int shellby_alias(char **args, char __attribute__((__unused__)) **front)
{
	alias_t *current_alias = aliases;
	int i, ret = 0;
	char *value;

	/* If no arguments are provided, print all aliases */
	if (!args[0])
	{
		while (current_alias)
		{
			print_alias(current_alias);
			current_alias = current_alias->next;
		}
		return (ret);
	}

	/* Loop through provided arguments */
	for (i = 0; args[i]; i++)
	{
		current_alias = aliases;
		value = _strchr(args[i], '=');

		/* If no '=' found, print the alias */
		if (!value)
		{
			while (current_alias)
			{
				if (_strcmp(args[i], current_alias->name) == 0)
				{
					print_alias(current_alias);
					break;
				}
				current_alias = current_alias->next;
			}
			if (!current_alias)
				ret = create_error(args + i, 1);
		}
		else
			set_alias(args[i], value);
	}
	return (ret);
}

/**
 * set_alias - Will either set an existing alias 'name' with a new value,
 * 'value' or creates a new alias with 'name' and 'value'.
 * @alias_name: Name of the alias.
 * @alias_value: Value of the alias. First character is a '='.
 */
void set_alias(char *alias_name, char *alias_value)
{
	alias_t *current_alias = aliases;
	int len, j, k;
	char *new_value;

	*alias_value = '\0';
	alias_value++;
	len = _strlen(alias_value) - _strspn(alias_value, "'\"");
	new_value = malloc(sizeof(char) * (len + 1));
	if (!new_value)
		return;

	/* Copy the value while removing quotes */
	for (j = 0, k = 0; alias_value[j]; j++)
	{
		if (alias_value[j] != '\'' && alias_value[j] != '"')
			new_value[k++] = alias_value[j];
	}
	new_value[k] = '\0';

	/* Check if the alias already exists and update its value */
	while (current_alias)
	{
		if (_strcmp(alias_name, current_alias->name) == 0)
		{
			free(current_alias->value);
			current_alias->value = new_value;
			break;
		}
		current_alias = current_alias->next;
	}

	/* If the alias doesn't exist, add a new alias */
	if (!current_alias)
		add_alias_end(&aliases, alias_name, new_value);
}

/**
 * print_alias - Prints the alias in the format name='value'.
 * @alias: Pointer to an alias.
 */
void print_alias(alias_t *alias)
{
	char *alias_string;
	int len = _strlen(alias->name) + _strlen(alias->value) + 4;

	alias_string = malloc(sizeof(char) * (len + 1));
	if (!alias_string)
		return;
	_strcpy(alias_string, alias->name);
	_strcat(alias_string, "='");
	_strcat(alias_string, alias->value);
	_strcat(alias_string, "'\n");

	write(STDOUT_FILENO, alias_string, len);
	free(alias_string);
}

/**
 * replace_aliases - Goes through the arguments and replaces any matching alias
 * with their value.
 * @args: 2D pointer to the arguments.
 *
 * Return: 2D pointer to the arguments.
 */
char **replace_aliases(char **args)
{
	alias_t *current_alias;
	int i;
	char *new_value;

	/* If the command is 'alias', do not replace aliases */
	if (_strcmp(args[0], "alias") == 0)
		return (args);

	/* Loop through the arguments and replace aliases with their values */
	for (i = 0; args[i]; i++)
	{
		current_alias = aliases;
		while (current_alias)
		{
			if (_strcmp(args[i], current_alias->name) == 0)
			{
				new_value = malloc(sizeof(char) * (_strlen(current_alias->value) + 1));
				if (!new_value)
				{
					free_args(args, args);
					return (NULL);
				}
				_strcpy(new_value, current_alias->value);
				free(args[i]);
				args[i] = new_value;
				i--;
				break;
			}
			current_alias = current_alias->next;
		}
	}

	return (args);
}

