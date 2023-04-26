#include "main.h"

/**
 * input_bfr - buffers chained commands
 * @info: parameter struct
 * @bfr: address of buffer
 * @len: address of len var
 *
 * Return: bytes read
 */
ssize_t input_bfr(info_t *info, char **bfr, size_t *len)
{
	ssize_t r = 0;
	size_t len_p = 0;

	if (!*len) /* if nothing left in the bfrfer, fill it */
	{
		/*bfree((void **)info->cmd_bfr);*/
		free(*bfr);
		*bfr = NULL;
		signal(SIGINT, sigintHandler);
#if USE_GETLINE
		r = getline(bfr, &len_p, stdin);
#else
		r = _getline(info, bfr, &len_p);
#endif
		if (r > 0)
		{
			if ((*bfr)[r - 1] == '\n')
			{
				(*bfr)[r - 1] = '\0'; /* remove trailing newline */
				r--;
			}
			info->linecount_flag = 1;
			remove_comments(*bfr);
			build_history_list(info, *bfr, info->histcount++);
			/* if (_strchr(*bfr, ';')) is this a command chain? */
			{
				*len = r;
				info->cmd_bfr = bfr;
			}
		}
	}
	return (r);
}

/**
 * get_input - gets a line minus the newline
 * @info: parameter struct
 *
 * Return: bytes read
 */
ssize_t get_input(info_t *info)
{
	static char *bfr; /* the ';' command chain bfrfer */
	static size_t i, j, len;
	ssize_t r = 0;
	char **bfr_p = &(info->arg), *p;

	_putchar(BUF_FLUSH);
	r = input_bfr(info, &bfr, &len);
	if (r == -1) /* EOF */
		return (-1);
	if (len) /* we have commands left in the chain bfrfer */
	{
		j = i; /* init new iterator to current bfr position */
		p = bfr + i; /* get pointer for return */

		check_chain(info, bfr, &j, i, len);
		while (j < len) /* iterate to semicolon or end */
		{
			if (is_chain(info, bfr, &j))
				break;
			j++;
		}

		i = j + 1; /* increment past nulled ';'' */
		if (i >= len) /* reached end of bfrfer? */
		{
			i = len = 0; /* reset position and length */
			info->cmd_bfr_type = CMD_NORM;
		}

		*bfr_p = p; /* pass back pointer to current command position */
		return (_strlen(p)); /* return length of current command */
	}

	*bfr_p = bfr; /* else not a chain, pass back bfrfer from _getline() */
	return (r); /* return length of bfrfer from _getline() */
}

/**
 * read_bfr - reads a bfrfer
 * @info: parameter struct
 * @bfr: bfrfer
 * @i: size
 *
 * Return: r
 */
ssize_t read_bfr(info_t *info, char *bfr, size_t *i)
{
	ssize_t r = 0;

	if (*i)
		return (0);
	r = read(info->readfd, bfr, READ_BUF_SIZE);
	if (r >= 0)
		*i = r;
	return (r);
}

/**
 * _getline - gets the next line of input from STDIN
 * @info: parameter struct
 * @ptr: address of pointer to bfrfer, preallocated or NULL
 * @length: size of preallocated ptr bfrfer if not NULL
 *
 * Return: s
 */
int _getline(info_t *info, char **ptr, size_t *length)
{
	static char bfr[READ_BUF_SIZE];
	static size_t i, len;
	size_t k;
	ssize_t r = 0, s = 0;
	char *p = NULL, *new_p = NULL, *c;

	p = *ptr;
	if (p && length)
		s = *length;
	if (i == len)
		i = len = 0;

	r = read_bfr(info, bfr, &len);
	if (r == -1 || (r == 0 && len == 0))
		return (-1);

	c = _strchr(bfr + i, '\n');
	k = c ? 1 + (unsigned int)(c - bfr) : len;
	new_p = _realloc(p, s, s ? s + k : k + 1);
	if (!new_p) /* MALLOC FAILURE! */
		return (p ? free(p), -1 : -1);

	if (s)
		_strncat(new_p, bfr + i, k - i);
	else
		_strncpy(new_p, bfr + i, k - i + 1);

	s += k - i;
	i = k;
	p = new_p;

	if (length)
		*length = s;
	*ptr = p;
	return (s);
}

/**
 * sigintHandler - blocks ctrl-C
 * @sig_num: the signal number
 *
 * Return: void
 */
void sigintHandler(__attribute__((unused))int sig_num)
{
	_puts("\n");
	_puts("$ ");
	_putchar(BUF_FLUSH);
}
