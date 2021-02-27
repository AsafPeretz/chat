#include "ui_manager.h"

#define SIZE 10

int main() {
	char name[SIZE], password[SIZE];
	UI_Err();
	UI_OK();
	UI_OpenScreen();
	UI_StartMenu();
	UI_LoginMenu();
	UI_GroupMenu();
	UI_Login(name,password);
	return 0;
}
