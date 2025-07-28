#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <fstream>
#include "sqlite3.h"

using namespace std;
using namespace sf;


int callbackVectorPair(void* Used, int argc, char** argv, char** azColName)
{
	std::vector<std::pair<std::string, std::string>>* data = (std::vector< std::pair<std::string, std::string>>*)Used;
	for (int i = 0; i < argc; i++)
		data->emplace_back(azColName[i], argv[i]);
	return 0;
}


int selectID(){
    sqlite3* DB;
	std::vector<std::pair<std::string , std::string>> data;
	char* messageError;
	std::string sql = "SELECT MAX(id) FROM Scores;";
	
	data.clear();
	int exit = sqlite3_open("resources/data.db", &DB);
	
	exit = sqlite3_exec(DB, sql.c_str(), callbackVectorPair, &data, &messageError);

	if (exit != SQLITE_OK) {
		std::cerr << "Error selecting data: " << messageError << std::endl;
		sqlite3_free(messageError);
	}
	else
		std::cout << "Data selected successfully!" << std::endl;
	sqlite3_close(DB);

    return stoi(data[0].second);
}


void saveProgress(int score, string howquit, int levels){
    int id = selectID() + 1;
    sqlite3* DB;
	char* messageError;

	std::string sql = "INSERT INTO Scores VALUES(" + to_string(id) + ", " + to_string(score) + ", '" + howquit + "', " + to_string(levels) + ");";

	int exit = sqlite3_open("resources/data.db", &DB);

	exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
	if (exit != SQLITE_OK) {
		std::cerr << "Error inserting records: " << messageError << std::endl;
		sqlite3_free(messageError);
	}
	else
		std::cout << "Data inserted successfully" << std::endl;

	sqlite3_close(DB);
}


void updateMusicSound(Music *music, Sound *startSound, Sound *explodeSound, Sound *flyAlienSound, Sound *shotSound, float *volume, float *volumeMusic, float *volumeSound){
    (*music).setVolume(100*(*volume)*(*volumeMusic));
    (*startSound).setVolume(100*(*volume)*(*volumeMusic));
    (*explodeSound).setVolume(100*(*volume)*(*volumeSound));
    (*flyAlienSound).setVolume(100*(*volume)*(*volumeSound));
    (*shotSound).setVolume(100*(*volume)*(*volumeSound));
}


void createBackground(vector<RectangleShape> *background, vector<int> *backgroundDelay){
    RectangleShape blick(Vector2f(5.f, 5.f));
    Color colors[4] = {Color::White, Color::Green, Color::Red, Color(206, 138, 255)}; // Цвета для рандомных точек на фоне
    for (int i=0; i<20; i++){
        blick.setFillColor(colors[rand()%4]);
        blick.setPosition(Vector2f(rand()%820, 30*i));
        (*background).push_back(blick);
        (*backgroundDelay).push_back(rand()%20);
    }
}


void checkSideAlliens(vector<RectangleShape> *alliens, vector<int> *sideAlliens){
    (*sideAlliens).clear();
    if ((*alliens)[0].getFillColor() != Color::Black){
        (*sideAlliens).push_back(0);
    }
    if ((*alliens).size() > 1){
    int lastY=(*alliens)[0].getPosition().y;
    for (int i=1; i<(*alliens).size()-1; i++){
        if ((*alliens)[i].getPosition().y != lastY){
            lastY = (*alliens)[i].getPosition().y;
            if ((*alliens)[i-1].getFillColor() != Color::Black){
                (*sideAlliens).push_back(i-1);
            }
            if ((*alliens)[i].getFillColor() != Color::Black){
                (*sideAlliens).push_back(i);
            }
        }
    }
    if ((*alliens)[(*alliens).size()-1].getFillColor() != Color::Black){
        (*sideAlliens).push_back((*alliens).size()-1);
    }
    }

}


void createAliens(vector<RectangleShape> *aliens, Texture *alien_small, Texture *alien_mid, Texture *alien_big, Texture *alien_biggest, vector<int> *curAnimations){
    (*aliens).clear();
    (*curAnimations).clear();
    (*curAnimations) = {
        0, 10, 20, 30, 20, 10, 0, 10, 20, 30, 
        0, 10, 20, 30, 20, 10, 0, 10, 20, 30, 
        0, 10, 20, 30, 20, 10, 0, 10, 20, 30, 
           10, 20, 30, 20, 10, 0, 10, 20, 
               20, 30, 20, 10, 0, 10};
    RectangleShape alien_sm(Vector2f(42.f, 25.f));
    alien_sm.setTexture(alien_small); // Создание образца для маленького инопланетянина, то, что не зависит от i, j.
    
    RectangleShape alien_mi(Vector2f(42.2f, 25.f));
    alien_mi.setTexture(alien_mid);// Создание образца для среднего инопланетянина, то, что не зависит от i.

    RectangleShape alien_bi(Vector2f(42.2f, 25.f));
    alien_bi.setTexture(alien_big);// Создание образца для большого инопланетянина, то, что не зависит от i.

    RectangleShape alien_bigg(Vector2f(42.2f, 42.f));
    alien_bigg.setTexture(alien_biggest);// Создание образца для самого большого инопланетянина, общее для обоих.

    for (int i=0; i<3; i++){ // 3 строки инопланетян(small)
        for (int j=0; j<10; j++){ // 10 инопланетян в строке(small)
            alien_sm.setPosition(Vector2f((175+46*j), (205+35*i)));
            (*aliens).push_back(alien_sm);
        } 
    }
    for (int i=0; i<8; i++){ // 8 инопланетян(mid)
        alien_mi.setPosition(Vector2f(221+46*i, 170));
        (*aliens).push_back(alien_mi);
    }
    for (int i=0; i<6; i++){ // 6 инопланетян(big)
        alien_bi.setPosition(Vector2f(267+46*i, 135));
        (*aliens).push_back(alien_bi);
    }

    alien_bigg.setPosition(Vector2f(311.f, 95.f)); // 2 инопланетянина(biggest)
    (*aliens).push_back(alien_bigg);
    alien_bigg.setPosition(Vector2f(449.f, 95.f));
    (*aliens).push_back(alien_bigg);

}
    


int main(){
    RenderWindow window(VideoMode(825, 625), "Dendy Galaxian"); // Создание окна 825*625
    window.setFramerateLimit(60);
    string gameMode = "start"; // Режим - старт
    string prevmode;
    Font font; // Создание шрифта
    font.loadFromFile("resources/font.ttf"); // Выгрузка шрифта из файла
    int score_1=0; // Счётчик баллов одного пользователя
    int scoresBiggestRandom[] = {150, 200, 300, 800};
    int scoreBiggestFromRand;
    bool isDeath=false;
    bool isIntersect = false;
    int deathFrames;
    int curLevel=1;
    int whereScoreBiggest;
    float volume=1;
    float volumeMusic=1;
    float volumeSound=1;
    float vx_rocketShip = 0.f; // Горизонтальная скорость ракеты
    time_t currTime; // Текущее время
    time_t lastEnterGameNoPause; 
    time_t lastPause; 
    time_t timeInMenu;
    time_t startTime = time(nullptr); // Время начала игры
    time_t lastFlight = time(nullptr); // Время последнего вылета инопланетян
    int delayFlight = 6; // Задержка между вылетами инопланетян
    bool isPaused = false;
    bool isLoos = false;
    int speedAnimate = 30;
    vector<int> curAnimations = {
        0, 10, 20, 30, 20, 10, 0, 10, 20, 30, 
        0, 10, 20, 30, 20, 10, 0, 10, 20, 30, 
        0, 10, 20, 30, 20, 10, 0, 10, 20, 30, 
           10, 20, 30, 20, 10, 0, 10, 20, 
               20, 30, 20, 10, 0, 10};
    ifstream in("resources/score.txt");
    string maxScoreFile;
    getline(in, maxScoreFile);
    in.close();


    // Музыка
    SoundBuffer shotBuffer;
    if (!shotBuffer.loadFromFile("resources/sounds/shot.mp3"))
        return -1;
    Sound shotSound;
    shotSound.setBuffer(shotBuffer);

    SoundBuffer flyAlienBuffer;
    if (!flyAlienBuffer.loadFromFile("resources/sounds/flyAlien.mp3"))
        return -1;
    Sound flyAlienSound;
    flyAlienSound.setBuffer(flyAlienBuffer);

    SoundBuffer explodeBuffer;
    if (!explodeBuffer.loadFromFile("resources/sounds/explode.mp3"))
        return -1;
    Sound explodeSound;
    explodeSound.setBuffer(explodeBuffer);

    SoundBuffer startBuffer;
    if (!startBuffer.loadFromFile("resources/sounds/starting.mp3"))
        return -1;
    Sound startSound;
    startSound.setBuffer(startBuffer);

    Music music;
    if (!music.openFromFile("resources/sounds/maintheme.mp3"))
        return -1; // error


    // Общее
    Text player_1(String("1UP"), font); // Текст 1UP Сверху слева
    player_1.setFillColor(Color(255, 0, 0));
    player_1.setPosition(Vector2f(140.f, 30.f));
    Text player_1_score(String("0"), font); // Счётчик баллов на экране для 1 игрока
    player_1_score.setFillColor(Color(255, 255, 255));
    player_1_score.setPosition(Vector2f(165.f, 62.f));

    Text company(String("NAMVLAD"), font); // Текст NAMVLAD
    company.setFillColor(Color(255, 0, 0));
    company.setPosition(Vector2f(285.f, 440.f));
    Text companyRights(String("2025 NAMVLAD LTD.\nALL RIGHTS RESERVED"), font); // Текст NAMVLAD
    companyRights.setPosition(Vector2f(125.f, 515.f));

    Text high_score(String("HI-SCORE"), font); // Текст 1UP Сверху слева
    high_score.setFillColor(Color(255, 0, 0));
    high_score.setPosition(Vector2f(290.f, 30.f));

    Text gameover(String("GAME OVER"), font); 
    gameover.setFillColor(Color(255, 255, 255));
    gameover.setPosition(Vector2f(290.f, 400.f));

    // БД макс. рекорд
    sqlite3* DB;
	std::vector<std::pair<std::string , std::string>> data;
	char* messageError;
	std::string sql = "SELECT MAX(score) FROM Scores;";
	
	data.clear();
	int exit = sqlite3_open("resources/data.db", &DB);
	
	exit = sqlite3_exec(DB, sql.c_str(), callbackVectorPair, &data, &messageError);

	if (exit != SQLITE_OK) {
		std::cerr << "Error selecting data: " << messageError << std::endl;
		sqlite3_free(messageError);
	}
	else
		std::cout << "Data selected successfully!" << std::endl;

	sqlite3_close(DB);
    
    
    Text high_score_value(String("0"), font); // Счётчик баллов на экране для 1 игрока
    string high_score_value_str = high_score_value.getString();
    if (stoi(maxScoreFile) > stoi(high_score_value_str)){
        high_score_value.setString(maxScoreFile);
    }
    if (stoi(data[0].second) > stoi((string)high_score_value.getString())){
        high_score_value.setString(data[0].second);
    }
    high_score_value.setFillColor(Color(0, 0, 255));
    high_score_value.setPosition(Vector2f(315.f, 62.f));
    string highest_score;


    // Старт
    RectangleShape logo(Vector2f(460.f, 124.f)); // Логотип
    RectangleShape lifeCount(Vector2f(30.f, 30.f)); 
    logo.setPosition(Vector2f(172.f, 150.f));

    Texture logo_pic;
    logo_pic.loadFromFile("resources/textures/logo.png"); // Картинка для лого
    logo.setTexture(&logo_pic);

    Text players(String("1 player"), font); // Текст 1 Player
    players.setPosition(Vector2f(310.f, 340.f));
    players.setFillColor(Color(206, 138, 255));

    Text settings(String("SETTINGS"), font); 
    settings.setFillColor(Color(206, 138, 255));
    settings.setPosition(Vector2f(310.f, 390.f));

    CircleShape pickTriangle(10.f, 3); // Треугольник текущего выбора
    pickTriangle.setFillColor(Color(255, 255, 255));
    pickTriangle.rotate(90);
    pickTriangle.setPosition(Vector2f(305.f, 350.f));


    // Настройки
    Text setvolumemain(String("VOLUME"), font); 
    setvolumemain.setFillColor(Color(206, 138, 255));
    setvolumemain.setPosition(Vector2f(100.f, 150.f));

    RectangleShape mainvolumeback(Vector2f(700.f, 50.f));
    mainvolumeback.setFillColor(Color(150, 150, 150));
    mainvolumeback.setPosition(Vector2f(75.f, 200.f));

    RectangleShape mainvolume(Vector2f(700.f, 50.f));
    mainvolume.setFillColor(Color::White);
    mainvolume.setPosition(Vector2f(75.f, 200.f));

    CircleShape pickTriangleMusic(10.f, 3); // Треугольник текущего выбора музыки
    pickTriangleMusic.setFillColor(Color(255, 255, 255));
    pickTriangleMusic.rotate(90);
    pickTriangleMusic.setPosition(Vector2f(80.f, 155.f));
    
    Text setvolumemusic(String("MUSIC VOLUME"), font); 
    setvolumemusic.setFillColor(Color(206, 138, 255));
    setvolumemusic.setPosition(Vector2f(100.f, 300.f));

    RectangleShape musicvolumeback(Vector2f(700.f, 50.f));
    musicvolumeback.setFillColor(Color(150, 150, 150));
    musicvolumeback.setPosition(Vector2f(75.f, 350.f));

    RectangleShape musicvolume(Vector2f(700.f, 50.f));
    musicvolume.setFillColor(Color::White);
    musicvolume.setPosition(Vector2f(75.f, 350.f));
    
    Text setvolumesound(String("SOUND VOLUME"), font); 
    setvolumesound.setFillColor(Color(206, 138, 255));
    setvolumesound.setPosition(Vector2f(100.f, 450.f));

    RectangleShape soundvolumeback(Vector2f(700.f, 50.f));
    soundvolumeback.setFillColor(Color(150, 150, 150));
    soundvolumeback.setPosition(Vector2f(75.f, 500.f));

    RectangleShape soundvolume(Vector2f(700.f, 50.f));
    soundvolume.setFillColor(Color::White);
    soundvolume.setPosition(Vector2f(75.f, 500.f));


    
    // Игра
    RectangleShape rocketShip(Vector2f(50.f, 70.f)); // Ракета
    IntRect rocketCollide;
    IntRect alie;
    IntRect bull;
    RectangleShape boom(Vector2f(48.f, 52.f));
    RectangleShape boomStatic(Vector2f(48.f, 52.f));
    rocketShip.setPosition(Vector2f(390.f, 550.f));
    Texture rocketShipImg; // Картинка для ракеты
    rocketShipImg.loadFromFile("resources/textures/rocketShip.png");
    rocketShip.setTexture(&rocketShipImg);
    vector<RectangleShape> bullets; // Список всех пуль(вектор)
    RectangleShape bullet(Vector2f(3.f, 15.f));
    bullet.setFillColor(Color(255, 154, 57));
    vector<RectangleShape> alienBullets;
    RectangleShape alienBullet(Vector2f(3.f, 15.f));
    alienBullet.setFillColor(Color(255, 255, 255));
    vector<RectangleShape> aliens; // Список всех инопланетян(вектор)
    vector<RectangleShape> flyAliens; // Список всех летящих инопланетян(вектор)
    vector<RectangleShape> background; // Список точек заднего фона(вектор)
    vector<RectangleShape> booms; 
    vector<RectangleShape> boomsStatic; 
    vector<RectangleShape> lifesCounter; 
    vector<RectangleShape> currentLevelCount;
    vector<Text> scoresBiggest; 
    vector<int> sideAliens;
    vector<int> sideAliensRedIndexes;
    vector<int> flyAliensCurrSpeed;
    vector<int> flyAliensIndexes;
    vector<Vector2f> flyAliensSpeeds;
    vector<const Texture *> flyAliensPrevTexture; 
    int randIndex;
    vector<RectangleShape *> startScreenObj; // Список всех объектов для анимации вначале игры(вектор)
    vector<Text *> startScreenText; // Список всех текстов для анимации вначале игры(вектор)
    vector<int> backgroundDelay; // Список задержек отображения точек заднего фона(вектор)
    vector<int> boomDelay; 
    vector<int> boomStaticDelay;
    Vector2f forAliens;
    RectangleShape flagLevel(Vector2f(30.f, 40.f));
    flagLevel.setPosition(Vector2f(640.f, 45.f)); 
    Texture alien_small, alien_mid, alien_big, alien_biggest; // Текстуры инопланетян
    Texture alien_smalldown, alien_smallup, alien_middown, alien_midup, alien_bigdown, alien_bigup;
    Texture boom1, boom2, boom3;
    Texture life, levelsFlag;
    alien_smalldown.loadFromFile("resources/textures/alien_smalldown.png");
    alien_smallup.loadFromFile("resources/textures/alien_smallup.png");
    alien_middown.loadFromFile("resources/textures/alien_middown.png");
    alien_midup.loadFromFile("resources/textures/alien_midup.png");
    alien_bigdown.loadFromFile("resources/textures/alien_bigdown.png");
    alien_bigup.loadFromFile("resources/textures/alien_bigup.png");
    alien_small.loadFromFile("resources/textures/alien_small.png"); // Текстура маленького инопланетянина 42*25
    alien_mid.loadFromFile("resources/textures/alien_mid.png"); // Текстура среднего инопланетянина 42*25
    alien_big.loadFromFile("resources/textures/alien_big.png"); // Текстура большого инопланетянина 42*25
    alien_biggest.loadFromFile("resources/textures/alien_biggest.png"); // Текстура самого большого инопланетянина 42*42
    boom1.loadFromFile("resources/textures/boom1.png");
    boom2.loadFromFile("resources/textures/boom2.png");
    boom3.loadFromFile("resources/textures/boom3.png");
    life.loadFromFile("resources/textures/life.png");
    lifeCount.setTexture(&life);
    levelsFlag.loadFromFile("resources/textures/flag.png");
    flagLevel.setTexture(&levelsFlag);
    currentLevelCount.push_back(flagLevel);
    createBackground(&background, &backgroundDelay); // Заполняем фон
    Text death(String("READY"), font);
    death.setFillColor(Color(255, 0, 0));
    death.setPosition(Vector2f(340.f, 430.f));
    Text pause(String("PAUSE"), font);
    pause.setFillColor(Color(255, 0, 0));
    pause.setPosition(Vector2f(340.f, 430.f));
    Text bigLevelCount(String("06"), font); 
    bigLevelCount.setPosition(Vector2f(675.f, 55.f));
    Text scoreBiggest(String("150"), font);
    vector<int> connectBoomScore;
    
    // Анимация входа
    startScreenText.push_back(&player_1_score);
    startScreenText.push_back(&company);
    startScreenText.push_back(&companyRights);
    startScreenObj.push_back(&logo);
    startScreenText.push_back(&players);
    startScreenText.push_back(&high_score);
    startScreenText.push_back(&high_score_value);
    startScreenText.push_back(&settings);
    startScreenText.push_back(&player_1);
    // startScreenObj.push_back(pickTriangle); Не забываем про него, но он - CircleShape
    pickTriangle.move(Vector2f(0.f, 700.f));
    for (int i=0; i<startScreenObj.size(); i++){
        (*(startScreenObj[i])).move(Vector2f(0.f, 700.f));
    }
    for (int i=0; i<startScreenText.size(); i++){
        (*(startScreenText[i])).move(Vector2f(0.f, 700.f));
    }
    
    window.setVerticalSyncEnabled(true); // VSync 
    while (window.isOpen()){
        Event event; // Текущее событие
        currTime = time(nullptr); // Текущее время
        
        while (window.pollEvent(event)){ // Прописываем кнопки
            if (event.type == Event::Closed){
                if (gameMode == "game"){
                    saveProgress(score_1, "exit", curLevel);
                }
                window.close(); // Кнопка закрытия окна
            }
            if (event.type == Event::KeyPressed){ // Если нажата некая кнопка, то в зависимости от режима разные кнопки
                if (gameMode == "start"){ // Если сейчас только старт
                    if (Keyboard::isKeyPressed(Keyboard::Enter)){ // Если нажат Enter
                        if (player_1.getPosition().y > 30){
                            pickTriangle.move(Vector2f(0.f, 30.0-player_1.getPosition().y));
                            for (int i=0; i<startScreenObj.size(); i++){
                                (*(startScreenObj[i])).move(Vector2f(0.f, 30.0-player_1.getPosition().y));
                            }
                            for (int i=0; i<startScreenText.size(); i++){
                                (*(startScreenText[i])).move(Vector2f(0.f, 30.0-player_1.getPosition().y));
                            }
                        } else if (pickTriangle.getPosition().y == 350){
                            score_1=0;
                            lastEnterGameNoPause = currTime;
                            isPaused = false;
                            lastFlight = currTime;
                            startSound.play();
                            startSound.setVolume(10);
                            music.play();
                            gameMode = "game";
                            music.setVolume(10);
                            createAliens(&aliens, &alien_small, &alien_mid, &alien_big, &alien_biggest, &curAnimations);
                            for (int i=0; i<2; i++){
                                lifeCount.setPosition(Vector2f(540.0+30.0*i, 60.f));
                                lifesCounter.push_back(lifeCount);
                            }
                        } else {
                            prevmode = gameMode;
                            gameMode = "settings";
                        }
                    }
                    if ((Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::Down)) && player_1.getPosition().y == 30){
                        if (pickTriangle.getPosition().y == 350){
                            pickTriangle.move(Vector2f(0.f, 50.f));
                        } else {
                            pickTriangle.move(Vector2f(0.f, -50.f));
                        }
                    }
                }
                if (gameMode == "settings"){
                    if (Keyboard::isKeyPressed(Keyboard::Escape)){
                        if (prevmode == "game"){
                            lastFlight += time(nullptr) - timeInMenu;
                        }
                        gameMode = prevmode;
                        if (startSound.getStatus() == Sound::Status::Paused){
                            startSound.play();
                        }
                        if (music.getStatus() == Sound::Status::Paused){
                            music.play();
                        }
                        continue;
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)){
                        if (pickTriangleMusic.getPosition().y < 400){
                            pickTriangleMusic.move(Vector2f(0.f, 150.f));
                        } else {
                            pickTriangleMusic.setPosition(Vector2f(80.f, 155.f));
                        }
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)){
                        if (pickTriangleMusic.getPosition().y > 200){
                            pickTriangleMusic.move(Vector2f(0.f, -150.f));
                        } else {
                            pickTriangleMusic.setPosition(Vector2f(80.f, 455.f));
                        }
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)){
                        if (pickTriangleMusic.getPosition().y == 155){
                            if (volume < 1){
                                volume += 0.01;
                                volume = round(volume*100)/100;
                            }
                        } else if (pickTriangleMusic.getPosition().y == 305){
                            if (volumeMusic < 1){
                                volumeMusic += 0.01;
                                volumeMusic = round(volumeMusic*100)/100;
                            }
                        } else {
                            if (volumeSound < 1){
                                volumeSound += 0.01;
                                volumeSound = round(volumeSound*100)/100;
                            }
                        }
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)){
                        if (pickTriangleMusic.getPosition().y == 155){
                            if (volume > 0){
                                volume -= 0.01;
                                volume = round(volume*100)/100;
                            }
                        } else if (pickTriangleMusic.getPosition().y == 305){
                            if (volumeMusic > 0){
                                volumeMusic -= 0.01;
                                volumeMusic = round(volumeMusic*100)/100;
                            }
                        } else {
                            if (volumeSound > 0){
                                volumeSound -= 0.01;
                                volumeSound = round(volumeSound*100)/100;
                            }
                        }
                    }
                }
                if (gameMode == "game" && isLoos){
                    if (Keyboard::isKeyPressed(Keyboard::Enter)){
                        isLoos = false;
                        score_1=0;
                        gameMode = "game";
                        lastEnterGameNoPause = currTime;
                        isPaused = false;
                        lastFlight = currTime;
                        startSound.play();
                        createAliens(&aliens, &alien_small, &alien_mid, &alien_big, &alien_biggest, &curAnimations);
                        for (int i=0; i<2; i++){
                            lifeCount.setPosition(Vector2f(540.0+30.0*i, 60.f));
                            lifesCounter.push_back(lifeCount);
                        }
                        curLevel=1;
                        for (int i=0; i<currentLevelCount.size() - 1; i++){
                            currentLevelCount.pop_back();
                        }
                        gameover.setPosition(Vector2f(290.f, 400.f));
                    }
                }
                if (gameMode == "game" && !isDeath && !isLoos){ // Игра
                    if (Keyboard::isKeyPressed(Keyboard::Escape)){
                        prevmode = "game";
                        gameMode = "settings";
                        timeInMenu = time(nullptr);
                        if (startSound.getStatus() == Sound::Status::Playing){
                            startSound.pause();
                        }
                        music.pause();
                    }
                    if (Keyboard::isKeyPressed(Keyboard::Enter) && ((currTime - lastEnterGameNoPause) >= 2)){
                        isPaused = !isPaused;
                        if (isPaused){
                            lastPause = currTime;
                        } else {
                            lastFlight += currTime - lastPause;
                        }
                    }
                    if (!isPaused){
                        if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)){ // Движение влево
                            vx_rocketShip = -4.5f;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)){ // Движение вправо
                            vx_rocketShip = 4.5f;
                        }
                        if ((Keyboard::isKeyPressed(Keyboard::Z) || Keyboard::isKeyPressed(Keyboard::X)) && bullets.size() == 0){ // Выстрел при нажатии Z или X каждые 3 секунды
                            bullet.setPosition(rocketShip.getPosition().x+25, 535);
                            bullets.push_back(bullet);
                            shotSound.play();
                        }
                    }
                }
            }
            if (event.type == Event::KeyReleased){ // Кнопка отпущена
                if (event.key.code == Keyboard::Left || event.key.code == Keyboard::Right || event.key.code == Keyboard::A || event.key.code == Keyboard::D){ // Если отпущена кнопка управления
                    vx_rocketShip = 0; // Зануляем скорость
                }
            }

        }
        
        // Общее
        window.clear(Color::Black); // Заливаем чёрным
        for(int i=0; i<background.size(); i++){ // Отрисовка и движение заднего фона
            if (!isPaused){
                if (background[i].getPosition().y + 2 < 820){ // Если не достиг низа экрана
                    background[i].move(Vector2f(0.f, 2.f)); // то двигается
                } else { // иначе
                    background[i].setPosition(Vector2f(background[i].getPosition().x, 0)); // y = 0
                }
                if (backgroundDelay[i] < 20){
                    backgroundDelay[i] += 1;
                } else {
                    backgroundDelay[i] = 0;
                }
            }
            if (backgroundDelay[i] < 10){
                window.draw(background[i]);
            }
        }
        if (player_1.getPosition().y > 30){
            for (int i=0; i<startScreenObj.size(); i++){
                (*(startScreenObj[i])).move(Vector2f(0.f, -5.f));
            }
            for (int i=0; i<startScreenText.size(); i++){
                (*(startScreenText[i])).move(Vector2f(0.f, -5.f));
            }
            pickTriangle.move(Vector2f(0.f, -5.f));
        }
        highest_score = high_score_value.getString();
        if (score_1 > stoi(highest_score)){
            high_score_value.setString(to_string(score_1));
            std::ofstream out;
            out.open("resources/score.txt");
            out << highest_score;
            out.close();
        }
        window.draw(player_1); // Текст сверху слева 1UP
        player_1_score.setString(to_string(score_1));
        window.draw(player_1_score); // Баллы в том же месте
        window.draw(high_score);
        window.draw(high_score_value);


        // Проверяем режимы
        if (gameMode == "start"){ // Если режим - старт
            window.draw(logo); // Рисуем лого
            window.draw(players); // Рисуем надпись 1 player
            window.draw(pickTriangle); // Нарисовать треугольник выбора
            window.draw(company);
            window.draw(companyRights);
            window.draw(settings);
        }

        if (gameMode == "game"){ // Если режим - игра
            if (isPaused){
                window.draw(pause);
            }
            if (isLoos){
                window.draw(gameover);
                if (gameover.getPosition().y > 400){
                    gameover.move(Vector2f(0.f, -3.f));
                }
            }
            if (aliens.size()==0){
                createAliens(&aliens, &alien_small, &alien_mid, &alien_big, &alien_biggest, &curAnimations);
                curLevel++;   
                if (curLevel < 6){
                    flagLevel.move(Vector2f(30.f, 0.f));
                    currentLevelCount.push_back(flagLevel);
                } else if (curLevel == 6){
                    for (int i=0; i<4; i++){
                        currentLevelCount.pop_back();
                    }
                }
            }
            if (!isPaused){
                rocketShip.move(Vector2f(vx_rocketShip, 0.f)); // Двигаем ракету
                if (rocketShip.getPosition().x < 0){
                    rocketShip.setPosition(Vector2f(0, rocketShip.getPosition().y));
                }
                if (rocketShip.getPosition().x > 775){
                    rocketShip.setPosition(Vector2f(775, rocketShip.getPosition().y));
                }
            }
            if (!isDeath){
                window.draw(rocketShip); // Рисуем ракету
            } else if (flyAliens.size()==0){
                deathFrames++;
                if (deathFrames > 40){
                    window.draw(death);
                }
                if (deathFrames > 150){
                    isDeath=false;
                }
            }
            for (int i=0; i<alienBullets.size(); i++){
                if (i>alienBullets.size()){
                    break;
                }
                window.draw(alienBullets[i]);
                if (!isPaused){
                    alienBullets[i].move(Vector2f(0.f, 10.f));
                }
                if (alienBullets[i].getPosition().y > 625){
                    alienBullets.erase(alienBullets.begin()+i);
                }
                bull = IntRect({(int)(alienBullets[i].getPosition().x), (int)(alienBullets[i].getPosition().y)}, {(int)(alienBullets[i].getSize().x), (int)(alienBullets[i].getSize().y)});
                rocketCollide = IntRect({(int)(rocketShip.getPosition().x), (int)(rocketShip.getPosition().y)}, {(int)(rocketShip.getSize().x), (int)(rocketShip.getSize().y)});
                if (bull.intersects(rocketCollide)){
                    alienBullets.erase(alienBullets.begin()+i);
                    if (lifesCounter.size()>0){
                        rocketShip.move(Vector2f(412.5-rocketShip.getPosition().x, 0.f));
                        isDeath=true;
                        deathFrames=0;
                        boomStatic.setPosition(alienBullets[i].getPosition());
                        boomStatic.setTexture(&boom1);
                        boomsStatic.push_back(boomStatic);
                        boomStaticDelay.push_back(0);
                        lifesCounter.erase(lifesCounter.begin()+lifesCounter.size()-1);
                    } else {
                        isLoos = true;
                        gameover.move(Vector2f(0.f, 300.f));
                        boomStatic.setPosition(flyAliens[i].getPosition());
                        boomStatic.setTexture(&boom1);
                        boomsStatic.push_back(boomStatic);
                        boomStaticDelay.push_back(0);
                    }
                }


            }

            updateMusicSound(&music, &startSound, &explodeSound, &flyAlienSound, &shotSound, &volume, &volumeMusic, &volumeSound);
            for (int i=0; i<bullets.size(); i++){ // Для каждой пули
                if (i>bullets.size()){ // Если вышли за край случайно
                    break; // Заканчиваем цикл
                }
                if (!isPaused){
                    bullets[i].move(Vector2f(0.f, -10.f)); // Двигаем пулю
                }
                if (bullets[i].getPosition().y<0){ // Если пуля залетела за край экрана
                    bullets.erase(bullets.begin()+i); // Удаляем пулю
                }
                isIntersect = false;
                bull = IntRect({(int)(bullets[i].getPosition().x), (int)(bullets[i].getPosition().y)}, {(int)(bullets[i].getSize().x), (int)(bullets[i].getSize().y)});
                for (int j=0; j<aliens.size(); j++){
                    alie = IntRect({(int)(aliens[j].getPosition().x), (int)(aliens[j].getPosition().y)}, {(int)(aliens[j].getSize().x), (int)(aliens[j].getSize().y)});
                    if (bull.intersects(alie) && aliens[j].getFillColor() != Color::Black){
                        for (int i=0; i<flyAliensIndexes.size(); i++){
                            if (flyAliensIndexes[i] > j){
                                flyAliensIndexes[i]--;
                            }
                        }
                        if (aliens[j].getTexture() == &alien_small){
                            score_1 += 30;
                        } else if (aliens[j].getTexture() == &alien_mid){
                            score_1 += 40;
                        } else if (aliens[j].getTexture() == &alien_big){
                            score_1 += 50;
                        } else {
                            score_1 += 60;
                        }
                        explodeSound.play();
                        if (flyAlienSound.getStatus() == SoundSource::Status::Playing){
                            flyAlienSound.stop();
                        }
                        boom.setPosition(aliens[j].getPosition());
                        boom.setTexture(&boom1);
                        booms.push_back(boom);
                        boomDelay.push_back(0);
                        aliens.erase(aliens.begin()+j);
                        curAnimations.erase(curAnimations.begin()+j);
                        bullets.erase(bullets.begin()+i);
                        isIntersect = true;
                        break;
                    }
                }
                if (isIntersect){
                    break;
                }
                for (int j=0; j<flyAliens.size(); j++){
                    alie = IntRect({(int)(flyAliens[j].getPosition().x), (int)(flyAliens[j].getPosition().y)}, {(int)(flyAliens[j].getSize().x), (int)(flyAliens[j].getSize().y)});
                    if (bull.intersects(alie)){
                        if (flyAliens[j].getTexture() == &alien_small || flyAliens[j].getTexture() == &alien_smallup || flyAliens[j].getTexture() == &alien_smalldown){
                            score_1 += 60;
                        } else if (flyAliens[j].getTexture() == &alien_mid || flyAliens[j].getTexture() == &alien_midup || flyAliens[j].getTexture() == &alien_middown){
                            score_1 += 80;
                        } else if (flyAliens[j].getTexture() == &alien_big || flyAliens[j].getTexture() == &alien_bigup || flyAliens[j].getTexture() == &alien_bigdown){
                            score_1 += 100;
                        } else {
                            scoreBiggestFromRand = scoresBiggestRandom[rand()%4];
                            score_1 += scoreBiggestFromRand;
                            scoreBiggest.setString(to_string(scoreBiggestFromRand));
                            scoreBiggest.setPosition(flyAliens[j].getPosition());
                            scoresBiggest.push_back(scoreBiggest);
                            connectBoomScore.push_back((int)(boomsStatic.size()));
                        }
                        explodeSound.play();
                        if (flyAlienSound.getStatus() == SoundSource::Status::Playing){
                            flyAlienSound.stop();
                        }
                        boomStatic.setPosition(flyAliens[j].getPosition());
                        boomStatic.setTexture(&boom1);
                        boomsStatic.push_back(boomStatic);
                        boomStaticDelay.push_back(0);
                        aliens.erase(aliens.begin()+flyAliensIndexes[j]);
                        curAnimations.erase(curAnimations.begin()+flyAliensIndexes[j]);
                        flyAliens.erase(flyAliens.begin()+j);
                        flyAliensCurrSpeed.erase(flyAliensCurrSpeed.begin()+j);
                        flyAliensSpeeds.erase(flyAliensSpeeds.begin()+2*j);
                        flyAliensSpeeds.erase(flyAliensSpeeds.begin()+2*j);
                        flyAliensIndexes.erase(flyAliensIndexes.begin()+j);
                        flyAliensPrevTexture.erase(flyAliensPrevTexture.begin()+j);
                        bullets.erase(bullets.begin()+i);
                        isIntersect = true;
                        break;
                    }
                }
                if (!isIntersect){
                window.draw(bullets[i]); // Рисуем пулю
                }
            }
            for (int i=0; i<lifesCounter.size(); i++){
                window.draw(lifesCounter[i]);
            }
            for (int i=0; i<currentLevelCount.size(); i++){
                window.draw(currentLevelCount[i]);
            }
            if (curLevel > 5){
                if (curLevel < 10){
                    bigLevelCount.setString("0"+to_string(curLevel));
                } else {
                    bigLevelCount.setString(to_string(curLevel));
                }
                window.draw(bigLevelCount);
            }
            if (!isPaused && !isLoos){
                if (currTime >= (lastFlight + delayFlight) && !isDeath && !isPaused){
                    checkSideAlliens(&aliens, &sideAliens);
                    flyAlienSound.play();
                    randIndex = sideAliens[(rand())%(sideAliens.size())];
                    flyAliens.push_back(aliens[randIndex]);
                    flyAliensCurrSpeed.push_back(0);
                    if (aliens[randIndex].getPosition().x < 412){
                        flyAliensSpeeds.push_back(Vector2f(-1.0*aliens[randIndex].getPosition().x/60, 1.0*0.5*(625-aliens[randIndex].getPosition().y)/60));
                        if (rocketShip.getPosition().x < 412){
                            flyAliensSpeeds.push_back(Vector2f(1.0*rocketShip.getPosition().x/60, -0.4+1.0*0.5*(625-aliens[randIndex].getPosition().y)/60));
                        } else {
                            flyAliensSpeeds.push_back(Vector2f(1.0*412/60, -0.4+1.0*0.5*(625-aliens[randIndex].getPosition().y)/60));
                        }
                    } else {
                        flyAliensSpeeds.push_back(Vector2f(1.0*(783-aliens[randIndex].getPosition().x)/60, 1.0*0.5*(625-aliens[randIndex].getPosition().y)/60));
                        if (rocketShip.getPosition().x > 412){
                            flyAliensSpeeds.push_back(Vector2f((-825+42+1.0*rocketShip.getPosition().x)/60, -0.4+1.0*0.5*(625-aliens[randIndex].getPosition().y)/60));
                        } else {
                            flyAliensSpeeds.push_back(Vector2f((-825+42+1.0*412)/60, -0.4+1.0*0.5*(625-aliens[randIndex].getPosition().y)/60));
                        }
                    }
                    flyAliensPrevTexture.push_back(aliens[randIndex].getTexture());
                    flyAliensIndexes.push_back(randIndex);
                    aliens[randIndex].setFillColor(Color::Black);
                    lastFlight = currTime;
                    
                    
                    if (aliens[randIndex].getTexture() == &alien_biggest){
                        sideAliensRedIndexes.clear();
                        for (int i=0; i<aliens.size(); i++){
                            if ((aliens[i].getTexture() == &alien_big || aliens[i].getTexture() == &alien_bigup || aliens[i].getTexture() == &alien_bigdown) && (abs(aliens[i].getPosition().x - aliens[randIndex].getPosition().x) < 80) && (aliens[i].getFillColor() != Color::Black)){
                                sideAliensRedIndexes.push_back(i);
                            }
                        }
                        if (sideAliensRedIndexes.size() > 0){
                            if (sideAliensRedIndexes.size() > 2){
                                flyAliens.push_back(aliens[sideAliensRedIndexes[0]]);
                                flyAliensCurrSpeed.push_back(0);

                                if (aliens[randIndex].getPosition().x < 412){
                                    flyAliensSpeeds.push_back(Vector2f(-1.0*aliens[sideAliensRedIndexes[0]].getPosition().x/60, 1.0*0.5*(705-aliens[sideAliensRedIndexes[0]].getPosition().y)/60));
                                    if (rocketShip.getPosition().x < 412){
                                        flyAliensSpeeds.push_back(Vector2f(1.0*(rocketShip.getPosition().x-40)/60, 1.0*0.5*(545-aliens[sideAliensRedIndexes[0]].getPosition().y)/60));
                                    } else {
                                        flyAliensSpeeds.push_back(Vector2f(1.0*372/60, 1.0*0.5*(545-aliens[sideAliensRedIndexes[0]].getPosition().y)/60));
                                    }
                                } else {
                                    flyAliensSpeeds.push_back(Vector2f(1.0*(783-aliens[sideAliensRedIndexes[0]].getPosition().x)/60, 1.0*0.5*(705-aliens[sideAliensRedIndexes[0]].getPosition().y)/60));
                                    if (rocketShip.getPosition().x > 412){
                                        flyAliensSpeeds.push_back(Vector2f((-825+42+1.0*rocketShip.getPosition().x-40)/60, 1.0*0.5*(545-aliens[sideAliensRedIndexes[0]].getPosition().y)/60));
                                    } else {
                                        flyAliensSpeeds.push_back(Vector2f((-825+42+1.0*412-40)/60, 1.0*0.5*(545-aliens[sideAliensRedIndexes[0]].getPosition().y)/60));
                                    }
                                }
                                flyAliensPrevTexture.push_back(aliens[sideAliensRedIndexes[0]].getTexture());
                                flyAliensIndexes.push_back(sideAliensRedIndexes[0]);
                                aliens[sideAliensRedIndexes[0]].setFillColor(Color::Black);

                                flyAliens.push_back(aliens[sideAliensRedIndexes[2]]);  
                                flyAliensCurrSpeed.push_back(0);
                                            
                                if (aliens[randIndex].getPosition().x < 412){
                                    flyAliensSpeeds.push_back(Vector2f(-1.0*aliens[sideAliensRedIndexes[2]].getPosition().x/60, 1.0*0.5*(785-aliens[sideAliensRedIndexes[2]].getPosition().y)/60));
                                    if (rocketShip.getPosition().x < 412){
                                        flyAliensSpeeds.push_back(Vector2f(1.0*(rocketShip.getPosition().x+40)/60, -0.4+1.0*0.5*(465-aliens[sideAliensRedIndexes[2]].getPosition().y)/60));
                                    } else {
                                        flyAliensSpeeds.push_back(Vector2f(1.0*452/60, -0.4+1.0*0.5*(465-aliens[sideAliensRedIndexes[2]].getPosition().y)/60));
                                    }
                                } else {
                                    flyAliensSpeeds.push_back(Vector2f(1.0*(783-aliens[sideAliensRedIndexes[2]].getPosition().x)/60, 1.0*0.5*(785-aliens[sideAliensRedIndexes[2]].getPosition().y)/60));
                                    if (rocketShip.getPosition().x > 412){
                                        flyAliensSpeeds.push_back(Vector2f((-825+42+40+1.0*rocketShip.getPosition().x)/60, -0.4+1.0*0.5*(465-aliens[sideAliensRedIndexes[2]].getPosition().y)/60));
                                    } else {
                                        flyAliensSpeeds.push_back(Vector2f((-825+42+40+1.0*412)/60, -0.4+1.0*0.5*(465-aliens[sideAliensRedIndexes[2]].getPosition().y)/60));
                                    }
                                }
                                
                                flyAliensPrevTexture.push_back(aliens[sideAliensRedIndexes[2]].getTexture());
                                flyAliensIndexes.push_back(sideAliensRedIndexes[2]);
                                aliens[sideAliensRedIndexes[2]].setFillColor(Color::Black);

                            } else {
                                for (int i=0; i<sideAliensRedIndexes.size(); i++){
                                    flyAliens.push_back(aliens[sideAliensRedIndexes[i]]);
                                    flyAliensCurrSpeed.push_back(0);
                                    
                                    flyAliensPrevTexture.push_back(aliens[sideAliensRedIndexes[i]].getTexture());
                                    flyAliensIndexes.push_back(sideAliensRedIndexes[i]);
                                    aliens[sideAliensRedIndexes[i]].setFillColor(Color::Black);

                                    if (aliens[randIndex].getPosition().x < 412){
                                        flyAliensSpeeds.push_back(Vector2f(-1.0*aliens[sideAliensRedIndexes[i]].getPosition().x/60, 1.0*0.5*(625+80*(i+1)-aliens[sideAliensRedIndexes[i]].getPosition().y)/60));
                                        if (rocketShip.getPosition().x < 412){
                                            flyAliensSpeeds.push_back(Vector2f(1.0*(rocketShip.getPosition().x-40+80*i)/60, -0.4+1.0*0.5*(625-80*(i+1)-aliens[sideAliensRedIndexes[i]].getPosition().y)/60));
                                        } else {
                                            flyAliensSpeeds.push_back(Vector2f(1.0*(372+80*i)/60, -0.4+1.0*0.5*(625-80*(i+1)-aliens[sideAliensRedIndexes[i]].getPosition().y)/60));
                                        }
                                    } else {
                                        flyAliensSpeeds.push_back(Vector2f(1.0*(783-aliens[sideAliensRedIndexes[i]].getPosition().x)/60, 1.0*0.5*(625+80*(i+1)-aliens[sideAliensRedIndexes[i]].getPosition().y)/60));
                                        if (rocketShip.getPosition().x > 412){
                                            flyAliensSpeeds.push_back(Vector2f((-825+42-40+80*i+1.0*rocketShip.getPosition().x)/60, -0.4+1.0*0.5*(625-80*(i+1)-aliens[sideAliensRedIndexes[i]].getPosition().y)/60));
                                        } else {
                                            flyAliensSpeeds.push_back(Vector2f((-825+42-40+80*i+1.0*412)/60, -0.4+1.0*0.5*(625-80*(i+1)-aliens[sideAliensRedIndexes[i]].getPosition().y)/60));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (!isPaused){
                if ((int)(aliens[0].getPosition().x) % 2 == 0){
                    forAliens = Vector2f(-2.f, 0.f);
                } else {
                    forAliens = Vector2f(2.f, 0.f);
                }
                float minX = aliens[0].getPosition().x;
                float maxX = aliens[0].getPosition().x;
                for (int i=1; i<aliens.size(); i++){
                    if (aliens[i].getPosition().x < minX){
                        minX = aliens[i].getPosition().x;
                    }
                    if (aliens[i].getPosition().x > maxX){
                        maxX = aliens[i].getPosition().x;
                    }
                }
                if ((int)(minX) < 110){
                    for (int i=0; i<aliens.size(); i++){
                        aliens[i].move(Vector2f(3.f, 0.f));
                    }
                    for (int i=0; i<booms.size(); i++){
                        booms[i].move(Vector2f(3.f, 0.f));
                    }
                } else if ((int)(maxX) > 720){ 
                    for (int i=0; i<aliens.size(); i++){
                        aliens[i].move(Vector2f(-1.f, 0.f));
                    }
                    for (int i=0; i<booms.size(); i++){
                        booms[i].move(Vector2f(-1.f, 0.f));
                    }
                } else {
                    for (int i=0; i<aliens.size(); i++){
                        aliens[i].move(forAliens);
                    }
                    for (int i=0; i<booms.size(); i++){
                        booms[i].move(forAliens);
                    }
                }
            }
            for (int i=0; i<aliens.size(); i++){
                if ((aliens[i].getTexture() == &alien_small || aliens[i].getTexture() == &alien_smallup || aliens[i].getTexture() == &alien_smalldown) && aliens[i].getFillColor() != Color::Black){
                    curAnimations[i]++;
                    if (curAnimations[i] == 4*speedAnimate){
                        aliens[i].setTexture(&alien_small);
                        curAnimations[i] = 0;
                    } else if (curAnimations[i] == 3*speedAnimate){
                        aliens[i].setTexture(&alien_smalldown);
                    } else if (curAnimations[i] == 2*speedAnimate){
                        aliens[i].setTexture(&alien_small);
                    } else if (curAnimations[i] == 1*speedAnimate){
                        aliens[i].setTexture(&alien_smallup);
                    }
                } else if ((aliens[i].getTexture() == &alien_mid || aliens[i].getTexture() == &alien_midup || aliens[i].getTexture() == &alien_middown) && aliens[i].getFillColor() != Color::Black){
                    curAnimations[i]++;
                    if (curAnimations[i] == 4*speedAnimate){
                        aliens[i].setTexture(&alien_mid);
                        curAnimations[i] = 0;
                    } else if (curAnimations[i] == 3*speedAnimate){
                        aliens[i].setTexture(&alien_middown);
                    } else if (curAnimations[i] == 2*speedAnimate){
                        aliens[i].setTexture(&alien_mid);
                    } else if (curAnimations[i] == 1*speedAnimate){
                        aliens[i].setTexture(&alien_midup);
                    }
                } else if ((aliens[i].getTexture() == &alien_big || aliens[i].getTexture() == &alien_bigup || aliens[i].getTexture() == &alien_bigdown) && aliens[i].getFillColor() != Color::Black){
                    curAnimations[i]++;
                    if (curAnimations[i] == 4*speedAnimate){
                        aliens[i].setTexture(&alien_big);
                        curAnimations[i] = 0;
                    } else if (curAnimations[i] == 3*speedAnimate){
                        aliens[i].setTexture(&alien_bigdown);
                    } else if (curAnimations[i] == 2*speedAnimate){
                        aliens[i].setTexture(&alien_big);
                    } else if (curAnimations[i] == 1*speedAnimate){
                        aliens[i].setTexture(&alien_bigup);
                    }
                }
                window.draw(aliens[i]);
            }
            rocketCollide = IntRect({(int)(rocketShip.getPosition().x), (int)(rocketShip.getPosition().y)}, {(int)(rocketShip.getSize().x), (int)(rocketShip.getSize().y)});
            for (int i=0; i<flyAliens.size(); i++){
                alie = IntRect({(int)(flyAliens[i].getPosition().x), (int)(flyAliens[i].getPosition().y)}, {(int)(flyAliens[i].getSize().x), (int)(flyAliens[i].getSize().y)});
                if (rocketCollide.intersects(alie) && !isDeath && !isLoos){
                    if (flyAliens[i].getTexture() == &alien_small){
                            score_1 += 60;
                        } else if (flyAliens[i].getTexture() == &alien_mid){
                            score_1 += 80;
                        } else if (flyAliens[i].getTexture() == &alien_big){
                            score_1 += 100;
                        } else {
                            scoreBiggestFromRand = scoresBiggestRandom[rand()%4];
                            score_1 += scoreBiggestFromRand;
                            scoreBiggest.setString(to_string(scoreBiggestFromRand));
                            scoreBiggest.setPosition(flyAliens[i].getPosition());
                            scoresBiggest.push_back(scoreBiggest);
                            connectBoomScore.push_back((int)(boomsStatic.size()));
                        }
                    if (lifesCounter.size()>0){
                        rocketShip.move(Vector2f(412.5-rocketShip.getPosition().x, 0.f));
                        isDeath=true;
                        deathFrames=0;
                        boomStatic.setPosition(flyAliens[i].getPosition());
                        boomStatic.setTexture(&boom1);
                        boomsStatic.push_back(boomStatic);
                        boomStaticDelay.push_back(0);
                        for (int j=0; j<flyAliensIndexes.size(); j++){
                            if (flyAliensIndexes[j] > flyAliensIndexes[i]){
                                flyAliensIndexes[j]--;
                            }
                        }
                        aliens.erase(aliens.begin()+flyAliensIndexes[i]);
                        curAnimations.erase(curAnimations.begin()+flyAliensIndexes[i]);
                        flyAliensCurrSpeed.erase(flyAliensCurrSpeed.begin()+i);
                        flyAliensPrevTexture.erase(flyAliensPrevTexture.begin()+i);
                        flyAliensSpeeds.erase(flyAliensSpeeds.begin()+2*i);
                        flyAliensSpeeds.erase(flyAliensSpeeds.begin()+2*i);
                        flyAliensIndexes.erase(flyAliensIndexes.begin()+i);
                        flyAliens.erase(flyAliens.begin()+i);
                        lifesCounter.erase(lifesCounter.begin()+lifesCounter.size()-1);
                    } else {
                        isLoos = true;
                        gameover.move(Vector2f(0.f, 300.f));
                        saveProgress(score_1, "loos", curLevel);
                        boomStatic.setPosition(flyAliens[i].getPosition());
                        boomStatic.setTexture(&boom1);
                        boomsStatic.push_back(boomStatic);
                        boomStaticDelay.push_back(0);
                        for (int j=0; j<flyAliensIndexes.size(); j++){
                            if (flyAliensIndexes[j] > flyAliensIndexes[i]){
                                flyAliensIndexes[j]--;
                            }
                        }
                        aliens.erase(aliens.begin()+flyAliensIndexes[i]);
                        curAnimations.erase(curAnimations.begin()+flyAliensIndexes[i]);
                        flyAliensCurrSpeed.erase(flyAliensCurrSpeed.begin()+i);
                        flyAliensPrevTexture.erase(flyAliensPrevTexture.begin()+i);
                        flyAliensSpeeds.erase(flyAliensSpeeds.begin()+2*i);
                        flyAliensSpeeds.erase(flyAliensSpeeds.begin()+2*i);
                        flyAliensIndexes.erase(flyAliensIndexes.begin()+i);
                        flyAliens.erase(flyAliens.begin()+i);
                    }
                } else {
                    if (flyAliens[i].getPosition().y < rocketShip.getPosition().y){
                        flyAliens[i].setOrigin(Vector2f(flyAliens[i].getSize().x/2, flyAliens[i].getSize().y/2));
                        flyAliens[i].setRotation(360-180/3.14*atan((rocketShip.getPosition().x-flyAliens[i].getPosition().x)/(rocketShip.getPosition().y-flyAliens[i].getPosition().y)));
                        flyAliens[i].setOrigin(Vector2f(0, 0));
                    }
                    window.draw(flyAliens[i]);
                    if (!isPaused) {
                        flyAliensCurrSpeed[i]++;
                        if (flyAliensCurrSpeed[i] == 120){
                            flyAliens[i].move(Vector2f(412.5-flyAliens[i].getPosition().x, -1.0*flyAliens[i].getPosition().y));
                        } 
                        if (flyAliensCurrSpeed[i] == 20){
                            alienBullet.setPosition(flyAliens[i].getPosition());
                            alienBullets.push_back(alienBullet);
                            alienBullet.setPosition(Vector2f(flyAliens[i].getPosition().x+42, flyAliens[i].getPosition().y));
                            alienBullets.push_back(alienBullet);
                        }
                        if (flyAliensCurrSpeed[i] == 90){
                            alienBullet.setPosition(flyAliens[i].getPosition());
                            alienBullets.push_back(alienBullet);
                            alienBullet.setPosition(Vector2f(flyAliens[i].getPosition().x+42, flyAliens[i].getPosition().y));
                            alienBullets.push_back(alienBullet);
                        }
                        if (flyAliensCurrSpeed[i] < 120){
                            flyAliens[i].move(flyAliensSpeeds[2*i+(flyAliensCurrSpeed[i]/60)]);
                        } else if (flyAliensCurrSpeed[i] < 200) {
                            if (flyAliens[i].getPosition().x != aliens[flyAliensIndexes[i]].getPosition().x){
                                flyAliens[i].move(Vector2f((aliens[flyAliensIndexes[i]].getPosition().x-flyAliens[i].getPosition().x)/30, 0.f));
                            }
                            if (flyAliens[i].getPosition().y != aliens[flyAliensIndexes[i]].getPosition().y){
                                flyAliens[i].move(Vector2f(0.f, (aliens[flyAliensIndexes[i]].getPosition().y-flyAliens[i].getPosition().y)/30));
                            } 
                        } else {
                            aliens[flyAliensIndexes[i]].setFillColor(Color::White);
                            aliens[flyAliensIndexes[i]].setTexture(flyAliensPrevTexture[i]);
                            flyAliensCurrSpeed.erase(flyAliensCurrSpeed.begin()+i);
                            flyAliensPrevTexture.erase(flyAliensPrevTexture.begin()+i);
                            flyAliensSpeeds.erase(flyAliensSpeeds.begin()+2*i);
                            flyAliensSpeeds.erase(flyAliensSpeeds.begin()+2*i);
                            flyAliensIndexes.erase(flyAliensIndexes.begin()+i);
                            flyAliens.erase(flyAliens.begin()+i);
                        }
                    }
                }
            }
            for (int i=0; i<booms.size();i++){
                if (boomDelay[i] == 11){
                    booms[i].setTexture(&boom2);
                } else if (boomDelay[i] == 21){
                    booms[i].setTexture(&boom3);
                } else if (boomDelay[i] == 31){
                    booms.erase(booms.begin()+i);
                    boomDelay.erase(boomDelay.begin()+i);
                } 
                if (i<booms.size() && !isPaused){
                    window.draw(booms[i]);
                    boomDelay[i]++;
                }
            }
            for (int i=0; i<boomsStatic.size();i++){
                if (boomStaticDelay[i] <= 10){
                    window.draw(boomsStatic[i]);
                } else if (boomStaticDelay[i] > 10 && boomStaticDelay[i] <= 20){
                    boomsStatic[i].setTexture(&boom2);
                    window.draw(boomsStatic[i]);
                } else if (boomStaticDelay[i] > 20 && boomStaticDelay[i] <= 30){
                    boomsStatic[i].setTexture(&boom3);
                    window.draw(boomsStatic[i]);
                } else if (boomStaticDelay[i] > 30  && (find(connectBoomScore.begin(), connectBoomScore.end(), i) == connectBoomScore.end())){
                    boomsStatic.erase(boomsStatic.begin()+i);
                    boomStaticDelay.erase(boomStaticDelay.begin()+i);
                } else if (boomStaticDelay[i] > 30 && boomStaticDelay[i] < 50 && (find(connectBoomScore.begin(), connectBoomScore.end(), i) != connectBoomScore.end())){
                    window.draw(scoresBiggest[find(connectBoomScore.begin(), connectBoomScore.end(), i) - connectBoomScore.begin()]);
                } else if (boomStaticDelay[i] == 50 && (find(connectBoomScore.begin(), connectBoomScore.end(), i) != connectBoomScore.end())){
                    whereScoreBiggest = find(connectBoomScore.begin(), connectBoomScore.end(), i) - connectBoomScore.begin();
                    boomsStatic.erase(boomsStatic.begin()+i);
                    boomStaticDelay.erase(boomStaticDelay.begin()+i);
                    scoresBiggest.erase(scoresBiggest.begin()+whereScoreBiggest);
                    connectBoomScore.erase(connectBoomScore.begin()+whereScoreBiggest);
                }
                if (i<boomsStatic.size() && !isPaused){
                    boomStaticDelay[i]++;
                }
            }  
        }
        if (gameMode == "settings"){
            window.draw(setvolumemain);
            window.draw(mainvolumeback);
            mainvolume.setSize(Vector2f(700.0*volume, mainvolume.getSize().y));
            window.draw(mainvolume);
            window.draw(setvolumemusic);
            window.draw(musicvolumeback);
            musicvolume.setSize(Vector2f(700.0*volumeMusic, musicvolume.getSize().y));
            window.draw(musicvolume);
            window.draw(setvolumesound);
            window.draw(soundvolumeback);
            soundvolume.setSize(Vector2f(700.0*volumeSound, soundvolume.getSize().y));
            window.draw(soundvolume);
            window.draw(pickTriangleMusic);
        }
        window.display();
    }
}