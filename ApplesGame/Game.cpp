#include "Game.h"
#include <cassert>

namespace ApplesGame
{


	void StartPlayingState(Game& game)
	{
		SetPlayerPosition(game.player, { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f });
		SetPlayerSpeed(game.player, INITIAL_SPEED);
		SetPlayerDirection(game.player, PlayerDirection::Right);

		// Init apples
		for (int i = 0; i < game.NUM_APPLES; ++i)
		{
			SetApplePosition(game.apples[i], GetRandomPositionInRectangle(game.screenRect));
		}

		// Init rocks
		for (int i = 0; i < NUM_ROCKS; ++i)
		{
			SetRockPosition(game.rocks[i], GetRandomPositionInRectangle(game.screenRect));
		}

		game.numEatenApples = 0;
		game.isGameFinished = false;
		game.timeSinceGameFinish = 0;
		game.scoreText.setString("Apples eaten: " + std::to_string(game.numEatenApples));
	}

	void UpdatePlayingState(Game& game, float deltaTime)
	{
		// Handle input
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			SetPlayerDirection(game.player, PlayerDirection::Right);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			SetPlayerDirection(game.player, PlayerDirection::Up);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			SetPlayerDirection(game.player, PlayerDirection::Left);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			SetPlayerDirection(game.player, PlayerDirection::Down);
		}

		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
		{
			RestartGame(game, deltaTime);
		}


		UpdatePlayer(game.player, deltaTime);

		// Find player collisions with apples
		for (int i = 0; i < game.NUM_APPLES; ++i)
		{

			if (!game.applesAlive[i])
				continue;

			if (DoShapesCollide(GetPlayerCollider(game.player), GetAppleCollider(game.apples[i])))
			{

				bool isInfiniteApples = game.gameMode & static_cast <uint32_t>(GameSettingBits::IsGameInfinite);


				if (isInfiniteApples)
				{
					SetApplePosition(game.apples[i], GetRandomPositionInRectangle(game.screenRect));
				}

				else
				{
					game.applesAlive[i] = false;
				}


				++game.numEatenApples;

				bool IsAcceleration = game.gameMode & static_cast <uint32_t>(GameSettingBits::IsGameWithAcceleration);


				if (IsAcceleration)
				{
					SetPlayerSpeed(game.player, GetPlayerSpeed(game.player) + ACCELERATION);
				}
				else
				{
					SetPlayerSpeed(game.player, GetPlayerSpeed(game.player));
				}

				game.eatAppleSound.play();
				game.scoreText.setString("Apples eaten: " + std::to_string(game.numEatenApples));
			}
		}

		// Find player collisions with rocks
		for (int i = 0; i < NUM_ROCKS; ++i)
		{
			if (DoShapesCollide(GetPlayerCollider(game.player), GetRockCollider(game.rocks[i])))
			{
				StartGameoverState(game);
			}
		}

		// Check screen borders collision
		if (!DoShapesCollide(GetPlayerCollider(game.player), game.screenRect))
		{
			StartGameoverState(game);
		}
	}


	void StartGameoverState(Game& game)
	{

		const std::string PLAYER_NAME = "Player";
		game.recordsTable[PLAYER_NAME] = game.numEatenApples;
		game.isGameFinished = true;
		game.timeSinceGameFinish = 0.f;
		game.gameOverSound.play();
		game.gameOverScoreText.setString("Your scores: " + std::to_string(game.numEatenApples));
	}


	void UpdateGameoverState(Game& game, float deltaTime)
	{

		if (game.timeSinceGameFinish <= PAUSE_LENGTH)
		{
			game.timeSinceGameFinish += deltaTime;
			game.background.setFillColor(sf::Color::Red);
		}
		else
		{
			// Reset backgound
			game.background.setFillColor(sf::Color::Black);

			StartPlayingState(game);
		}
	}

	std::string GetLeaderboardString(const std::unordered_map<std::string, int>& records)
	{
		// Переносим в мультимап (очки ? имя) для сортировки
		std::multimap<int, std::string> sortedRecords;
		for (const auto& item : records)
		{
			sortedRecords.insert({ item.second, item.first });
		}

		std::string result = "===== LEADERBOARD =====\n";

		int position = 1;
		// Идём в обратном порядке (от большего к меньшему)
		for (auto it = sortedRecords.rbegin(); it != sortedRecords.rend() && position <= 5; ++it, ++position)
		{
			int score = it->first;
			const std::string& name = it->second;

			// Форматируем строку с точками
			std::string line = std::to_string(position) + ". " + name;



			auto dotsNeeded = 20 - line.length(); // примерно для выравнивания
			for (int i = 0; i < dotsNeeded; ++i)
				line += ".";
			line += " " + std::to_string(score);

			result += line + "\n";
		}

		result += "=======================\n";
		return result;
	}


	void RestartGame(Game& game, float deltaTime)
	{
		InitPlayer(game.player, game);
		StartGameoverState(game);
		game.numEatenApples = 0;
	}

	void InitializeLeaderBoard(Game& game)
	{
		game.recordsTable.clear();

		std::vector<std::string> names = {
			"Alice", "Bob", "Carol", "Dave", "Eve",
			"Frank", "Grace", "Henry", "Iris", "Jack"
		};

		for (const auto& name : names)
		{
			int randomScore = 1 + (rand() % 30);
			game.recordsTable[name] = randomScore;
		}
	}


	void InitGame(Game& game)
	{
		// Load resources
		assert(game.playerTexture.loadFromFile(RESOURCES_PATH + "\\Player.png"));
		assert(game.appleTexture.loadFromFile(RESOURCES_PATH + "\\Apple.png"));
		assert(game.rockTexture.loadFromFile(RESOURCES_PATH + "\\Rock.png"));
		assert(game.eatAppleSoundBuffer.loadFromFile(RESOURCES_PATH + "\\AppleEat.wav"));
		assert(game.gameOverSoundBuffer.loadFromFile(RESOURCES_PATH + "\\Death.wav"));
		assert(game.font.loadFromFile(RESOURCES_PATH + "\\Fonts\\Roboto-Bold.ttf"));


		InitializeLeaderBoard(game);

		game.gameMode = 0;
		game.gameMode |= static_cast <uint32_t>(GameSettingBits::HardcoreMode);
		game.gameMode |= static_cast <uint32_t>(GameSettingBits::IsGameWithAcceleration);
		game.gameMode |= static_cast <uint32_t>(GameSettingBits::IsGameInfinite);


		bool isHardcore = game.gameMode & static_cast <uint32_t> (GameSettingBits::HardcoreMode);


		if (!isHardcore)
		{
			game.NUM_APPLES = 20;

		}
		else if (isHardcore)
		{
			game.NUM_APPLES = 50;
		}
		else
		{
			game.NUM_APPLES = 20;
		}



		game.apples = new Apple[game.NUM_APPLES];
		// Init game objects
		game.screenRect = { 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT };

		InitPlayer(game.player, game);

		// Init apples
		for (int i = 0; i < game.NUM_APPLES; ++i)
		{
			game.applesAlive[i] = true;
			InitApple(game.apples[i], game);
		}

		// Init rocks
		for (int i = 0; i < NUM_ROCKS; ++i)
		{
			InitRock(game.rocks[i], game);
		}

		// Init background
		game.background.setSize(sf::Vector2f(game.screenRect.size.x, game.screenRect.size.y));
		game.background.setFillColor(sf::Color::Black);
		game.background.setPosition(0.f, 0.f);

		// Init sounds
		game.eatAppleSound.setBuffer(game.eatAppleSoundBuffer);
		game.gameOverSound.setBuffer(game.gameOverSoundBuffer);

		// Init texts
		game.scoreText.setFont(game.font);
		game.scoreText.setCharacterSize(20);
		game.scoreText.setFillColor(sf::Color::White);
		game.scoreText.setPosition(20.f, 10.f);

		game.controlsHintText.setFont(game.font);
		game.controlsHintText.setCharacterSize(20);
		game.controlsHintText.setFillColor(sf::Color::White);
		game.controlsHintText.setString("Use arrows to move, ESC to exit");
		game.controlsHintText.setPosition(SCREEN_WIDTH - game.controlsHintText.getGlobalBounds().width - 20.f, 10.f);

		game.controlsHintText2.setFont(game.font);
		game.controlsHintText2.setCharacterSize(20);
		game.controlsHintText2.setFillColor(sf::Color::Blue);
		game.controlsHintText2.setString("Press P to restart game");
		game.controlsHintText2.setPosition(SCREEN_WIDTH - game.controlsHintText2.getGlobalBounds().width - 20.f,
			SCREEN_HEIGHT - game.controlsHintText2.getGlobalBounds().height - 20.f);


		game.gameOverText.setFont(game.font);
		game.gameOverText.setCharacterSize(100);
		game.gameOverText.setFillColor(sf::Color::White);
		game.gameOverText.setPosition(SCREEN_WIDTH / 2.f - 200.f, SCREEN_HEIGHT / 2.f - 50.f);
		game.gameOverText.setString("Game Over");

		game.gameOverScoreText.setFont(game.font);
		game.gameOverScoreText.setCharacterSize(30);
		game.gameOverScoreText.setFillColor(sf::Color::White);
		game.gameOverScoreText.setString("Your score: " + std::to_string(game.numEatenApples));
		game.gameOverScoreText.setPosition(SCREEN_WIDTH / 2.f - game.controlsHintText.getGlobalBounds().width / 4.f, SCREEN_HEIGHT / 2.f + 50.f);





		StartPlayingState(game);
	}

	void UpdateGame(Game& game, float deltaTime)
	{
		// Update game state
		if (!game.isGameFinished)
		{
			UpdatePlayingState(game, deltaTime);
		}

		else
		{
			UpdateGameoverState(game, deltaTime);
		}
	}

	void DrawGame(Game& game, sf::RenderWindow& window)
	{
		// Draw background
		window.draw(game.background);

		// Draw game objects
		DrawPlayer(game.player, window);



		for (int i = 0; i < game.NUM_APPLES; ++i)
		{
			if (game.applesAlive[i]) {
				DrawApple(game.apples[i], window);
			}
		}

		for (int i = 0; i < NUM_ROCKS; ++i)
		{
			DrawRock(game.rocks[i], window);
		}

		// Draw texts
		if (!game.isGameFinished)
		{
			window.draw(game.scoreText);
			window.draw(game.controlsHintText);
			window.draw(game.controlsHintText2);
		}


		else

		{


			std::string leaderboardText = GetLeaderboardString(game.recordsTable);
			game.gameOverText.setString(leaderboardText);
			game.gameOverText.setCharacterSize(18);
			game.gameOverText.setFont(game.font);
			game.gameOverText.setPosition(50, 100);



			window.draw(game.gameOverText);

			window.draw(game.gameOverScoreText);
		}
	}

	void DeinializeGame(Game& game)
	{
		delete[]game.apples;
		game.apples = nullptr;
	}
}
