#include <SFML/Graphics.hpp>
#include <iostream>
#include<ctime>
#include<cstdlib>
#include<algorithm>
#include<pthread.h>
#include<cmath>
#include<string>

using namespace std;

struct GameData {

	int matrixSize;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
    int GRID_SIZE;
	float spriteSizeRatio;
	int noOfSprites; 
    int player1Score;
    int player2Score;
    sf::RectangleShape** matrix;
    sf::Texture textureA;
    sf::Texture textureB;
    sf::Texture textureC;
    vector<sf::Sprite> sprites;
    sf::Sprite spriteB;    
    sf::Sprite spriteC; 
    sf::Sprite sprite;
    sf::Font font;
    sf::Text scoreText;
    sf::Text scoreText1;
    
};


	
int BoardSize() {

    // Seed the random number generator with the current time
    srand(time(0));
    // Generate a random number between 10 and 99 inclusive
    int random =  rand() % 90 + 10;
    random *= 9;
    int number = 1379 / random;
    number %= 25;
    if (number < 10)
        number += 15;
    return number;

}

void generatingScoreBoard(GameData& obj) {

    // Initialize the matrix with rectangles
    int xOffset = (obj.SCREEN_WIDTH - obj.matrixSize * obj.GRID_SIZE) / 2;
    int yOffset = (obj.SCREEN_HEIGHT - obj.matrixSize * obj.GRID_SIZE) / 2;
    obj.matrix = new sf::RectangleShape*[obj.matrixSize];
    for (int i = 0; i < obj.matrixSize; i++) {
        obj.matrix[i] = new sf::RectangleShape[obj.matrixSize];
        for (int j = 0; j < obj.matrixSize; j++) {
            sf::RectangleShape rectangle(sf::Vector2f(obj.GRID_SIZE, obj.GRID_SIZE));
            rectangle.setPosition(xOffset + j * obj.GRID_SIZE, yOffset + i * obj.GRID_SIZE);
            rectangle.setFillColor(sf::Color::Transparent);
            rectangle.setOutlineThickness(1);
            rectangle.setOutlineColor(sf::Color::Black);
            obj.matrix[i][j] = rectangle;
        }
    }
}


void* moveSpriteC(void* arg) {
    
    GameData* obj = static_cast<GameData*>(arg);
    std::vector<sf::Vector2f> savedPositions; // vector to store the positions of all sprites except spriteB
    while (true) {
        // Check for collisions between sprites
        bool collisionDetected = false;
        std::vector<size_t> indicesToRemove;
        
        // Check if all sprites have been collected by spriteB
        if (obj->sprites.size() == 0) {
            // End game if all sprites have been collected
            return NULL;
        }
        
        // Save the positions of all other sprites
        savedPositions.clear();
        for (size_t i = 0; i < obj->sprites.size(); i++) {
                savedPositions.push_back(obj->sprites[i].getPosition());
            }
        
        
        // Check if spriteB collides with any other sprite
        int count = 0;
        for (size_t i = 0; i < savedPositions.size(); i++) {
            if (obj->spriteC.getPosition() == savedPositions[i]) {
                // Add the index of the sprite to be removed
                indicesToRemove.push_back(i);
                collisionDetected = true;
                count++;
            }
        }
        
        // Remove all sprites with matching positions
        if (collisionDetected) {
            for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
                obj->sprites.erase(obj->sprites.begin() + *it);
            }
            obj->player2Score += 5;
            obj->scoreText1.setString("Player 2 score: " + std::to_string(obj->player2Score));
        }
        
        // Check if all sprites have been collected by spriteB
        if (count == savedPositions.size()) {
            // End game if all sprites have been collected
            return NULL;
        }

        // Move spriteB to a random adjacent cell
        int currentPosI, currentPosJ;
        for (int i = 0; i < obj->matrixSize; i++) {
            for (int j = 0; j < obj->matrixSize; j++) {
                if (obj->matrix[i][j].getPosition() == obj->spriteC.getPosition()) {
                    currentPosI = i;
                    currentPosJ = j;
                    break;
                }
            }
        }              
        int rand_i = currentPosI + (rand() % 3 - 1); // generate a random value between -1 and 1
        int rand_j = currentPosJ + (rand() % 3 - 1); // generate a random value between -1 and 1
        // check if the new position is within the bounds of the matrix
        if (rand_i >= 0 && rand_i < obj->matrixSize && rand_j >= 0 && rand_j < obj->matrixSize) {
            // check if the new position is adjacent to the current position and not occupied by another sprite
            if (abs(rand_i - currentPosI) <= 1 && abs(rand_j - currentPosJ) <= 1 &&
                obj->matrix[rand_i][rand_j].getFillColor() == sf::Color::Transparent) {
                // remove the sprite from its previous position
                obj->matrix[currentPosI][currentPosJ].setFillColor(sf::Color::Transparent);
                // set the new position of spriteB
                obj->spriteC.setTexture(obj->textureC);
                obj->spriteC.setScale(sf::Vector2f(obj->spriteSizeRatio, obj->spriteSizeRatio));
                obj->spriteC.setPosition(obj->matrix[rand_i][rand_j].getPosition());
                obj->sprites.push_back(obj->spriteC);
            } 
        }
        sf::sleep(sf::seconds(1)); // pause the thread for 1 second
       // if(obj->player1Score > 100) {
	    //   	exit(0);
    	//}
    }
    return NULL;
}

void* moveSpriteB(void* arg) {
    
    GameData* obj = static_cast<GameData*>(arg);
    std::vector<sf::Vector2f> savedPositions; // vector to store the positions of all sprites except spriteB
    while (true) {
        // Check for collisions between sprites
        bool collisionDetected = false;
        std::vector<size_t> indicesToRemove;
        
        // Check if all sprites have been collected by spriteB
        if (obj->sprites.size() == 0) {
            // End game if all sprites have been collected
            return NULL;
        }
        
        // Save the positions of all other sprites
        savedPositions.clear();
        for (size_t i = 0; i < obj->sprites.size(); i++) {
                savedPositions.push_back(obj->sprites[i].getPosition());
            }
        
        
        // Check if spriteB collides with any other sprite
        int count = 0;
        for (size_t i = 0; i < savedPositions.size(); i++) {
            if (obj->spriteB.getPosition() == savedPositions[i]) {
                // Add the index of the sprite to be removed
                indicesToRemove.push_back(i);
                collisionDetected = true;
                count++;
            }
        }
        
        // Remove all sprites with matching positions
        if (collisionDetected) {
            for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
                obj->sprites.erase(obj->sprites.begin() + *it);
            }
            obj->player1Score += 5;
            obj->scoreText.setString("Player 1 score: " + std::to_string(obj->player1Score));
        }
        
        // Check if all sprites have been collected by spriteB
        if (count == savedPositions.size()) {
            // End game if all sprites have been collected
            return NULL;
        }

        // Move spriteB to a random adjacent cell
        int currentPosI, currentPosJ;
        for (int i = 0; i < obj->matrixSize; i++) {
            for (int j = 0; j < obj->matrixSize; j++) {
                if (obj->matrix[i][j].getPosition() == obj->spriteB.getPosition()) {
                    currentPosI = i;
                    currentPosJ = j;
                    break;
                }
            }
        }              
        int rand_i = currentPosI + (rand() % 3 - 1); // generate a random value between -1 and 1
        int rand_j = currentPosJ + (rand() % 3 - 1); // generate a random value between -1 and 1
        // check if the new position is within the bounds of the matrix
        if (rand_i >= 0 && rand_i < obj->matrixSize && rand_j >= 0 && rand_j < obj->matrixSize) {
            // check if the new position is adjacent to the current position and not occupied by another sprite
            if (abs(rand_i - currentPosI) <= 1 && abs(rand_j - currentPosJ) <= 1 &&
                obj->matrix[rand_i][rand_j].getFillColor() == sf::Color::Transparent) {
                // remove the sprite from its previous position
                obj->matrix[currentPosI][currentPosJ].setFillColor(sf::Color::Transparent);
                // set the new position of spriteB
                obj->spriteB.setTexture(obj->textureB);
                obj->spriteB.setScale(sf::Vector2f(obj->spriteSizeRatio, obj->spriteSizeRatio));
                obj->spriteB.setPosition(obj->matrix[rand_i][rand_j].getPosition());
                obj->sprites.push_back(obj->spriteB);
            } 
        }
        sf::sleep(sf::seconds(1)); // pause the thread for 1 second
       // if(obj->player1Score > 100) {
	    //   	exit(0);
    	//}
    }
    return NULL;
}




int main() {

	GameData obj;
	cout << obj.sprites.size();
	obj.player1Score = 0;
	obj.player2Score = 0;
	obj.matrixSize = 10;//BoardSize(); 
    obj.SCREEN_WIDTH= 800;
    obj.SCREEN_HEIGHT = 600;
    obj.GRID_SIZE = obj.SCREEN_WIDTH / obj.matrixSize / 2;
    sf::RenderWindow window(sf::VideoMode(obj.SCREEN_WIDTH, obj.SCREEN_HEIGHT), "ABUBAKER :]");
    obj.matrix = nullptr;
  
    generatingScoreBoard(obj);
	
   
    if (!(obj.textureA.loadFromFile("a.png"))) {
        std::cout << "Couldn't Load Texture a.png" << std::endl;
        return 0;
    }

    if (!(obj.textureB.loadFromFile("b.png"))) {
        std::cout << "Couldn't Load Texture b.png For Player1" << std::endl;
        return 0;
    }
    
       
    if (!(obj.textureC.loadFromFile("b.png"))) {
        std::cout << "Couldn't Load Texture b.png For Player2" << std::endl;
        return 0;
    }
    
    // Set the scale of the sprites to fit inside the box
    obj.spriteSizeRatio = static_cast<float>(obj.GRID_SIZE) / obj.textureA.getSize().x;


   	// Create a Player 1  sprite
	obj.spriteB.setTexture(obj.textureB);
	obj.spriteB.setScale(sf::Vector2f(obj.spriteSizeRatio, obj.spriteSizeRatio));
	obj.spriteB.setPosition(obj.matrix[obj.matrixSize - 1][obj.matrixSize / 2].getPosition());

	
	// Create a Player 2  sprite
	obj.spriteC.setTexture(obj.textureC);
	obj.spriteC.setScale(sf::Vector2f(obj.spriteSizeRatio, obj.spriteSizeRatio));
	obj.spriteC.setPosition(obj.matrix[0][obj.matrixSize / 2].getPosition());
	
	
    srand(time(NULL)); // seed the random number generator with the current time
    obj.noOfSprites = obj.matrixSize / 2 + 15;
    for (int i = 0; i < obj.noOfSprites; i++) { // Generate half sprites of matrix Size
        int random_i = rand() % obj.matrixSize;
        int random_j = rand() % obj.matrixSize;

        // Check if a sprite already exists at this position
        bool exists = false;
        for (const auto& sprite : obj.sprites) {
            if (obj.sprite.getPosition() == obj.matrix[random_i][random_j].getPosition() || obj.spriteB.getPosition() == obj.matrix[random_i][random_j].getPosition() ) {
                exists = true;
                break;
            }
        }
        // If a sprite doesn't exist, create one at this position
        if (!exists) {
			// If a sprite doesn't exist, create one at this position
			obj.sprite.setTexture(obj.textureA);
			obj.sprite.setScale(sf::Vector2f(obj.spriteSizeRatio, obj.spriteSizeRatio));
			obj.sprite.setPosition(obj.matrix[random_i][random_j].getPosition());
			obj.sprites.push_back(obj.sprite);
			}
		}
		
	pthread_t threadId;
   	pthread_create(&threadId, NULL, moveSpriteB, &obj);
   	pthread_t threadId1;
	pthread_create(&threadId1, NULL, moveSpriteC, &obj);

	 // Create font object and load font file
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf")) {
        return EXIT_FAILURE;
    }

    // Create text object and set its properties
    sf::Text text;
    text.setFont(font);
    text.setString("Abubaker:]");
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Blue);
    text.setStyle(sf::Text::Bold);
    text.setPosition(370.f, 55.f);
	
	sf::Text text1;
    text1.setFont(font);
    text1.setString("Name: ");
    text1.setCharacterSize(20);
    text1.setFillColor(sf::Color::Black);
    text1.setStyle(sf::Text::Bold);
    text1.setPosition(300.f, 55.f);
  
  	obj.scoreText.setFont(font);
  	obj.scoreText.setCharacterSize(20);
  	obj.scoreText.setFillColor(sf::Color::Black);
	obj.scoreText.setStyle(sf::Text::Bold);
	obj.scoreText.setPosition(400.f, 75.f);
	  
  	 obj.scoreText1.setFont(font);
  	 obj.scoreText1.setCharacterSize(20);
  	 obj.scoreText1.setFillColor(sf::Color::Black);
  	 obj.scoreText1.setStyle(sf::Text::Bold);
     obj.scoreText1.setPosition(200.f, 75.f);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
		    if (event.type == sf::Event::Closed) {
		        window.close();
		    }
		}
		
		// Draw the matrix
		for (int i = 0; i < obj.matrixSize; i++) {
		    for (int j = 0; j < obj.matrixSize; j++) {
		        window.draw(obj.matrix[i][j]);
		    }
		}
		window.draw(obj.spriteC);

		// Draw the sprites
		for (const auto& sprite : obj.sprites) {
		    window.draw(sprite);
		}
		

		// Draw text
		window.draw(text1);
        window.draw(text);
        //drawing player1 Score
        window.draw(obj.scoreText);
		//drawing player2 Score
        window.draw(obj.scoreText1);
        if(obj.player1Score > 200 && obj.player1Score > obj.player2Score) {
        	
        	cout << endl << "Player1 Win" << endl;
        	exit(0);
        }
        else if(obj.player2Score > 200 && obj.player2Score > obj.player1Score) {
        	cout << endl << "Player2 Win" << endl;
        	exit(0);
        }
		window.display();
		window.clear(sf::Color::White);
}

	// Check for collisions between sprite and spriteB
	pthread_detach(threadId);

	return 0;
	
}
