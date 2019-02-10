#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include <include/interface.h>
#include <include/player.h>
#include <include/bomberman.h>
#include <include/bomb.h>
#include <include/game.h>
#include <include/server.h>
#include <include/client.h>


/*
* CLI usage only.
* waiting for someone to implement GUI menu
*/
void usage(char *arg)
{
	printf("usage : %s <type>\n", arg);
	printf("type : client or server\n");
}

int main(int argc, char *argv[])
{
	game_t *game = NULL;
	pthread_t thread_sdl, thread_net;

	if (argc != 2) {
		usage(argv[0]);
		return -1;
	}

	game = init_game();

	if (game == NULL) {
		fprintf(stderr, "failed to to init game\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&thread_sdl, NULL, game_loop, (void*) game) < 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&thread_net, NULL, start_networking, (void*) argv[1]) < 0) {
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(thread_sdl, NULL) != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	printf("stop\n");
	pthread_kill(thread_net, SIGSTOP);
	perror("pthread_kill");

	destroy_game(game->interface, game->player, game->bomb);
}
