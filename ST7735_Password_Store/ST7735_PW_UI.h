#include "ST7735_PW_Keyboard.h"
#include "HardwareSerial.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Colours.h>
#include <AES.h>
#include <IR_Codes.h>
#include "EEPROM_Manager.h"

#define PASSWORDS_PER_PAGE 10
#define PASSWORD_SEP 14
#define EEPROM_PW_ENTRY_SIZE 96 // Each entry takes up 96 bytes (IN HEX)
#define PASSWORD_START_Y 18

#define PHRASE_SEP 12
#define PHRASES_PER_PAGE 10

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

class Password_Manager;
class Password_Entry;

class Wallet_Manager;
class Wallet_Entry;

class ST7735_PW_Menu_Item
{
	public:
		ST7735_PW_Menu_Item(Adafruit_ST7735*, const char*);
		void display(int);
        void displaySelected(int);
        void displayAdd(int);
        void displayAddSelected(int);
        const char* getTitle();
        boolean addSelected = false;
		
	private:
		Adafruit_ST7735* tft;
        const char* title;
};

class ST7735_PW_Menu
{
	public:
		ST7735_PW_Menu(Adafruit_ST7735*);
		void display();
        void interact(uint32_t*, Password_Manager*, Wallet_Manager*);
		
	private:
		Adafruit_ST7735* tft;
        ST7735_PW_Menu_Item* items;

        int selected_index;
        boolean entered = false;
};

class Password_Manager
{
    public:
        Password_Manager(Adafruit_ST7735*, AES128*, ST7735_PW_Keyboard*, EEPROM_Manager*);
        void display(); // Displays passwords and usernames on screen
        void interact(uint32_t*); // Allows user interaction
        void encrypt(char*, byte*); // Encrypts passed data and stores in aes_buffer
        void decrypt(byte*, char*); // Decrypts the passed data and stores in aes_buffer
        void setKey(char* key); // Saves the encryption key in key_bytes
        void load(int position); // Loads and decrypts saved password from file and stores in Password_Entry instance
        void save(Password_Entry*); // Saves encrypted Password_Entry to SD card at given position
        boolean escaped = false;
        void setStage(int);
        void sortEntries();

    private:
        Adafruit_ST7735* tft;
        AES128* aes128;
        ST7735_PW_Keyboard* keyboard;
        byte key_bytes[16];
        int stage = 0; // 0 = display names, 1 = display selected password/username, 2 = add name, 3 = add email, 4 = add password
        EEPROM_Manager* eeprom_manager;

        int password_count = 0;
        Password_Entry* entries;

        // For displaying passwords
        int start_pw_display_index = 0;
        int selected_pw_index = 0;
};

// Each entry stored in memory as | 0 | encrypted_name (32 bytes) |
//  encrypted email (32 bytes) | encrypted password (32 bytes)
class Password_Entry
{
    public:
        char* getName();
        char* getEmail();
        char* getPassword();

        void setName(char*);
        void setEmail(char*);
        void setPassword(char*);

    private:
        char name[32];
        char email[32];
        char password [32];
};

class Wallet_Manager
{
    public:
        Wallet_Manager(Adafruit_ST7735*, AES128*, ST7735_PW_Keyboard*, EEPROM_Manager*);
        void display(); // Displays passwords and usernames on screen
        void interact(uint32_t*); // Allows user interaction
        void encrypt(char*, byte*); // Encrypts passed data and stores in aes_buffer
        void decrypt(byte*, char*); // Decrypts the passed data and stores in aes_buffer
        void load(int position); // Loads and decrypts saved password from file and stores in Password_Entry instance
        void save(Wallet_Entry*); // Saves encrypted Password_Entry to SD card at given position
        void setStage(int);

        boolean escaped = false;

    private:
        Adafruit_ST7735* tft;
        AES128* aes128;
        ST7735_PW_Keyboard* keyboard;
        int stage = 0; // 0 = display names, 1 = display selected password/username, 2 = add name, 3 = add email, 4 = add password
        EEPROM_Manager* eeprom_manager;

        int wallet_count = 0;
        Wallet_Entry* entries;

        // For displaying passwords
        int start_wallet_display_index = 0;
        int selected_wallet_index = 0;

        int new_phrase_count = 0;
        int current_phrases_added = 0;
};

// Each entry stored in memory as | 0 | encrypted_name (32 bytes) |
//  encrypted email (32 bytes) | encrypted password (32 bytes)
class Wallet_Entry
{
    public:
        char* getName();
        char** getPhrases();

        void setName(char*);
        void addPhrase(char*);

        int phrase_count;

    private:
        char name[32];
        char* phrases[32];
};
