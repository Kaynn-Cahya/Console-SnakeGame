#include <iostream>
#include <string>
#include <limits>
#include <conio.h>

#include <thread>
#include <chrono>

enum Direction2D {
	UNDEFINED = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

bool IsOppositeOf(const Direction2D& first, const Direction2D& second) {
	bool isOpposite = false;
	if (first == LEFT && second == RIGHT) {
		isOpposite = true;
	}
	else if (first == RIGHT && second == LEFT) {
		isOpposite = true;
	}
	else if (first == UP && second == DOWN) {
		isOpposite = true;
	}
	else if (first == DOWN && second == UP) {
		isOpposite = true;
	}

	return isOpposite;
}

struct Vector2D {
	int x;
	int y;
};

// Since we are printing downwards, 'up' would technically be down.
Vector2D const Up = { 0, -1 };
Vector2D const Down = { 0, 1 };

Vector2D const Left = { -1, 0 };
Vector2D const Right = { 1, 0 };

Vector2D const Zero = { 0, 0 };

#pragma region Vector2D_Operators

bool operator ==(const Vector2D& first, const Vector2D& second) {
	return (first.x == second.x) && (first.y == second.y);
}

Vector2D operator +(const Vector2D& first, const Vector2D& second) {
	return { (first.x + second.x), (first.y + second.y) };
}

Vector2D operator -(const Vector2D& first, const Vector2D& second) {
	return { (first.x - second.x), (first.y - second.y) };
}

#pragma endregion

bool gameOver;

int playerScore;

int width;
int height;

Vector2D snakeHeadPosition;
Vector2D currentFruitPosition;

Direction2D currentSnakeMovingDirection;

Vector2D tailPositions[256];

int tailLength;

#pragma region Util

void ClearCinInput() {
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template <typename T>
void RequestForValidInputValue(T &receivingArg, std::string errorMsg) {
	// Ask for input while the input is not of the correct type.
	while (!(std::cin >> receivingArg)) {
		std::cout << errorMsg << std::endl;
		ClearCinInput();
	}
}

#pragma region Request_For_YesNo

void PrintRespectiveYesNoTextByFirstRun(bool firstRun) {
	if (firstRun) {
		std::cout << "(Y/N)" << std::endl;
	}
	else {
		std::cout << "Please input either 'Y' or 'N'." << std::endl;
	}
}

bool CheckIfValidYesNoInput(std::string input) {
	return (input == "Y" || input == "N");
}

bool AskUserForYesNo() {
	std::string userInput = "";

	bool loopFirstRun = true;

	// Ask for input again while the input was not a confirmation.
	do {
		PrintRespectiveYesNoTextByFirstRun(loopFirstRun);

		RequestForValidInputValue(userInput, "ERROR: Please input 'Y' or 'N' ");
		loopFirstRun = false;
	} while (!CheckIfValidYesNoInput(userInput));

	return userInput == "Y";
}

#pragma endregion

void ClearScreen() {
	// SLOW
	std::cout << std::string(100, '\n');
}

void PlaceFruitRandomlyInPlayingField() {
	currentFruitPosition = { rand() % width, rand() % height };
}

#pragma endregion

void HandleFruitEating() {
	++tailLength;
	++playerScore;
	PlaceFruitRandomlyInPlayingField();
}

bool SnakeEatingFruit() {
	return snakeHeadPosition == currentFruitPosition;
}

bool SnakeHeadTouchedTail() {
	bool touchedTail = false;

	for (int i = 0; i < tailLength; ++i) {
		if (tailPositions[i] == snakeHeadPosition) {
			touchedTail = true;
			break;
		}
	}
	return touchedTail;
}

bool SnakeHeadTouchedBorder() {
	int x = snakeHeadPosition.x;
	int y = snakeHeadPosition.y;

	return (x > width || x < 0 || y > height || y < 0);
}

void UpdateSnakePositionByCurrentMoveDirection() {
	switch (currentSnakeMovingDirection) {
	case LEFT:
		snakeHeadPosition = snakeHeadPosition + Left;
		break;
	case RIGHT:
		snakeHeadPosition = snakeHeadPosition + Right;
		break;
	case UP:
		snakeHeadPosition = snakeHeadPosition + Up;
		break;
	case DOWN:
		snakeHeadPosition = snakeHeadPosition + Down;
		break;
	default:
		break;
	}
}

void UpdateSnakeTailPositions() {
	Vector2D prevPosition = tailPositions[0];
	Vector2D temp;
	tailPositions[0] = snakeHeadPosition;

	for (int i = 1; i < tailLength; ++i) {
		temp = tailPositions[i];
		tailPositions[i] = prevPosition;
		prevPosition = temp;
	}
}

void UpdateGameLogic() {

	UpdateSnakeTailPositions();
	UpdateSnakePositionByCurrentMoveDirection();

	if (SnakeHeadTouchedBorder() || SnakeHeadTouchedTail()) {
		gameOver = true;
	}
	else if (SnakeEatingFruit()) {
		HandleFruitEating();
	}
}

#pragma region Handling_Inputs

void MoveSnakeIfValid(Direction2D newDirection) {
	if (tailLength > 0) {
		// To prevent suicidal acts by moving backwards into a tail.
		if (!IsOppositeOf(newDirection, currentSnakeMovingDirection)) {
			currentSnakeMovingDirection = newDirection;
		}
	}
	else {
		currentSnakeMovingDirection = newDirection;
	}
}

void MoveSnakeByUserInput(char inputValue) {
	Direction2D newMoveDirection;
	switch (inputValue) {
	case 'a':
		newMoveDirection = LEFT;
		break;
	case 'd':
		newMoveDirection = RIGHT;
		break;
	case 'w':
		newMoveDirection = UP;
		break;
	case 's':
		newMoveDirection = DOWN;
		break;
	default:
		newMoveDirection = UNDEFINED;
		break;
	}

	if (newMoveDirection != UNDEFINED) {
		MoveSnakeIfValid(newMoveDirection);
	}
}

bool GetKeyPressInputIfExists(char &keyPressValue) {
	if (_kbhit()) {
		keyPressValue = _getch();
		return true;
	}
	else {
		return false;
	}
}

#pragma endregion

void HandleInput() {
	char keyPressValue;
	if (GetKeyPressInputIfExists(keyPressValue)) {
		MoveSnakeByUserInput(keyPressValue);
	}
}

#pragma region Drawing_PlayField

bool PrintTailOnPositionIfNeeded(Vector2D position) {
	bool tailPrinted = false;
	for (int i = 0; i < tailLength; ++i) {
		if (position == tailPositions[i]) {
			std::cout << "o";
			tailPrinted = true;
			break;
		}
	}
	return tailPrinted;
}

void PrintFruitOrSnakeOrEmptyByPosition(Vector2D position) {
	if (snakeHeadPosition == position) {
		std::cout << "O";
	}
	else if (currentFruitPosition == position) {
		std::cout << "X";
	}
	else if (!PrintTailOnPositionIfNeeded(position)) {
		std::cout << " ";
	}
}

void DrawHorizontalBorderByWidth() {
	for (int i = 0; i < width + 2; ++i) {
		std::cout << "#";
	}
	std::cout << std::endl;
}

void DrawMiddleSection() {
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (j == 0) {
				std::cout << "#";
			}

			PrintFruitOrSnakeOrEmptyByPosition({ j, i });

			if (j == width - 1) {
				std::cout << "#";
			}
		}
		std::cout << std::endl;
	}
}

#pragma endregion

void DrawPlayingField() {
	// system("cls");
	ClearScreen();

	// Top
	DrawHorizontalBorderByWidth();

	DrawMiddleSection();
	// Bottom
	DrawHorizontalBorderByWidth();
}

void UpdateGame() {
	DrawPlayingField();
	HandleInput();
	UpdateGameLogic();
}

#pragma region Request_PlayField_Width_And_Height_From_User

#pragma region Request_For_Valid_Play_Field_Size

bool PlayingFieldSizeIsValid() {
	return (width >= 10) && (height >= 10);
}

void RequestForValidPlayingFieldSize() {
	bool loopFirstRun = true;

	// Keep asking for width and height while the given playing field is not of valid size.
	do {
		if (!loopFirstRun) {
			std::cout << "The width and height of the playing field must be at least 10 or more!";
		}

		std::cout << "Enter the gamearea's width: " << std::endl;
		RequestForValidInputValue(width, "ERROR: width must be an integer!");
		std::cout << "Enter the gamearea's height: " << std::endl;
		RequestForValidInputValue(height, "ERROR: Height must be an integer!");

		loopFirstRun = false;
	} while (!PlayingFieldSizeIsValid());
}

#pragma endregion

void RequestForPlayingFieldSize() {
	bool confirmed = false;
	do {
		RequestForValidPlayingFieldSize();

		std::cout << "Your game area will be (" << width << ", " << height << "), Confirm?" << std::endl;
		confirmed = AskUserForYesNo();

	} while (!confirmed);
}

#pragma endregion

void ClearAndInitalizeVariables() {
	tailLength = 0;
	playerScore = 0;
	gameOver = false;
	currentSnakeMovingDirection = UNDEFINED;
}

void InitializeGame() {
	ClearScreen();
	ClearAndInitalizeVariables();

	RequestForPlayingFieldSize();
	// Start in the center
	snakeHeadPosition = { (width / 2), (height / 2) };
	tailPositions[0] = snakeHeadPosition;

	PlaceFruitRandomlyInPlayingField();
}

int main() {
	// Play the game while the user wants to play again.
	do {
		InitializeGame();

		while (!gameOver) {
			UpdateGame();
			std::this_thread::sleep_for(std::chrono::milliseconds(400));
		}

		std::cout << "GAMEOVER, Your Score: " << playerScore << std::endl;

		std::cout << "Play again? ";

	} while (AskUserForYesNo());

	return 0;
}