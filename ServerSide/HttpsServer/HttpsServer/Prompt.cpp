#include "Prompt.h"

const std::string g_readError{ "Read Error!" };
const std::string g_readExit{ "Exit++" };
const std::string g_startingPrompt =
    "^__^ To Sign Up as new client type '1' ^__^ \n" 
    "^__^ To Login into your account type '2' ^__^ \n"
    "^__^ To exit the app please type 'Exit++' ^__^ \n";
const std::string g_whichServiceMainPropmt = 
    "^__^ To chat with your partner type '1': ^__^ \n" 
    "^__^ To chat with a group of friends type '2': ^__^ \n"
    "^__^ To have a broadcast to all online useres type '3' ^__^ \n"
    "^__^ To exit type 'Exit++' ^__^ \n";
const std::string g_userEmailLoginPrompt{
    "To Login please type your email below or Exit++ to close the connection: " };
const std::string g_userIdLoginPrompt{ "Please type your User ID below or Exit++ to close the connection: " };
const std::string g_invalidArgument{ "Invalid Argument. Please enter valid arguments! \n" };
const std::string g_emailRegistPrompt{ "Please enter your email address below: " };
const std::string g_userNamePrompt{ "What is your name? " };
const std::string g_userIdMessageCreated{ "Your User ID is: " };
const std::string g_successLogIn{ "Client Successfully Logged In" };
const std::string g_successLogInClientSide{ "Logged In Successfully" };
const std::string g_emailAddressExist{ "The email address already exist! Please select Login" };
const std::string g_defaultConsoleUserResponse{ "The user response is: " };
