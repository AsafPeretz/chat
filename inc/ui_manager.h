#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <stdio.h>  /* printf */
#include <stdlib.h>
#include <string.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

void UI_OpenScreen();

void UI_StartMenu();

void UI_LoginMenu();

void UI_GroupMenu();

void UI_Login(char *a_name, char *a_password);

void UI_Register(char *a_name, char *a_password);

void UI_Err();

void UI_OK();

#endif /* UI_MANAGER_H */