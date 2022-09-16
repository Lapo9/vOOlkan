#ifndef VULKAN_GAMESTATUS
#define VULKAN_GAMESTATUS

#include <vector>

#include "Hitbox.h"
#include "Universe.h"

const Vulkan::Physics::Position RESTING_BALLS_POSITION{ 100.0f, 0.0f, 0.0f };
const Vulkan::Physics::Position READY_BALL{ 2.500f, -3.200f, 0.0f };


using Lights = struct {
	alignas(16) glm::vec3 color0;
	alignas(16) glm::vec3 position0;
	alignas(16) glm::vec3 color1;
	alignas(16) glm::vec3 position1;
	alignas(16) glm::vec3 color2;
	alignas(16) glm::vec3 position2;
	alignas(16) glm::vec3 color3;
	alignas(16) glm::vec3 position3;
	alignas(16) glm::vec3 color4;
	alignas(16) glm::vec3 position4;
	alignas(16) glm::vec3 color5;
	alignas(16) glm::vec3 position5;
	alignas(16) glm::vec3 color6;
	alignas(16) glm::vec3 position6;
	alignas(16) glm::vec3 color7;
	alignas(16) glm::vec3 position7;
	alignas(8) glm::vec2 decayFactor;

	alignas(16) glm::vec3 directionalLightColor;
	alignas(16) glm::vec3 directionalLightDirection;

	alignas(16) glm::vec3 basicAmbient;
	alignas(16) glm::vec3 dxColor;
	alignas(16) glm::vec3 dyColor;
	alignas(16) glm::vec3 dzColor;

	alignas(16) glm::vec3 eyePosition;
};

class GameStatus {
public:

	GameStatus(std::vector<Vulkan::Physics::Hitbox*> balls, std::vector<Vulkan::Physics::Hitbox*> bumpers, Lights& lights, Vulkan::Physics::Universe& universe) :
		balls{ balls }, bumpers{ bumpers }, universe{ universe }, pointLightsInfo{}, inGame{ false }, isMultiball{ false }, ballsInPlay(balls.size()), activeBumpers(bumpers.size()), lights{ lights }, lightsOff{ false } {
		pointLightsInfo.push_back({ lights.color0, lights.position0 });
		pointLightsInfo.push_back({ lights.color1, lights.position1 });
		pointLightsInfo.push_back({ lights.color2, lights.position2 });
		pointLightsInfo.push_back({ lights.color3, lights.position3 });
		pointLightsInfo.push_back({ lights.color4, lights.position4 });
		pointLightsInfo.push_back({ lights.color5, lights.position5 });
		pointLightsInfo.push_back({ lights.color6, lights.position6 });
		pointLightsInfo.push_back({ lights.color7, lights.position7 });

		setUpNewMatch();
	}


	void invertBumper(Vulkan::Physics::Hitbox* bumper) {
		auto foundBumper = std::find(bumpers.begin(), bumpers.end(), bumper);
		if (foundBumper != bumpers.end()) {
			int i = std::distance(bumpers.begin(), foundBumper);
			activeBumpers[i] = !activeBumpers[i];
			invertLight(i);
			checkMultiball();
			checkLight();
		}
	}


	void killBall(Vulkan::Physics::Hitbox* ball) {
		auto foundBall = std::find(balls.begin(), balls.end(), ball);
		if (foundBall != balls.end()) {
			universe.removeBody(*ball);
			ball->reset(RESTING_BALLS_POSITION);

			int i = std::distance(balls.begin(), foundBall);
			ballsInPlay[i] = false;
			setPointLight(i, false);
			checkBallsInPlay();
		}
	}


	void startNewGame(Vulkan::Physics::Speed speed) {
		if (!inGame) {
			ballsInPlay[0] = true;
			balls[0]->addExternalForce(Vulkan::Physics::Force{ speed * 5000.0f});
			universe.addBody(*balls[0]);
			inGame = true;
		}
	}


	bool isGameOver() const {
		return !inGame;
	}


	bool areLightsOff() const {
		return lightsOff;
	}


	void setLight(bool isOn) {
		lightsOff = !isOn;

		for (int i = bumpers.size(); i < balls.size() + bumpers.size(); ++i) {
			setPointLight(i, ballsInPlay[i-bumpers.size()] && !isOn, {0.0f, 0.1f, 0.0f});
		}

		if (lightsOff) {
			lights.directionalLightColor = { 0.0f, 0.0f, 0.0f };
		}
		else {
			lights.directionalLightColor = { 0.2f, 0.2f, 0.2f };
		}
	}


private:

	void checkMultiball() {
		int activeCount = 0;
		for (auto bumper : activeBumpers) {
			if (bumper) {
				activeCount++;
			}
		}

		if (activeCount >=2 && !isMultiball) {
			isMultiball = true;
			deactivateAllBumpers();
			for (int i = 0, activated = 0; i < balls.size(); ++i) {
				if (!ballsInPlay[i]) {
					ballsInPlay[i] = true;
					balls[i]->setPosition({ activated * 4.0f - 2.0f, 0.0f, 0.0f });
					universe.addBody(*balls[i]);
					activated++;
				}
			}
		}
	}



	void checkLight() {
		int activeCount = 0;
		for (auto bumper : activeBumpers) {
			if (bumper) {
				activeCount++;
			}
		}

		if (activeCount >= 3) {
			setLight(false);
		}
		if (activeCount <= 1) {
			setLight(true);
		}
	}



	void checkBallsInPlay() {
		int numberOfBallsInPlay = 0;
		for (auto isActive : ballsInPlay) {
			if (isActive) {
				numberOfBallsInPlay++;
			}
		}

		if (numberOfBallsInPlay == 1) {
			isMultiball = false;
		}
		else if (numberOfBallsInPlay == 0) {
			inGame = false;
			setUpNewMatch();
			setLight(true);
		}
	}



	void setUpNewMatch() {
		balls[0]->reset(READY_BALL);
		deactivateAllBumpers();
	}



	void deactivateAllBumpers() {
		for (int i = 0; i < bumpers.size(); ++i) {
			pointLightsInfo[i].first = { 0.0f, 0.0f, 0.0f };
			activeBumpers[i] = false;
		}
	}



	void invertLight(int i) {
		if (pointLightsInfo[i].first == glm::vec3{0.0f, 0.0f, 0.0f}) {
			setPointLight(i, true);
		}
		else {
			setPointLight(i, false);
		}
	}


	void setPointLight(int i, bool isOn, glm::vec3 color = {1.0f, 0.0f, 0.0f}) {
		if (isOn) {
			pointLightsInfo[i].first = color;
		}
		else {
			pointLightsInfo[i].first = { 0.0f, 0.0f, 0.0f };
		}
	}


	std::vector<bool> ballsInPlay;
	std::vector<bool> activeBumpers;
	bool isMultiball;
	bool inGame;
	bool lightsOff;

	std::vector<Vulkan::Physics::Hitbox*> balls;
	std::vector<Vulkan::Physics::Hitbox*> bumpers;
	
	std::vector<std::pair<glm::vec3&, glm::vec3&>> pointLightsInfo;
	Lights& lights;
	Vulkan::Physics::Universe& universe;
};


#endif