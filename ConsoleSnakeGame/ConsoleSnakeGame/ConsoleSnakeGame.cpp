#include <iostream>
#include <string>
#include <limits>

#include <thread>
#include <chrono>

enum Direction2D {
	UNDEFINED = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

struct Position2D
{
	int x;
	int y;
};

bool gameOver;

int width;
int height;

Position2D snakeHeadPosition;
Position2D currentFruitPosition;

Direction2D currentSnakeMovingDirection;

#pragma region Util

void ClearScreen() {
	// SLOW
	std::cout << std::string(100, '\n');
}

void ClearCinInput() {
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template <typename T>
void RequestForValidInputValue(T &receivingArg, std::string errorMsg) {
	// While the input is not of the correct type.
	while (!(std::cin >> receivingArg)) {
		std::cout << errorMsg << std::endl;
		ClearCinInput();
	}
}

void PlaceFruitRandomlyInPlayingField() {
	currentFruitPosition = { rand() % width, rand() % height };
}

#pragma endregion

void HandleLogic() {

}

void HandleInput() {

}

#pragma region Drawing_PlayField

void DrawHorizontalBorderByWidth() {
	for (int i = 0; i < width + 2; ++i) {
		std::cout << "#";
	}
	std::cout << std::endl;
}

void DrawPlayingFieldBorders() {
	
	DrawHorizontalBorderByWidth();

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (j == 0) {
				std::cout << "#";
			}

			std::cout << " ";

			if (j == width - 1) {
				std::cout << "#";
			}
		}
		std::cout << std::endl;
	}

	DrawHorizontalBorderByWidth();
}

#pragma endregion


void DrawPlayingField() {
	// system("cls");
	ClearScreen();

	DrawPlayingFieldBorders();
}

void UpdateGame() {
	DrawPlayingField();
	HandleInput();
	HandleLogic();
}

#pragma region Request_PlayField_Width_And_Height_From_User

void PrintRespectiveConfirmationTextByFirstRun(bool firstRun) {
	if (firstRun) {
		std::cout << "Confirm? (Y/N)" << std::endl;
	}
	else {
		std::cout << "Please input either 'Y' or 'N' to confirm." << std::endl;
	}
}

bool CheckIfValidConfirmationInput(std::string input) {
	return (input == "Y" || input == "N");
}

bool AskUserForConfirmation() {
	std::string userInput = "";

	bool firstRun = true;

	// Ask for input again while the input was not a confirmation.
	do {
		PrintRespectiveConfirmationTextByFirstRun(firstRun);

		RequestForValidInputValue(userInput, "ERROR: Please input 'Y' or 'N' ");
		firstRun = false;
	} while (!CheckIfValidConfirmationInput(userInput));

	return userInput == "Y";
}

bool PlayingFieldSizeIsValid() {
	return (width >= 10) && (height >= 10);
}

void RequestForValidPlayingFieldSize() {
	bool firstRun = true;

	// Keep asking for width and height while the given playing field is not of valid size.
	do {
		if (!firstRun) {
			std::cout << "The width and height of the playing field must be at least 10 or more!";
		}

		std::cout << "Enter the gamearea's width: " << std::endl;
		RequestForValidInputValue(width, "ERROR: width must be an integer!");
		std::cout << "Enter the gamearea's height: " << std::endl;
		RequestForValidInputValue(height, "ERROR: Height must be an integer!");

		firstRun = false;
	} while (!PlayingFieldSizeIsValid());
}

void RequestForInitialValues() {
	bool confirmed = false;
	do {
		RequestForValidPlayingFieldSize();

		std::cout << "Your game area will be (" << width << ", " << height << ")" << std::endl;
		confirmed = AskUserForConfirmation();

	} while (!confirmed);
}

#pragma endregion

void InitializeGame() {
	gameOver = false;
	currentSnakeMovingDirection = UNDEFINED;
	RequestForInitialValues();
	// Start in the center
	snakeHeadPosition = { (width / 2), (height / 2) };

	PlaceFruitRandomlyInPlayingField();
}

int main()
{
	InitializeGame();

	while (!gameOver) {
		DrawPlayingField();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	// Since 'system("pause")' is bad.
	std::string dummy;
	std::cout << "Input anything to continute." << std::endl;
	std::cin >> dummy;

	return 0;
}