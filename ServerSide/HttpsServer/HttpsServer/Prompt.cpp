#include "Prompt.h"

const std::string g_readError{ "Read Error!" };
const std::string g_readExit{ "Exit++" };
const std::string g_startingPrompt =
    "(◠‿◠)      To Sign Up as new client type '1'       (◠‿◠) \n" 
    "^__^       To Login into your account type '2'      ^__^ \n"
    "(╯°□°)╯    To exit the app please type 'Exit++'    (╯°□°)╯ \n";
const std::string g_whichServiceMainPropmt = 
    "(◠‿◠)         To chat with your partner type '1':                  (◠‿◠) \n" 
    "^__^          To chat with a group of friends type '2':             ^__^ \n"
    "(〜￣▽￣)〜    To have a broadcast to all online useres type '3'   (〜￣▽￣)〜 \n"
    "(╯°□°)╯       To exit type 'Exit++'                                (╯°□°)╯ \n";
const std::string g_userEmailLoginPrompt{
    "To Login please type your email below or Exit++ to close the connection: " };
const std::string g_addPartnerToChatPrompt{
    "(◠‿◠)      To add a new friend to chat type 1:     (◠‿◠)\n"
    "^__^       To chat with existing friend type 2:     ^__^\n"
    "^__^       To return to previous menu type 3:       ^__^\n"
    "(╯°□°)╯    To exit the app please type 'Exit++'    (╯°□°)╯\n"
};
const std::string g_errorUnableToLoginUser{
    "An error occurred! We are very sorry. Currently we are not able to connect you to the server. Please try later. (ᗒᗩᗕ)"
};
const std::string g_partnerUnavailable{ "The user your are trying to reach is not available! ヽ(°〇°)ﾉ"};
const std::string g_emailAddressExist{ "The email address already exist! Please select Login ヽ(°〇°)ﾉ" };
const std::string g_addNewFriendMainPrompt{ "Please type the Email or User ID of a friend you are wishing to add: ʕ •ᴥ•ʔ" };
const std::string g_userIdLoginPrompt{ "Please type your User ID below or Exit++ to close the connection: " };
const std::string g_invalidArgument{ "Invalid Argument. Please enter valid arguments! \n" };
const std::string g_emailRegistPrompt{ "Please enter your email address below: " };
const std::string g_userNamePrompt{ "What is your name? " };
const std::string g_userIdMessageCreated{ "Your User ID is: " };
const std::string g_successLogIn{ "Client Successfully Logged In" };
const std::string g_successLogInClientSide{ "Logged In Successfully" };
const std::string g_defaultConsoleUserResponse{ "The user response is: " };
const std::string g_addPartnerToChat{ "Type the Email or ID of your partner" };
const std::string g_errorConnection{ "Error! Connection Failed!" };
const std::string g_userAlreadyLoggedIn{ "You are already logged in!" };
const std::string g_errorOperation{ "Operation failed!" };