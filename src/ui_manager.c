#include "ui_manager.h"

void UI_OpenScreen() {
    printf("\n 😀️ Hello wellcome to chat app\n"); 
}

void UI_StartMenu() {
    printf("\n 🤔️ Menu: \n0)Register\n1)Log in\n9)Exit\n\n"); 
}

void UI_LoginMenu() {
	printf("\n\nMenu: \n4)Join group\n3)Open group\n2)Log out\n9)Exit\n\n");
}

void UI_GroupMenu() {
	printf("\n\nMenu: \n5)Leave group\n4)Join group\n3)Open group\n2)Log out\n9)Exit\n\n");	
}

void UI_Login(char *a_name, char *a_password) {
	printf("\n✍️  LOG IN page\n");
	printf("\n🤗️  Please insert your name:\n");
	scanf("%s",a_name);
	printf("\n🤗️  Please insert your password:\n");
	scanf("%s",a_password);
}

void UI_Register(char *a_name, char *a_password) {
	printf("\n✍️  REGISTER page\n");
	printf("\n🤗️  Please insert your name:\n");
	scanf("%s",a_name);
	printf("\n🤗️  Please insert your password:\n");
	scanf("%s",a_password);
}

void UI_Err() {
	printf(RED"\nYour request FAILED !\n"RESET);
}

void UI_OK() {
	printf(GRN"\nYour request SUCCESS !\n"RESET);
}
	
