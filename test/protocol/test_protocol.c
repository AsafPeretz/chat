#include <stdio.h>
#include <string.h>
#include "protocol.h"

#define PRINT_TEST(a_testResult, a_testName) if (!a_testResult) { printf("%s\n", a_testName); }

static int Test_Pack_CheckTag_Request(char *a_testName);
static int Test_Pack_CheckTag_Response(char *a_testName);
static int Test_UnPack_Request(char *a_testName);
static int Test_Response_UnPack(char *a_testName);

int main() {
	char testName[100];
	PRINT_TEST(Test_Pack_CheckTag_Request(testName), testName);
	PRINT_TEST(Test_Pack_CheckTag_Response(testName), testName);
	PRINT_TEST(Test_UnPack_Request(testName), testName);
	PRINT_TEST(Test_Response_UnPack(testName), testName);
	return 0;
}


static int Test_Pack_CheckTag_Request(char *a_testName) {
	char tagR, tagIn, tagOut, tagOpen, tagJoin, tagLeave;
	char bufferR[4096], bufferIn[4096], bufferOut[4096], bufferOpen[4096], bufferJoin[4096], bufferLeave[4096];
	size_t sizeR, sizeIn, sizeOut, sizeOpen, sizeJoin, sizeLeave;
	Request_LogIn reg = {Protocol_Request_Register , "asaf" , "1122aabb1122"}; 				/* size = 1 + 1 + 5 + 13 = 20 */
	Request_LogIn login = {Protocol_Request_LogIn , "maor" , "3344ccdd4433"}; 				/* size = 1 + 1+ 5 + 13 = 20 */
	Request_Logout logout = {Protocol_Request_LogOut , "ziv" , "2\0group1\0group2"}; 	/* size = 1 + 1+ 4 + 4000 = 4006 */
	Request_Group open = {Protocol_Request_GroupOpen , "team1"}; 											/* size = 1 + 1+ 6 = 8 */
	Request_Group join = {Protocol_Request_GroupJoin , "team22"}; 										/* size = 1 + 1+ 7 = 9 */
	Request_Group leave = {Protocol_Request_GroupLeave , "teammmm3"}; 								/* size = 1 + 1+ 9 = 11 */
	strcpy(a_testName, "Test_Pack_CheckTag_Request");

	if (Protocol_Success != Protocol_Pack(&reg, bufferR, &sizeR) || sizeR != 20) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&login, bufferIn, &sizeIn) || sizeIn != 20) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&logout, bufferOut, &sizeOut) || sizeOut != 4006) {
		return 0;
	}
	if (Protocol_Success !=Protocol_Pack(&open, bufferOpen, &sizeOpen) || sizeOpen != 8) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&join, bufferJoin, &sizeJoin) || sizeJoin != 9) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&leave, bufferLeave, &sizeLeave) || sizeLeave != 11) {
		return 0;
	}

	if (Protocol_Success != Protocol_CheckTag(bufferR , &tagR) || tagR != Protocol_Request_Register) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferIn , &tagIn) || tagIn != Protocol_Request_LogIn) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferOut , &tagOut) || tagOut != Protocol_Request_LogOut) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferOpen , &tagOpen) || tagOpen != Protocol_Request_GroupOpen) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferJoin , &tagJoin) || tagJoin != Protocol_Request_GroupJoin) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferLeave , &tagLeave) || tagLeave != Protocol_Request_GroupLeave) {
		return 0;
	}
	return 1;
}


static int Test_Pack_CheckTag_Response(char *a_testName) {
	char tagR, tagIn, tagOut, tagOpen, tagJoin, tagNames, tagLeave;
	char bufferR[4096], bufferIn[4096], bufferOut[4096], bufferOpen[4096], bufferJoin[4096], bufferNames[4096], bufferLeave[4096];
	size_t sizeR, sizeIn, sizeOut, sizeOpen, sizeJoin, sizeNames, sizeLeave;
	Response_LogInOut reg = {Protocol_Response_Register , 1};  																	/* size = 3 */ 
	Response_LogInOut login = {Protocol_Response_LogIn , 1}; 																		/* size = 3 */ 
	Response_LogInOut logout = {Protocol_Response_LogOut , 1};																	/* size = 3 */ 
	Response_Group open = {Protocol_Response_GroupOpen , 1,  "225.0.0.1" , "5000", "teamZiv"}; 	/* size = 3 + 10 + 5 + 8 = 26 */
	Response_Group join = {Protocol_Response_GroupJoin , 1, "225.0.0.4" , "5000" , "teamA"}; 		/* size = 3 + 10 + 5 + 6 = 24 */
	Response_GroupNames allNames = {Protocol_Response_GroupsNames , "3\0aaa\0bbb\0ccc\0"}; 			/* size = 2 + 4000 = 4002 */
	Response_LeaveGroup leave = {Protocol_Response_LeaveGroup , 1, "group"}; 										/* size = 3 + 6 = 9 */
	strcpy(a_testName, "Test_Pack_CheckTag_Response");

	if (Protocol_Success != Protocol_Pack(&reg, bufferR, &sizeR) || sizeR != 3) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&login, bufferIn, &sizeIn) || sizeIn != 3) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&logout, bufferOut, &sizeOut) || sizeOut != 3) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&open, bufferOpen, &sizeOpen) || sizeOpen != 26) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&join, bufferJoin, &sizeJoin) || sizeJoin != 24) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&allNames, bufferNames, &sizeNames) || sizeNames != 4002) {
		return 0;
	}
	if (Protocol_Success != Protocol_Pack(&leave, bufferLeave, &sizeLeave) || sizeLeave != 9) {
		return 0;
	}

	if (Protocol_Success != Protocol_CheckTag(bufferR , &tagR) || tagR != Protocol_Response_Register) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferIn , &tagIn) || tagIn != Protocol_Response_LogIn) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferOut , &tagOut) || tagOut != Protocol_Response_LogOut) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferOpen , &tagOpen) || tagOpen != Protocol_Response_GroupOpen) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferJoin , &tagJoin) || tagJoin != Protocol_Response_GroupJoin) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferNames , &tagNames) || tagNames != Protocol_Response_GroupsNames) {
		return 0;
	}
	if (Protocol_Success != Protocol_CheckTag(bufferLeave , &tagLeave) || tagLeave != Protocol_Response_LeaveGroup) {
		return 0;
	}
	return 1;
}


static int Test_UnPack_Request(char *a_testName) {
	char bufferR[4096], bufferIn[4096], bufferOut[4096], bufferOpen[4096], bufferJoin[4096], bufferNames[4096], bufferLeave[4096];
	size_t sizeR, sizeIn, sizeOut, sizeOpen, sizeJoin, sizeNames, sizeLeave;
	Request_LogIn reg = {Protocol_Request_Register , "asaf" , "1122aabb1122"}, regEmpty;
	Request_LogIn login = {Protocol_Request_LogIn , "maor" , "3344ccdd4433"}, loginEmpty;
	Request_LogIn logout = {Protocol_Request_LogOut , "ziv" , "123ggff321"}, logoutEmpty;
	Request_Group open = {Protocol_Request_GroupOpen , "team1"}, openEmpty;
	Request_Group join = {Protocol_Request_GroupJoin , "team22"}, joinEmpty;
	Request_Group leave = {Protocol_Request_GroupLeave , "teammmm3"}, leaveEmpty;
	Request_GroupsNames names = {Protocol_Response_GroupsNames}, namesEmpty;
	strcpy(a_testName, "Test_UnPack_Request");

	Protocol_Pack(&reg, bufferR, &sizeR);
	Protocol_Pack(&login, bufferIn, &sizeIn);
	Protocol_Pack(&logout, bufferOut, &sizeOut);
	Protocol_Pack(&open, bufferOpen, &sizeOpen);
	Protocol_Pack(&join, bufferJoin, &sizeJoin);
	Protocol_Pack(&names, bufferNames, &sizeNames);
	Protocol_Pack(&leave, bufferLeave, &sizeLeave);

	if (Protocol_Success != Protocol_Unpack(bufferR, &regEmpty) || regEmpty.m_tag != Protocol_Request_Register || strcmp(regEmpty.m_name,"asaf") || strcmp(regEmpty.m_password,"1122aabb1122")) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferIn, &loginEmpty) || loginEmpty.m_tag != Protocol_Request_LogIn || strcmp(loginEmpty.m_name,"maor") || strcmp(loginEmpty.m_password,"3344ccdd4433")) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferOut, &logoutEmpty) || logoutEmpty.m_tag != Protocol_Request_LogOut || strcmp(logoutEmpty.m_name,"ziv") || strcmp(logoutEmpty.m_password,"123ggff321")) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferOpen, &openEmpty) || openEmpty.m_tag != Protocol_Request_GroupOpen || strcmp(openEmpty.m_groupName,"team1")) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferJoin, &joinEmpty) || joinEmpty.m_tag != Protocol_Request_GroupJoin || strcmp(joinEmpty.m_groupName,"team22")) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferNames, &namesEmpty) || namesEmpty.m_tag != Protocol_Response_GroupsNames) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferLeave, &leaveEmpty) || leaveEmpty.m_tag != Protocol_Request_GroupLeave || strcmp(leaveEmpty.m_groupName,"teammmm3")) {
		return 0;
	}
	return 1;
}


static int Test_Response_UnPack(char *a_testName) {
	char bufferR[4096], bufferIn[4096], bufferOut[4096], bufferOpen[4096], bufferJoin[4096], bufferNames[4096], bufferLeave[4096];
	size_t sizeR, sizeIn, sizeOut, sizeOpen, sizeJoin, sizeNames, sizeLeave;

	Response_LogInOut reg = {Protocol_Response_Register , 1}, regEmpty;
	Response_LogInOut login = {Protocol_Response_LogIn , 0}, loginEmpty;
	Response_LogInOut logout = {Protocol_Response_LogOut , 1}, logoutEmpty;
	Response_Group open = {Protocol_Response_GroupOpen , 1, "225.0.0.3", "5000", "teamZiv"}, openEmpty;
	Response_Group join = {Protocol_Response_GroupJoin , 0, "225.0.0.1", "5000" , "teamA"}, joinEmpty;
	Response_GroupNames names = {Protocol_Response_GroupsNames, "2\0aaa\0bbb"}, namesEmpty;
	Response_LeaveGroup leave = {Protocol_Response_LeaveGroup , 1}, leaveEmpty;
	strcpy(a_testName, "Test_Response_UnPack");

	Protocol_Pack(&reg, bufferR, &sizeR);
	Protocol_Pack(&login, bufferIn, &sizeIn);
	Protocol_Pack(&logout, bufferOut, &sizeOut);
	Protocol_Pack(&open, bufferOpen, &sizeOpen);
	Protocol_Pack(&join, bufferJoin, &sizeJoin);
	Protocol_Pack(&names, bufferNames, &sizeNames);
	Protocol_Pack(&leave, bufferLeave, &sizeLeave);

	if (Protocol_Success != Protocol_Unpack(bufferR, &regEmpty) || regEmpty.m_tag != Protocol_Response_Register || regEmpty.m_status != 1) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferIn, &loginEmpty) || loginEmpty.m_tag != Protocol_Response_LogIn || loginEmpty.m_status != 0) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferOut, &logoutEmpty) || logoutEmpty.m_tag != Protocol_Response_LogOut || logoutEmpty.m_status != 1) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferOpen, &openEmpty) || openEmpty.m_tag != Protocol_Response_GroupOpen || openEmpty.m_status != 1 || strcmp(openEmpty.m_ipAddress,"225.0.0.3") || strcmp(openEmpty.m_port,"5000") || strcmp(openEmpty.m_groupName,"teamZiv")) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferJoin, &joinEmpty) || joinEmpty.m_tag != Protocol_Response_GroupJoin || joinEmpty.m_status != 0 || strcmp(joinEmpty.m_ipAddress,"225.0.0.1") || strcmp(joinEmpty.m_port,"5000") || strcmp(joinEmpty.m_groupName,"teamA")) {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferNames, &namesEmpty) || namesEmpty.m_tag != Protocol_Response_GroupsNames || namesEmpty.m_allGroups[0] != '2' || namesEmpty.m_allGroups[1] != '\0' || namesEmpty.m_allGroups[4] != 'a' || namesEmpty.m_allGroups[7] != 'b') {
		return 0;
	}
	if (Protocol_Success != Protocol_Unpack(bufferLeave, &leaveEmpty) || leaveEmpty.m_tag != Protocol_Response_LeaveGroup || leaveEmpty.m_status != 1) {
		return 0;
	}
	return 1;
}
