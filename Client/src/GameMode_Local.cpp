#define NOMINMAX

#include "SharedData.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include "GameMode_Local.h"
#include "GameModeManager.h"
#include "Graphics.h"

LogCategory LogGameModeLocal("GameModeLocal");

void GameMode_Local::beginPlay()
{
	LOG(LogGameModeLocal, LogVerbosity::Log, "Local Mode is Started");
	player1.beginPlay();
	player2.beginPlay();
	ball.beginPlay();
	net.beginPlay();
	leftWall.beginPlay();
	rightWall.beginPlay();
	floor.beginPlay();

	currentGameState = GameStateType::Playing;
	LOG(LogGameModeLocal, LogVerbosity::Log, "Game is Started");
	readyRound();
}

void GameMode_Local::endPlay()
{
	LOG(LogGameModeLocal, LogVerbosity::Log, "Local Mode is Terminated");
}

void GameMode_Local::tick(float elapsedTime)
{
	if (currentRoundState != RoundStateType::Ready)
	{
		updatePosition(elapsedTime);
		updatePhysics(elapsedTime);
	}

	if (currentRoundState == RoundStateType::Ready)
	{
		roundWaitTimer += elapsedTime;
		if (roundWaitTimer >= Config::Get().ROUND_WAIT_TIME)
		{
			startRound();
		}
	}
	else if (currentRoundState == RoundStateType::End)
	{
		if (currentGameState != GameStateType::End)
		{
			roundEndTimer += elapsedTime;
			if (roundEndTimer >= Config::Get().ROUND_END_TIME)
			{
				readyRound();
			}
		}
	}
}

void GameMode_Local::renderFrame(float elapsedTime)
{
	// Object Rendering
	Graphics::Get().renderObject(player1);
	Graphics::Get().renderObject(player2);
	Graphics::Get().renderObject(ball);
	Graphics::Get().renderObject(net);
	Graphics::Get().renderObject(leftWall);
	Graphics::Get().renderObject(rightWall);
	Graphics::Get().renderObject(floor);
	Graphics::Get().renderObject(ceil);

	// Text Rendering
	std::ostringstream fpsString;
	fpsString << std::fixed << std::setprecision(1) << (1.0f / elapsedTime);
	Graphics::Get().renderText((fpsString.str() + " FPS").c_str(), 20.f, Config::Get().SCREEN_HEIGHT - 30.f, 0.25f, FPS_TEXT_COLOR);
	Graphics::Get().renderText(std::to_string(scorePlayer1), Config::Get().INITIAL_PLAYER1_POSITION.x, Config::Get().SCREEN_HEIGHT - 100.f, 1.f, SCORE_TEXT_COLOR, true);
	Graphics::Get().renderText(std::to_string(scorePlayer2), Config::Get().INITIAL_PLAYER2_POSITION.x, Config::Get().SCREEN_HEIGHT - 100.f, 1.f, SCORE_TEXT_COLOR, true);
	if (currentRoundState == RoundStateType::Ready)
		Graphics::Get().renderText("Ready?", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - TEXT_SIZE / 2.f, 1.f, READY_TEXT_COLOR, true);
	if (currentGameState == GameStateType::End)
	{
		Graphics::Get().renderText("Game Set!", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - TEXT_SIZE / 2.f, 1.f, GAME_SET_TEXT_COLOR, true);
		Graphics::Get().renderText("Press ESC or click the 'X' button to exit.", Config::Get().SCREEN_WIDTH / 2.f, Config::Get().SCREEN_HEIGHT / 2.f - 75, 0.7f, EXCEPTION_TEXT_COLOR, true);
	}
}

void GameMode_Local::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	static bool DPressed = false, GPressed = false, RPressed = false, FPressed = false;
	static bool leftPressed = false, rightPressed = false, upPressed = false, downPressed = false;

	if (currentGameState == GameStateType::End)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			Graphics::Get().reserveClose();
		}
		return;
	}

	if (currentGameState != GameStateType::Playing || currentRoundState != RoundStateType::Playing)
	{
		DPressed = false, GPressed = false, RPressed = false, FPressed = false;
		player1.setAcc(glm::vec2(0, player1.getAcc().y));

		leftPressed = false, rightPressed = false, upPressed = false, downPressed = false;
		player2.setAcc(glm::vec2(0, player2.getAcc().y));
		return;
	}

	// player1
	if (key == GLFW_KEY_D && action == GLFW_PRESS) DPressed = true;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE) DPressed = false;
	if (key == GLFW_KEY_G && action == GLFW_PRESS) GPressed = true;
	if (key == GLFW_KEY_G && action == GLFW_RELEASE) GPressed = false;
	if (key == GLFW_KEY_R && action == GLFW_PRESS) RPressed = true;
	if (key == GLFW_KEY_R && action == GLFW_RELEASE) RPressed = false;
	if (key == GLFW_KEY_F && action == GLFW_PRESS) FPressed = true;
	if (key == GLFW_KEY_F && action == GLFW_RELEASE) FPressed = false;
	if (!player1.getSliding())
	{
		// jump
		if (key == GLFW_KEY_R && action == GLFW_PRESS && !player1.getJumping()) player1.jump();

		// move
		if (DPressed && GPressed) player1.setAcc(glm::vec2(0, player1.getAcc().y));
		else if (DPressed) player1.setAcc(glm::vec2(-Config::Get().PLAYER_MOVE_ACC, player1.getAcc().y));
		else if (GPressed) player1.setAcc(glm::vec2(Config::Get().PLAYER_MOVE_ACC, player1.getAcc().y));
		else player1.setAcc(glm::vec2(0, player1.getAcc().y));

		// slide
		if (key == GLFW_KEY_Z && action == GLFW_PRESS && !player1.getJumping())
		{
			if (DPressed)
			{
				player1.slide(false);
				DPressed = GPressed = false;
			}
			else if (GPressed)
			{
				player1.slide(true);
				DPressed = GPressed = false;
			}
		}
	}
	if (player1.getJumping())
	{
		// spike direction
		if (!DPressed && !GPressed && !RPressed && !FPressed)
		{
			player1.setSpikeDirection(SpikeDirectionType::None);
		}
		else if (DPressed || GPressed)
		{
			if (RPressed) player1.setSpikeDirection(SpikeDirectionType::Front_Up);
			else if (FPressed) player1.setSpikeDirection(SpikeDirectionType::Front_Down);
			else player1.setSpikeDirection(SpikeDirectionType::Front);
		}
		else if (RPressed) player1.setSpikeDirection(SpikeDirectionType::Up);
		else if (FPressed) player1.setSpikeDirection(SpikeDirectionType::Down);

		// spike
		if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		{
			player1.reserveSpike();
		}
	}

	// player2
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) leftPressed = true;
	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) leftPressed = false;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) rightPressed = true;
	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) rightPressed = false;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) upPressed = true;
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE) upPressed = false;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) downPressed = true;
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) downPressed = false;
	if (!player2.getSliding())
	{
		// jump
		if (key == GLFW_KEY_UP && action == GLFW_PRESS && !player2.getJumping()) player2.jump();

		// move
		if (leftPressed && rightPressed) player2.setAcc(glm::vec2(0, player2.getAcc().y));
		else if (leftPressed) player2.setAcc(glm::vec2(-Config::Get().PLAYER_MOVE_ACC, player2.getAcc().y));
		else if (rightPressed) player2.setAcc(glm::vec2(Config::Get().PLAYER_MOVE_ACC, player2.getAcc().y));
		else player2.setAcc(glm::vec2(0, player2.getAcc().y));

		// slide
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS && !player2.getJumping())
		{
			if (leftPressed)
			{
				player2.slide(false);
				leftPressed = rightPressed = false;
			}
			else if (rightPressed)
			{
				player2.slide(true);
				leftPressed = rightPressed = false;
			}
		}
	}
	if (player2.getJumping())
	{
		// spike direction
		if (!leftPressed && !rightPressed && !upPressed && !downPressed)
		{
			player2.setSpikeDirection(SpikeDirectionType::None);
		}
		else if (leftPressed || rightPressed)
		{
			if (upPressed) player2.setSpikeDirection(SpikeDirectionType::Front_Up);
			else if (downPressed) player2.setSpikeDirection(SpikeDirectionType::Front_Down);
			else player2.setSpikeDirection(SpikeDirectionType::Front);
		}
		else if (upPressed) player2.setSpikeDirection(SpikeDirectionType::Up);
		else if (downPressed) player2.setSpikeDirection(SpikeDirectionType::Down);

		// spike
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
		{
			player2.reserveSpike();
		}
	}
}


void GameMode_Local::readyRound()
{
	currentRoundState = RoundStateType::Ready;
	GameModeManager::Get().currentTimeScale = Config::Get().BASIC_TIME_SCALE;
	player1.reset();
	player2.reset();
	ball.reset();
	roundWaitTimer = 0.f;
	LOG(LogGameModeLocal, LogVerbosity::Log, "Ready Round %d", scorePlayer1 + scorePlayer2 + 1);
}

void GameMode_Local::startRound()
{
	currentRoundState = RoundStateType::Playing;
	GameModeManager::Get().currentTimeScale = Config::Get().BASIC_TIME_SCALE;
	LOG(LogGameModeLocal, LogVerbosity::Log, "Start Round %d", scorePlayer1 + scorePlayer2 + 1);
}

void GameMode_Local::endRound()
{
	currentRoundState = RoundStateType::End;
	LOG(LogGameModeLocal, LogVerbosity::Log, "End Round %d. Current Round Score: [%d:%d]. Round Winner Player: %d",
		scorePlayer1 + scorePlayer2, scorePlayer1, scorePlayer2, lastRoundWinnerPlayerID);

	// game set
	if (scorePlayer1 >= Config::Get().MAX_SCORE || scorePlayer2 >= Config::Get().MAX_SCORE)
	{
		currentGameState = GameStateType::End;
		LOG(LogGameModeLocal, LogVerbosity::Log, "Game is Finished! Winner Player: %d", scorePlayer1 >= Config::Get().MAX_SCORE ? player1.getPlayerID() : player2.getPlayerID());
	}
	else
	{
		GameModeManager::Get().currentTimeScale = Config::Get().ROUND_END_TIME_SCALE;
		roundEndTimer = 0.f;
	}
}

bool GameMode_Local::checkCollision(GameObject obj1, GameObject obj2)
{
	return obj1.getPosition().x + obj1.getWidth()/2 > obj2.getPosition().x - obj2.getWidth()/2
		&& obj1.getPosition().x - obj1.getWidth()/2 < obj2.getPosition().x + obj2.getWidth()/2
		&& obj1.getPosition().y + obj1.getHeight()/2 > obj2.getPosition().y - obj2.getHeight()/2
		&& obj1.getPosition().y - obj1.getHeight()/2 < obj2.getPosition().y + obj2.getHeight()/2;
}

void GameMode_Local::updatePosition(float elapsedTime)
{
	player1.updatePosition(elapsedTime);
	player1.updateSpeed(elapsedTime);
	player1.updateState(elapsedTime);

	player2.updatePosition(elapsedTime);
	player2.updateSpeed(elapsedTime);
	player2.updateState(elapsedTime);

	ball.updatePosition(elapsedTime);
	ball.updateSpeed(elapsedTime);
}

void GameMode_Local::updatePhysics(float elapsedTime)
{
	// ball - floor
	if (checkCollision(ball, floor))
	{
		if( currentGameState == GameStateType::Playing && currentRoundState == RoundStateType::Playing )
		{
			if (ball.getPosition().x < Config::Get().SCREEN_WIDTH / 2)
			{
				scorePlayer2++;
				lastRoundWinnerPlayerID = player2.getPlayerID();
			}
			else
			{
				scorePlayer1++;
				lastRoundWinnerPlayerID = player1.getPlayerID();
			}
			endRound();
			return;
		}

		ball.setPosition(glm::vec2(ball.getPosition().x, floor.getTop() + ball.getHeight() / 2));
		ball.setSpeed(glm::vec2(ball.getSpeed().x, std::abs(ball.getSpeed().y)));
	}

	// ball - ceil
	if (checkCollision(ball, ceil))
	{
		ball.setPosition(glm::vec2(ball.getPosition().x, ceil.getBottom() - ball.getHeight() / 2));
		ball.setSpeed(glm::vec2(ball.getSpeed().x, 0.f));
	}

	// ball - net
	if (checkCollision(ball, net))
	{
		float overlapX = std::min(ball.getRight(), net.getRight()) - std::max(ball.getLeft(), net.getLeft());
		float overlapY = std::min(ball.getTop(), net.getTop()) - std::max(ball.getBottom(), net.getBottom());
		if(overlapX > overlapY)
		{
			ball.setPosition(glm::vec2(ball.getPosition().x, net.getTop() + ball.getHeight() / 2));
			ball.setSpeed(glm::vec2(ball.getSpeed().x, std::abs(ball.getSpeed().y)));
		}
		else if (net.getPosition().x < ball.getPosition().x)
		{
			ball.setPosition(glm::vec2(net.getRight() + ball.getWidth() / 2, ball.getPosition().y));
			ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
		}
		else if (net.getPosition().x > ball.getPosition().x)
		{
			ball.setPosition(glm::vec2(net.getLeft() - ball.getWidth() / 2, ball.getPosition().y));
			ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
		}
	}

	// ball - player1
	if (checkCollision(ball, player1))
	{
		if(ball.getIgnorePlayerCollision() == false)
		{
			// position
			{
				float overlapX = std::min(ball.getRight(), player1.getRight()) - std::max(ball.getLeft(), player1.getLeft());
				float overlapY = std::min(ball.getTop(), player1.getTop()) - std::max(ball.getBottom(), player1.getBottom());
				if (overlapX > overlapY)
				{
					ball.setPosition(glm::vec2(ball.getPosition().x, player1.getTop() + ball.getHeight() / 2));
				}
				else if (player1.getPosition().x < ball.getPosition().x)
				{
					ball.setPosition(glm::vec2(player1.getRight() + ball.getWidth() / 2, ball.getPosition().y));
				}
				else if (player1.getPosition().x > ball.getPosition().x)
				{
					ball.setPosition(glm::vec2(player1.getLeft() - ball.getWidth() / 2, ball.getPosition().y));
				}
			}

			// speed
			{
				glm::vec2 newBallSpeed = ball.getSpeed();

				// spike
				if(player1.getSpikeReserved())
				{
					switch (player1.getSpikeDirection())
					{
						case SpikeDirectionType::None:
							newBallSpeed.x = Config::Get().BALL_SPIKE_SPEED;
							newBallSpeed.y = 0.f;
							break;
						case SpikeDirectionType::Front:
							newBallSpeed.x = Config::Get().BALL_SPIKE_SPEED * 2;
							newBallSpeed.y = 0.f;
							break;
						case SpikeDirectionType::Up:
							newBallSpeed.x = Config::Get().BALL_SPIKE_SPEED;
							newBallSpeed.y = std::abs(ball.getSpeed().y * 2);
							break;
						case SpikeDirectionType::Down:
							newBallSpeed.x = Config::Get().BALL_SPIKE_SPEED;
							newBallSpeed.y = -std::abs(ball.getSpeed().y * 2);
							break;
						case SpikeDirectionType::Front_Up:
							newBallSpeed.x = Config::Get().BALL_SPIKE_SPEED * 2;
							newBallSpeed.y = std::abs(ball.getSpeed().y * 2);
							break;
						case SpikeDirectionType::Front_Down:
							newBallSpeed.x = Config::Get().BALL_SPIKE_SPEED * 2;
							newBallSpeed.y = -std::abs(ball.getSpeed().y * 2);
							break;
						default:
							LOG(LogGameModeLocal, LogVerbosity::Error, "updatePhysics: Unhandled Spike Direction!");
					}
				}
				// normal collision
				else
				{
					newBallSpeed.x = Config::Get().NORMAL_COLLISION_IMPACT_FACTOR * (ball.getPosition().x - player1.getPosition().x);
					newBallSpeed.y = std::abs(ball.getSpeed().y);
				}
				ball.setSpeed(newBallSpeed);
			}
			ball.setIgnorePlayerCollision(true);
		}
	}
	// ball - player2
	else if (checkCollision(ball, player2))
	{
		if (ball.getIgnorePlayerCollision() == false)
		{
			// position
			{
				float overlapX = std::min(ball.getRight(), player2.getRight()) - std::max(ball.getLeft(), player2.getLeft());
				float overlapY = std::min(ball.getTop(), player2.getTop()) - std::max(ball.getBottom(), player2.getBottom());
				if (overlapX > overlapY)
				{
					ball.setPosition(glm::vec2(ball.getPosition().x, player2.getTop() + ball.getHeight() / 2));
				}
				else if (player2.getPosition().x < ball.getPosition().x)
				{
					ball.setPosition(glm::vec2(player2.getRight() + ball.getWidth() / 2, ball.getPosition().y));
				}
				else if(player2.getPosition().x > ball.getPosition().x)
				{
					ball.setPosition(glm::vec2(player2.getLeft() - ball.getWidth() / 2, ball.getPosition().y));
				}
			}

			// speed
			{
				glm::vec2 newBallSpeed = ball.getSpeed();

				// spike
				if (player2.getSpikeReserved())
				{
					switch (player2.getSpikeDirection())
					{
						case SpikeDirectionType::None:
							newBallSpeed.x = -Config::Get().BALL_SPIKE_SPEED;
							newBallSpeed.y = 0.f;
							break;
						case SpikeDirectionType::Front:
							newBallSpeed.x = -Config::Get().BALL_SPIKE_SPEED * 2;
							newBallSpeed.y = 0.f;
							break;
						case SpikeDirectionType::Up:
							newBallSpeed.x = -Config::Get().BALL_SPIKE_SPEED;
							newBallSpeed.y = std::abs(ball.getSpeed().y * 2);
							break;
						case SpikeDirectionType::Down:
							newBallSpeed.x = -Config::Get().BALL_SPIKE_SPEED;
							newBallSpeed.y = -std::abs(ball.getSpeed().y * 2);
							break;
						case SpikeDirectionType::Front_Up:
							newBallSpeed.x = -Config::Get().BALL_SPIKE_SPEED * 2;
							newBallSpeed.y = std::abs(ball.getSpeed().y * 2);
							break;
						case SpikeDirectionType::Front_Down:
							newBallSpeed.x = -Config::Get().BALL_SPIKE_SPEED * 2;
							newBallSpeed.y = -std::abs(ball.getSpeed().y * 2);
							break;
						default:
							LOG(LogGameModeLocal, LogVerbosity::Error, "updatePhysics: Unhandled Spike Direction!");
					}
				}
				// normal collision
				else
				{
					newBallSpeed.x = Config::Get().NORMAL_COLLISION_IMPACT_FACTOR * (ball.getPosition().x - player2.getPosition().x);
					newBallSpeed.y = std::abs(ball.getSpeed().y);
				}
				ball.setSpeed(newBallSpeed);
			}
			ball.setIgnorePlayerCollision(true);
		}
	}
	else
	{
		ball.setIgnorePlayerCollision(false);
	}

	// ball - wall
	if (checkCollision(ball, leftWall))
	{
		ball.setPosition(glm::vec2(leftWall.getRight() + ball.getWidth() / 2, ball.getPosition().y));
		ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
	}
	if (checkCollision(ball, rightWall))
	{
		ball.setPosition(glm::vec2(rightWall.getLeft() - ball.getWidth() / 2, ball.getPosition().y));
		ball.setSpeed(glm::vec2(-ball.getSpeed().x, ball.getSpeed().y));
	}
}
