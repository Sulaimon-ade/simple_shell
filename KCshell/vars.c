#include "main.h"

/**
 * is_chain - test if current char in bfrfer is a chain delimeter
 * @info: the parameter struct
 * @bfr: the char bfrfer
 * @p: address of current position in bfr
 *
 * Return: 1 if chain delimeter, 0 otherwise
 */
int is_chain(info_t *info, char *bfr, size_t *p)
{
	size_t j = *p;

	if (bfr[j] == '|' && bfr[j + 1] == '|')
	{
		bfr[j] = 0;
		j++;
		info->cmd_bfr_type = CMD_OR;
	}
	else if (bfr[j] == '&' && bfr[j + 1] == '&')
	{
		bfr[j] = 0;
		j++;
		info->cmd_bfr_type = CMD_AND;
	}
	else if (bfr[j] == ';') /* found end of this command */
	{
		bfr[j] = 0; /* replace semicolon with null */
		info->cmd_bfr_type = CMD_CHAIN;
	}
	else
		return (0);
	*p = j;
	return (1);
}

/**
 * check_chain - checks we should continue chaining based on last status
 * @info: the parameter struct
 * @bfr: the char bfrfer
 * @p: address of current position in bfr
 * @i: starting position in bfr
 * @len: length of bfr
 *
 * Return: Void
 */
void check_chain(info_t *info, char *bfr, size_t *p, size_t i, size_t len)
{
	size_t j = *p;

	if (info->cmd_bfr_type == CMD_AND)
	{
		if (info->status)
		{
			bfr[i] = 0;
			j = len;
		}
	}
	if (info->cmd_bfr_type == CMD_OR)
	{
		if (!info->status)
		{
			bfr[i] = 0;
			j = len;
		}
	}

	*p = j;
}

/**
 * replace_alias - replaces an aliases in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 otherwise
 */
int replace_alias(info_t *info)
{
	int i;
	list_t *node;
	char *p;

	for (i = 0; i < 10; i++)
	{
		node = node_starts_with(info->alias, info->argv[0], '=');
		if (!node)
			return (0);
		free(info->argv[0]);
		p = _strchr(node->str, '=');
		if (!p)
			return (0);
		p = _strdup(p + 1);
		if (!p)
			return (0);
		info->argv[0] = p;
	}
	return (1);
}

/**
 * replace_vars - replaces vars in the tokenized string
 * @info: the parameter struct
 *
 * Return: 1 if replaced, 0 otherwise
 */
int replace_vars(info_t *info)
{
	int i = 0;
	list_t *node;

	for (i = 0; info->argv[i]; i++)
	{
		if (info->argv[i][0] != '$' || !info->argv[i][1])
			continue;

		if (!_strcmp(info->argv[i], "$?"))
		{
			replace_string(&(info->argv[i]),
					_strdup(convert_number(info->status, 10, 0)));
			continue;
		}
		if (!_strcmp(info->argv[i], "$$"))
		{
			replace_string(&(info->argv[i]),
					_strdup(convert_number(getpid(), 10, 0)));
			continue;
		}
		node = node_starts_with(info->env, &info->argv[i][1], '=');
		if (node)
		{
			replace_string(&(info->argv[i]),
					_strdup(_strchr(node->str, '=') + 1));
			continue;
		}
		replace_string(&info->argv[i], _strdup(""));

	}
	return (0);
}

/**
 * replace_string - replaces string
 * @old: address of old string
 * @new: new string
 *
 * Return: 1 if replaced, 0 otherwise
 */
int replace_string(char **old, char *new)
{
	free(*old);
	*old = new;
	return (1);
}
