#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Constants.h"
#include "Math.h"
#include "Player.h"
#include "Apple.h"
#include "Rock.h"
#include <map>
#include <unordered_map>

namespace ApplesGame
{

	enum class GameSettingBits
	{
		IsGameInfinite = 1 << 0,
		IsGameWithAcceleration = 1 << 1,
		HardcoreMode = 1 << 2,
		EasyMode = 1 << 3,
	};

	struct Game
	{
		Rectangle screenRect;
		Player player;
		Apple* apples;
		Rock rocks[NUM_ROCKS];
		uint32_t gameMode = 0;
		int NUM_APPLES = 20;


		std::unordered_map<std::string, int> recordsTable;


		// Global game data
		int numEatenApples = 0;
		bool isGameFinished = false;
		bool applesAlive[MAX_APPLES];
		float timeSinceGameFinish = 0.f;


		// Resources
		sf::Texture playerTexture;
		sf::Texture appleTexture;
		sf::Texture rockTexture;
		sf::SoundBuffer eatAppleSoundBuffer;
		sf::SoundBuffer gameOverSoundBuffer;

		// Game objects
		sf::Sound eatAppleSound;
		sf::Sound gameOverSound;
		sf::RectangleShape background;

		// Texts and fonts
		sf::Font font;
		sf::Text scoreText;
		sf::Text controlsHintText;
		sf::Text gameOverText;
		sf::Text gameOverScoreText;
		sf::Text controlsHintText2;
	};



	void InitGame(Game& game);
	void UpdateGame(Game& game, float deltaTime);
	void DrawGame(Game& game, sf::RenderWindow& window);
	void DeinializeGame(Game& game);
	void StartPlayingState(Game& game);
	void UpdatePlayingState(Game& game, float deltaTime);
	void StartGameoverState(Game& game);
	void UpdateGameoverState(Game& game, float deltaTime);
	std::string GetLeaderboardString(const std::unordered_map<std::string, int>& records);
	void RestartGame(Game& game, float deltaTime);
	void InitializeLeaderBoard(Game& game);
}