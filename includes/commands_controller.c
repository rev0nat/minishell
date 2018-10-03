/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands_controller.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguillot <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/18 17:44:19 by aguillot          #+#    #+#             */
/*   Updated: 2018/09/18 17:44:20 by aguillot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


void	free_tabs(char **tab)
{
	off_t i;

	i = 0;
	while (tab[i])
	{
		ft_strdel(&tab[i]);
		i++;
	}
	free(tab);
}

int		is_it_command(char *command)
{
	if (ft_strcmp(command, "echo") == 0 || ft_strcmp(command, "setenv") == 0\
		|| ft_strcmp(command, "env") == 0 || ft_strcmp(command, "unsetenv")\
		== 0 || ft_strcmp(command, "cd") == 0 || ft_strcmp(command, "exit")\
		== 0)
		return (1);
	return (-1);
}

void 	fourchette(t_minish *minish, char *paths_p_command, off_t command_id)
{
	char 	**env_tab;
	char	**av_tab;
	pid_t	pid;

	env_tab = get_env_tab(minish->env_list);
	av_tab = get_input_tab(minish->input_list, command_id);
	if ((pid = fork()) == 0)
	{
		execve(paths_p_command, av_tab, env_tab);
		free_tabs(env_tab);
		free_tabs(av_tab);
	}
	else if (pid < 0)
	{
		ft_putstr("Error: Fork failed to create a new process.\n");
		free_tabs(env_tab);
		free_tabs(av_tab);
	}
	if (env_tab)
		free_tabs(env_tab);
	if (av_tab)
		free_tabs(av_tab);
	wait(&pid);
}


void	commands_controller(t_input_node *input_node, t_minish *minish)
{
	off_t 			i;
	char			**paths;
	char			**paths_p_command;

	i = 0;
	if (input_node->words[0][0] != '$')
	{
		if (access(input_node->words[0], X_OK) == 0 || access(input_node->words[0], R_OK) == 0)
			fourchette(minish, input_node->words[0], input_node->command_id);
		else
		{
			paths = get_paths(minish->env_list);
			if ((paths_p_command = add_command_to_paths(input_node->words[0], paths)) == NULL)
				print_error_path(input_node->words[0]);
			else
			{
				while(paths_p_command[i])
				{
					if (access(paths_p_command[i], X_OK) == 0 || access(paths_p_command[i], R_OK) == 0)
						break;
					i++;
				}
				if (paths_p_command[i])
					fourchette(minish, paths_p_command[i], input_node->command_id);
				else
					print_error_path(input_node->words[0]);
			}
			free_double_tab(paths_p_command);
			free_double_tab(paths);
		}
		//free_lists_and_carry_on(minish);
	}
}
