#pragma once

// IRC control characters
#define IRC_CONTROL_BOLD TEXT('\x02')
#define IRC_CONTROL_UNDERLINE TEXT('\x1F')
#define IRC_CONTROL_ITALIC TEXT('\x16')
#define IRC_CONTROL_NORMAL TEXT('\x0F')

// All control characters
#define IRC_CONTROL TEXT("\x0F\x02\x1F\x16")

// Default IRC port as used on the internet
#define IRC_DEFAULT_PORT TEXT("6667")

// Network Working Group                                               C. Kalt
// Request for Comments: 2812                                       April 2000
// Updates: 1459
// Category: Informational


//                     Internet Relay Chat: Client Protocol

// Status of this Memo

//    This memo provides information for the Internet community.  It does
//    not specify an Internet standard of any kind.  Distribution of this
//    memo is unlimited.

// Copyright Notice

//    Copyright (C) The Internet Society (2000).  All Rights Reserved.

// ...

// Replies

//    The following is a list of numeric replies which are generated in
//    response to the commands given above.  Each numeric is given with its
//    number, name and reply string.

// Command responses

//     Numerics in the range from 001 to 099 are used for client-server
//     connections only and should never travel between servers.  Replies
//     generated in the response to commands are found in the range from 200
//     to 399.

#define IRC_RPL_WELCOME TEXT("001")
//            "Welcome to the Internet Relay Network
//             <nick>!<user>@<host>"
#define IRC_RPL_YOURHOST TEXT("002")
//            "Your host is <servername>, running version <ver>"
#define IRC_RPL_CREATED TEXT("003")
//            "This server was created <date>"
#define IRC_RPL_MYINFO TEXT("004")
//            "<servername> <version> <available user modes>
//             <available channel modes>"

//       - The server sends Replies 001 to 004 to a user upon
//         successful registration.

#define IRC_RPL_BOUNCE TEXT("005")
//            "Try server <server name>, port <port number>"

//       - Sent by the server to a user to suggest an alternative
//         server.  This is often used when the connection is
//         refused because the server is already full.

#define IRC_RPL_USERHOST TEXT("302")
//            ":*1<reply> *( " " <reply> )"

//       - Reply format used by USERHOST to list replies to
//         the query list.  The reply string is composed as
//         follows:

//         reply = nickname [ "*" ] "=" ( "+" / "-" ) hostname

//         The '*' indicates whether the client has registered
//         as an Operator.  The '-' or '+' characters represent
//         whether the client has set an AWAY message or not
//         respectively.

#define IRC_RPL_ISON TEXT("303")
//            ":*1<nick> *( " " <nick> )"

//       - Reply format used by ISON to list replies to the
//         query list.

#define IRC_RPL_AWAY TEXT("301")
//            "<nick> :<away message>"
#define IRC_RPL_UNAWAY TEXT("305")
//            ":You are no longer marked as being away"
#define IRC_RPL_NOWAWAY TEXT("306")
//            ":You have been marked as being away"

//       - These replies are used with the AWAY command (if
//         allowed).  RPL_AWAY is sent to any client sending a
//         PRIVMSG to a client which is away.  RPL_AWAY is only
//         sent by the server to which the client is connected.
//         Replies RPL_UNAWAY and RPL_NOWAWAY are sent when the
//         client removes and sets an AWAY message.

#define IRC_RPL_WHOISUSER TEXT("311")
//            "<nick> <user> <host> * :<real name>"
#define IRC_RPL_WHOISSERVER TEXT("312")
//            "<nick> <server> :<server info>"
#define IRC_RPL_WHOISOPERATOR TEXT("313")
//            "<nick> :is an IRC operator"
#define IRC_RPL_WHOISIDLE TEXT("317")
//            "<nick> <integer> :seconds idle"
#define IRC_RPL_ENDOFWHOIS TEXT("318")
//            "<nick> :End of WHOIS list"
#define IRC_RPL_WHOISCHANNELS TEXT("319")
//            "<nick> :*( ( "@" / "+" ) <channel> " " )"

//       - Replies 311 - 313, 317 - 319 are all replies
//         generated in response to a WHOIS message.  Given that
//         there are enough parameters present, the answering
//         server MUST either formulate a reply out of the above
//         numerics (if the query nick is found) or return an
//         error reply.  The '*' in RPL_WHOISUSER is there as
//         the literal character and not as a wild card.  For
//         each reply set, only RPL_WHOISCHANNELS may appear
//         more than once (for long lists of channel names).
//         The '@' and '+' characters next to the channel name
//         indicate whether a client is a channel operator or
//         has been granted permission to speak on a moderated
//         channel.  The RPL_ENDOFWHOIS reply is used to mark
//         the end of processing a WHOIS message.

#define IRC_RPL_WHOWASUSER TEXT("314")
//            "<nick> <user> <host> * :<real name>"
#define IRC_RPL_ENDOFWHOWAS TEXT("369")
//            "<nick> :End of WHOWAS"

//       - When replying to a WHOWAS message, a server MUST use
//         the replies RPL_WHOWASUSER, RPL_WHOISSERVER or
//         ERR_WASNOSUCHNICK for each nickname in the presented
//         list.  At the end of all reply batches, there MUST
//         be RPL_ENDOFWHOWAS (even if there was only one reply
//         and it was an error).

#define IRC_RPL_LISTSTART TEXT("321")
//            Obsolete. Not used.

#define IRC_RPL_LIST TEXT("322")
//            "<channel> <# visible> :<topic>"
#define IRC_RPL_LISTEND TEXT("323")
//            ":End of LIST"

//       - Replies RPL_LIST, RPL_LISTEND mark the actual replies
//         with data and end of the server's response to a LIST
//         command.  If there are no channels available to return,
//         only the end reply MUST be sent.

#define IRC_RPL_UNIQOPIS TEXT("325")
//            "<channel> <nickname>"

#define IRC_RPL_CHANNELMODEIS TEXT("324")
//            "<channel> <mode> <mode params>"

#define IRC_RPL_NOTOPIC TEXT("331")
//            "<channel> :No topic is set"
#define IRC_RPL_TOPIC TEXT("332")
//            "<channel> :<topic>"

//       - When sending a TOPIC message to determine the
//         channel topic, one of two replies is sent.  If
//         the topic is set, RPL_TOPIC is sent back else
//         RPL_NOTOPIC.

#define IRC_RPL_INVITING TEXT("341")
//            "<channel> <nick>"

//       - Returned by the server to indicate that the
//         attempted INVITE message was successful and is
//         being passed onto the end client.

#define IRC_RPL_SUMMONING TEXT("342")
//            "<user> :Summoning user to IRC"

//       - Returned by a server answering a SUMMON message to
//         indicate that it is summoning that user.

#define IRC_RPL_INVITELIST TEXT("346")
//            "<channel> <invitemask>"
#define IRC_RPL_ENDOFINVITELIST TEXT("347")
//            "<channel> :End of channel invite list"

//       - When listing the 'invitations masks' for a given channel,
//         a server is required to send the list back using the
//         RPL_INVITELIST and RPL_ENDOFINVITELIST messages.  A
//         separate RPL_INVITELIST is sent for each active mask.
//         After the masks have been listed (or if none present) a
//         RPL_ENDOFINVITELIST MUST be sent.

#define IRC_RPL_EXCEPTLIST TEXT("348")
//            "<channel> <exceptionmask>"
#define IRC_RPL_ENDOFEXCEPTLIST TEXT("349")
//            "<channel> :End of channel exception list"

//       - When listing the 'exception masks' for a given channel,
//         a server is required to send the list back using the
//         RPL_EXCEPTLIST and RPL_ENDOFEXCEPTLIST messages.  A
//         separate RPL_EXCEPTLIST is sent for each active mask.
//         After the masks have been listed (or if none present)
//         a RPL_ENDOFEXCEPTLIST MUST be sent.

#define IRC_RPL_VERSION TEXT("351")
//            "<version>.<debuglevel> <server> :<comments>"

//       - Reply by the server showing its version details.
//         The <version> is the version of the software being
//         used (including any patchlevel revisions) and the
//         <debuglevel> is used to indicate if the server is
//         running in "debug mode".

//         The "comments" field may contain any comments about
//         the version or further version details.

#define IRC_RPL_WHOREPLY TEXT("352")
//            "<channel> <user> <host> <server> <nick>
//            ( "H" / "G" > ["*"] [ ( "@" / "+" ) ]
//            :<hopcount> <real name>"

#define IRC_RPL_ENDOFWHO TEXT("315")
//            "<name> :End of WHO list"

//       - The RPL_WHOREPLY and RPL_ENDOFWHO pair are used
//         to answer a WHO message.  The RPL_WHOREPLY is only
//         sent if there is an appropriate match to the WHO
//         query.  If there is a list of parameters supplied
//         with a WHO message, a RPL_ENDOFWHO MUST be sent
//         after processing each list item with <name> being
//         the item.

#define IRC_RPL_NAMREPLY TEXT("353")
//            "( "=" / "*" / "@" ) <channel>
//             :[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )
//       - "@" is used for secret channels, "*" for private
//         channels, and "=" for others (public channels).

#define IRC_RPL_ENDOFNAMES TEXT("366")
//            "<channel> :End of NAMES list"

//       - To reply to a NAMES message, a reply pair consisting
//         of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the
//         server back to the client.  If there is no channel
//         found as in the query, then only RPL_ENDOFNAMES is
//         returned.  The exception to this is when a NAMES
//         message is sent with no parameters and all visible
//         channels and contents are sent back in a series of
//         RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark
//         the end.

#define IRC_RPL_LINKS TEXT("364")
//            "<mask> <server> :<hopcount> <server info>"
#define IRC_RPL_ENDOFLINKS TEXT("365")
//            "<mask> :End of LINKS list"

//       - In replying to the LINKS message, a server MUST send
//         replies back using the RPL_LINKS numeric and mark the
//         end of the list using an RPL_ENDOFLINKS reply.

#define IRC_RPL_BANLIST TEXT("367")
//            "<channel> <banmask>"
#define IRC_RPL_ENDOFBANLIST TEXT("368")
//            "<channel> :End of channel ban list"

//       - When listing the active 'bans' for a given channel,
//         a server is required to send the list back using the
//         RPL_BANLIST and RPL_ENDOFBANLIST messages.  A separate
//         RPL_BANLIST is sent for each active banmask.  After the
//         banmasks have been listed (or if none present) a
//         RPL_ENDOFBANLIST MUST be sent.

#define IRC_RPL_INFO TEXT("371")
//            ":<string>"
#define IRC_RPL_ENDOFINFO TEXT("374")
//            ":End of INFO list"

//       - A server responding to an INFO message is required to
//         send all its 'info' in a series of RPL_INFO messages
//         with a RPL_ENDOFINFO reply to indicate the end of the
//         replies.

#define IRC_RPL_MOTDSTART TEXT("375")
//            ":- <server> Message of the day - "
#define IRC_RPL_MOTD TEXT("372")
//            ":- <text>"
#define IRC_RPL_ENDOFMOTD TEXT("376")
//            ":End of MOTD command"

//       - When responding to the MOTD message and the MOTD file
//         is found, the file is displayed line by line, with
//         each line no longer than 80 characters, using
//         RPL_MOTD format replies.  These MUST be surrounded
//         by a RPL_MOTDSTART (before the RPL_MOTDs) and an
//         RPL_ENDOFMOTD (after).

#define IRC_RPL_YOUREOPER TEXT("381")
//            ":You are now an IRC operator"

//       - RPL_YOUREOPER is sent back to a client which has
//         just successfully issued an OPER message and gained
//         operator status.

#define IRC_RPL_REHASHING TEXT("382")
//            "<config file> :Rehashing"

//       - If the REHASH option is used and an operator sends
//         a REHASH message, an RPL_REHASHING is sent back to
//         the operator.

#define IRC_RPL_YOURESERVICE TEXT("383")
//            "You are service <servicename>"

//       - Sent by the server to a service upon successful
//         registration.

#define IRC_RPL_TIME TEXT("391")
//            "<server> :<string showing server's local time>"

//       - When replying to the TIME message, a server MUST send
//         the reply using the RPL_TIME format above.  The string
//         showing the time need only contain the correct day and
//         time there.  There is no further requirement for the
//         time string.

#define IRC_RPL_USERSSTART TEXT("392")
//            ":UserID   Terminal  Host"
#define IRC_RPL_USERS TEXT("393")
//            ":<username> <ttyline> <hostname>"
#define IRC_RPL_ENDOFUSERS TEXT("394")
//            ":End of users"
#define IRC_RPL_NOUSERS TEXT("395")
//            ":Nobody logged in"

//       - If the USERS message is handled by a server, the
//         replies RPL_USERSTART, RPL_USERS, RPL_ENDOFUSERS and
//         RPL_NOUSERS are used.  RPL_USERSSTART MUST be sent
//         first, following by either a sequence of RPL_USERS
//         or a single RPL_NOUSER.  Following this is
//         RPL_ENDOFUSERS.

#define IRC_RPL_TRACELINK TEXT("200")
//            "Link <version & debug level> <destination>
//             <next server> V<protocol version>
//             <link uptime in seconds> <backstream sendq>
//             <upstream sendq>"
#define IRC_RPL_TRACECONNECTING TEXT("201")
//            "Try. <class> <server>"
#define IRC_RPL_TRACEHANDSHAKE TEXT("202")
//            "H.S. <class> <server>"
#define IRC_RPL_TRACEUNKNOWN TEXT("203")
//            "???? <class> [<client IP address in dot form>]"
#define IRC_RPL_TRACEOPERATOR TEXT("204")
//            "Oper <class> <nick>"
#define IRC_RPL_TRACEUSER TEXT("205")
//            "User <class> <nick>"
#define IRC_RPL_TRACESERVER TEXT("206")
//            "Serv <class> <int>S <int>C <server>
//             <nick!user|*!*>@<host|server> V<protocol version>"
#define IRC_RPL_TRACESERVICE TEXT("207")
//            "Service <class> <name> <type> <active type>"
#define IRC_RPL_TRACENEWTYPE TEXT("208")
//            "<newtype> 0 <client name>"
#define IRC_RPL_TRACECLASS TEXT("209")
//            "Class <class> <count>"
#define IRC_RPL_TRACERECONNECT TEXT("210")
//            Unused.
#define IRC_RPL_TRACELOG TEXT("261")
//            "File <logfile> <debug level>"
#define IRC_RPL_TRACEEND TEXT("262")
//            "<server name> <version & debug level> :End of TRACE"

//       - The RPL_TRACE* are all returned by the server in
//         response to the TRACE message.  How many are
//         returned is dependent on the TRACE message and
//         whether it was sent by an operator or not.  There
//         is no predefined order for which occurs first.
//         Replies RPL_TRACEUNKNOWN, RPL_TRACECONNECTING and
//         RPL_TRACEHANDSHAKE are all used for connections
//         which have not been fully established and are either
//         unknown, still attempting to connect or in the
//         process of completing the 'server handshake'.
//         RPL_TRACELINK is sent by any server which handles
//         a TRACE message and has to pass it on to another
//         server.  The list of RPL_TRACELINKs sent in
//         response to a TRACE command traversing the IRC
//         network should reflect the actual connectivity of
//         the servers themselves along that path.
//         RPL_TRACENEWTYPE is to be used for any connection
//         which does not fit in the other categories but is
//         being displayed anyway.
//         RPL_TRACEEND is sent to indicate the end of the list.

#define IRC_RPL_STATSLINKINFO TEXT("211")
//            "<linkname> <sendq> <sent messages>
//             <sent Kbytes> <received messages>
//             <received Kbytes> <time open>"

//       - reports statistics on a connection.  <linkname>
//         identifies the particular connection, <sendq> is
//         the amount of data that is queued and waiting to be
//         sent <sent messages> the number of messages sent,
//         and <sent Kbytes> the amount of data sent, in
//         Kbytes. <received messages> and <received Kbytes>
//         are the equivalent of <sent messages> and <sent
//         Kbytes> for received data, respectively.  <time
//         open> indicates how long ago the connection was
//         opened, in seconds.

#define IRC_RPL_STATSCOMMANDS TEXT("212")
//            "<command> <count> <byte count> <remote count>"

//       - reports statistics on commands usage.

#define IRC_RPL_ENDOFSTATS TEXT("219")
//            "<stats letter> :End of STATS report"

#define IRC_RPL_STATSUPTIME TEXT("242")
//            ":Server Up %d days %d:%02d:%02d"

//       - reports the server uptime.

#define IRC_RPL_STATSOLINE TEXT("243")
//            "O <hostmask> * <name>"

//       - reports the allowed hosts from where user may become IRC
//         operators.

#define IRC_RPL_UMODEIS TEXT("221")
//            "<user mode string>"

//       - To answer a query about a client's own mode,
//         RPL_UMODEIS is sent back.

#define IRC_RPL_SERVLIST TEXT("234")
//            "<name> <server> <mask> <type> <hopcount> <info>"
#define IRC_RPL_SERVLISTEND TEXT("235")
//            "<mask> <type> :End of service listing"

//       - When listing services in reply to a SERVLIST message,
//         a server is required to send the list back using the
//         RPL_SERVLIST and RPL_SERVLISTEND messages.  A separate
//         RPL_SERVLIST is sent for each service.  After the
//         services have been listed (or if none present) a
//         RPL_SERVLISTEND MUST be sent.

#define IRC_RPL_LUSERCLIENT TEXT("251")
//            ":There are <integer> users and <integer>
//             services on <integer> servers"
#define IRC_RPL_LUSEROP TEXT("252")
//            "<integer> :operator(s) online"
#define IRC_RPL_LUSERUNKNOWN TEXT("253")
//            "<integer> :unknown connection(s)"
#define IRC_RPL_LUSERCHANNELS TEXT("254")
//            "<integer> :channels formed"
#define IRC_RPL_LUSERME TEXT("255")
//            ":I have <integer> clients and <integer>
//              servers"

//       - In processing an LUSERS message, the server
//         sends a set of replies from RPL_LUSERCLIENT,
//         RPL_LUSEROP, RPL_USERUNKNOWN,
//         RPL_LUSERCHANNELS and RPL_LUSERME.  When
//         replying, a server MUST send back
//         RPL_LUSERCLIENT and RPL_LUSERME.  The other
//         replies are only sent back if a non-zero count
//         is found for them.

#define IRC_RPL_ADMINME TEXT("256")
//            "<server> :Administrative info"
#define IRC_RPL_ADMINLOC1 TEXT("257")
//            ":<admin info>"
#define IRC_RPL_ADMINLOC2 TEXT("258")
//            ":<admin info>"
#define IRC_RPL_ADMINEMAIL TEXT("259")
//            ":<admin info>"

//       - When replying to an ADMIN message, a server
//         is expected to use replies RPL_ADMINME
//         through to RPL_ADMINEMAIL and provide a text
//         message with each.  For RPL_ADMINLOC1 a
//         description of what city, state and country
//         the server is in is expected, followed by
//         details of the institution (RPL_ADMINLOC2)
//         and finally the administrative contact for the
//         server (an email address here is REQUIRED)
//         in RPL_ADMINEMAIL.

#define IRC_RPL_TRYAGAIN TEXT("263")
//            "<command> :Please wait a while and try again."

//       - When a server drops a command without processing it,
//         it MUST use the reply RPL_TRYAGAIN to inform the
//         originating client.

// Error Replies

//     Error replies are found in the range from 400 to 599.

#define IRC_ERR_NOSUCHNICK TEXT("401")
//            "<nickname> :No such nick/channel"

//        - Used to indicate the nickname parameter supplied to a
//          command is currently unused.

#define IRC_ERR_NOSUCHSERVER TEXT("402")
//            "<server name> :No such server"

//       - Used to indicate the server name given currently
//         does not exist.

#define IRC_ERR_NOSUCHCHANNEL TEXT("403")
//            "<channel name> :No such channel"

//       - Used to indicate the given channel name is invalid.

#define IRC_ERR_CANNOTSENDTOCHAN TEXT("404")
//            "<channel name> :Cannot send to channel"

//       - Sent to a user who is either (a) not on a channel
//         which is mode +n or (b) not a chanop (or mode +v) on
//         a channel which has mode +m set or where the user is
//         banned and is trying to send a PRIVMSG message to
//         that channel.

#define IRC_ERR_TOOMANYCHANNELS TEXT("405")
//            "<channel name> :You have joined too many channels"

//       - Sent to a user when they have joined the maximum
//         number of allowed channels and they try to join
//         another channel.

#define IRC_ERR_WASNOSUCHNICK TEXT("406")
//            "<nickname> :There was no such nickname"

//       - Returned by WHOWAS to indicate there is no history
//         information for that nickname.

#define IRC_ERR_TOOMANYTARGETS TEXT("407")
//            "<target> :<error code> recipients. <abort message>"

//       - Returned to a client which is attempting to send a
//         PRIVMSG/NOTICE using the user@host destination format
//         and for a user@host which has several occurrences.

//       - Returned to a client which trying to send a
//         PRIVMSG/NOTICE to too many recipients.

//       - Returned to a client which is attempting to JOIN a safe
//         channel using the shortname when there are more than one
//         such channel.

#define IRC_ERR_NOSUCHSERVICE TEXT("408")
//            "<service name> :No such service"

//       - Returned to a client which is attempting to send a SQUERY
//         to a service which does not exist.

#define IRC_ERR_NOORIGIN TEXT("409")
//            ":No origin specified"

//       - PING or PONG message missing the originator parameter.

#define IRC_ERR_NORECIPIENT TEXT("411")
//            ":No recipient given (<command>)"
#define IRC_ERR_NOTEXTTOSEND TEXT("412")
//            ":No text to send"
#define IRC_ERR_NOTOPLEVEL TEXT("413")
//            "<mask> :No toplevel domain specified"
#define IRC_ERR_WILDTOPLEVEL TEXT("414")
//            "<mask> :Wildcard in toplevel domain"
#define IRC_ERR_BADMASK TEXT("415")
//            "<mask> :Bad Server/host mask"

//       - 412 - 415 are returned by PRIVMSG to indicate that
//         the message wasn't delivered for some reason.
//         ERR_NOTOPLEVEL and ERR_WILDTOPLEVEL are errors that
//         are returned when an invalid use of
//         "PRIVMSG $<server>" or "PRIVMSG #<host>" is attempted.

#define IRC_ERR_UNKNOWNCOMMAND TEXT("421")
//            "<command> :Unknown command"

//       - Returned to a registered client to indicate that the
//         command sent is unknown by the server.

#define IRC_ERR_NOMOTD TEXT("422")
//            ":MOTD File is missing"

//       - Server's MOTD file could not be opened by the server.

#define IRC_ERR_NOADMININFO TEXT("423")
//            "<server> :No administrative info available"

//       - Returned by a server in response to an ADMIN message
//         when there is an error in finding the appropriate
//         information.

#define IRC_ERR_FILEERROR TEXT("424")
//            ":File error doing <file op> on <file>"

//       - Generic error message used to report a failed file
//         operation during the processing of a message.

#define IRC_ERR_NONICKNAMEGIVEN TEXT("431")
//            ":No nickname given"

//       - Returned when a nickname parameter expected for a
//         command and isn't found.

#define IRC_ERR_ERRONEUSNICKNAME TEXT("432")
//            "<nick> :Erroneous nickname"

//       - Returned after receiving a NICK message which contains
//         characters which do not fall in the defined set.  See
//         section 2.3.1 for details on valid nicknames.

#define IRC_ERR_NICKNAMEINUSE TEXT("433")
//            "<nick> :Nickname is already in use"

//       - Returned when a NICK message is processed that results
//         in an attempt to change to a currently existing
//         nickname.

#define IRC_ERR_NICKCOLLISION TEXT("436")
//            "<nick> :Nickname collision KILL from <user>@<host>"

//       - Returned by a server to a client when it detects a
//         nickname collision (registered of a NICK that
//         already exists by another server).

#define IRC_ERR_UNAVAILRESOURCE TEXT("437")
//            "<nick/channel> :Nick/channel is temporarily unavailable"

//       - Returned by a server to a user trying to join a channel
//         currently blocked by the channel delay mechanism.

//       - Returned by a server to a user trying to change nickname
//         when the desired nickname is blocked by the nick delay
//         mechanism.

#define IRC_ERR_USERNOTINCHANNEL TEXT("441")
//            "<nick> <channel> :They aren't on that channel"

//       - Returned by the server to indicate that the target
//         user of the command is not on the given channel.

#define IRC_ERR_NOTONCHANNEL TEXT("442")
//            "<channel> :You're not on that channel"

//       - Returned by the server whenever a client tries to
//         perform a channel affecting command for which the
//         client isn't a member.

#define IRC_ERR_USERONCHANNEL TEXT("443")
//            "<user> <channel> :is already on channel"

//       - Returned when a client tries to invite a user to a
//         channel they are already on.

#define IRC_ERR_NOLOGIN TEXT("444")
//            "<user> :User not logged in"

//       - Returned by the summon after a SUMMON command for a
//         user was unable to be performed since they were not
//         logged in.

#define IRC_ERR_SUMMONDISABLED TEXT("445")
//            ":SUMMON has been disabled"

//       - Returned as a response to the SUMMON command.  MUST be
//         returned by any server which doesn't implement it.

#define IRC_ERR_USERSDISABLED TEXT("446")
//            ":USERS has been disabled"

//       - Returned as a response to the USERS command.  MUST be
//         returned by any server which does not implement it.

#define IRC_ERR_NOTREGISTERED TEXT("451")
//            ":You have not registered"

//       - Returned by the server to indicate that the client
//         MUST be registered before the server will allow it
//         to be parsed in detail.

#define IRC_ERR_NEEDMOREPARAMS TEXT("461")
//            "<command> :Not enough parameters"

//       - Returned by the server by numerous commands to
//         indicate to the client that it didn't supply enough
//         parameters.

#define IRC_ERR_ALREADYREGISTRED TEXT("462")
//            ":Unauthorized command (already registered)"

//       - Returned by the server to any link which tries to
//         change part of the registered details (such as
//         password or user details from second USER message).

#define IRC_ERR_NOPERMFORHOST TEXT("463")
//            ":Your host isn't among the privileged"

//       - Returned to a client which attempts to register with
//         a server which does not been setup to allow
//         connections from the host the attempted connection
//         is tried.

#define IRC_ERR_PASSWDMISMATCH TEXT("464")
//            ":Password incorrect"

//       - Returned to indicate a failed attempt at registering
//         a connection for which a password was required and
//         was either not given or incorrect.

#define IRC_ERR_YOUREBANNEDCREEP TEXT("465")
//            ":You are banned from this server"

//       - Returned after an attempt to connect and register
//         yourself with a server which has been setup to
//         explicitly deny connections to you.

#define IRC_ERR_YOUWILLBEBANNED TEXT("466")

//       - Sent by a server to a user to inform that access to the
//         server will soon be denied.

#define IRC_ERR_KEYSET TEXT("467")
//            "<channel> :Channel key already set"
#define IRC_ERR_CHANNELISFULL TEXT("471")
//            "<channel> :Cannot join channel (+l)"
#define IRC_ERR_UNKNOWNMODE TEXT("472")
//            "<char> :is unknown mode char to me for <channel>"
#define IRC_ERR_INVITEONLYCHAN TEXT("473")
//            "<channel> :Cannot join channel (+i)"
#define IRC_ERR_BANNEDFROMCHAN TEXT("474")
//            "<channel> :Cannot join channel (+b)"
#define IRC_ERR_BADCHANNELKEY TEXT("475")
//            "<channel> :Cannot join channel (+k)"
#define IRC_ERR_BADCHANMASK TEXT("476")
//            "<channel> :Bad Channel Mask"
#define IRC_ERR_NOCHANMODES TEXT("477")
//            "<channel> :Channel doesn't support modes"
#define IRC_ERR_BANLISTFULL TEXT("478")
//            "<channel> <char> :Channel list is full"

#define IRC_ERR_NOPRIVILEGES TEXT("481")
//            ":Permission Denied- You're not an IRC operator"

//       - Any command requiring operator privileges to operate
//         MUST return this error to indicate the attempt was
//         unsuccessful.

#define IRC_ERR_CHANOPRIVSNEEDED TEXT("482")
//            "<channel> :You're not channel operator"

//       - Any command requiring 'chanop' privileges (such as
//         MODE messages) MUST return this error if the client
//         making the attempt is not a chanop on the specified
//         channel.

#define IRC_ERR_CANTKILLSERVER TEXT("483")
//            ":You can't kill a server!"

//       - Any attempts to use the KILL command on a server
//         are to be refused and this error returned directly
//         to the client.

#define IRC_ERR_RESTRICTED TEXT("484")
//            ":Your connection is restricted!"

//       - Sent by the server to a user upon connection to indicate
//         the restricted nature of the connection (user mode "+r").

#define IRC_ERR_UNIQOPPRIVSNEEDED TEXT("485")
//            ":You're not the original channel operator"

//       - Any MODE requiring "channel creator" privileges MUST
//         return this error if the client making the attempt is not
//         a chanop on the specified channel.

#define IRC_ERR_NOOPERHOST TEXT("491")
//            ":No O-lines for your host"

//       - If a client sends an OPER message and the server has
//         not been configured to allow connections from the
//         client's host as an operator, this error MUST be
//         returned.

#define IRC_ERR_UMODEUNKNOWNFLAG TEXT("501")
//            ":Unknown MODE flag"

//       - Returned by the server to indicate that a MODE
//         message was sent with a nickname parameter and that
//         the a mode flag sent was not recognized.

#define IRC_ERR_USERSDONTMATCH TEXT("502")
//            ":Cannot change mode for other users"

//       - Error sent to any user trying to view or change the
//         user mode for a user other than themselves.

// Reserved numerics

//     These numerics are not described above since they fall into one of
//     the following categories:

//     1. no longer in use;

//     2. reserved for future planned use;

//     3. in current use but are part of a non-generic 'feature' of
//        the current IRC server.

#define IRC_RPL_SERVICEINFO		TEXT("231")
#define IRC_RPL_ENDOFSERVICES	TEXT("232")
#define IRC_RPL_SERVICE			TEXT("233")
#define IRC_RPL_NONE			TEXT("300")
#define IRC_RPL_WHOISCHANOP		TEXT("316")
#define IRC_RPL_KILLDONE		TEXT("361")
#define IRC_RPL_CLOSING			TEXT("362")
#define IRC_RPL_CLOSEEND		TEXT("363")
#define IRC_RPL_INFOSTART		TEXT("373")
#define IRC_RPL_MYPORTIS		TEXT("384")

#define IRC_RPL_STATSCLINE		TEXT("213")
#define IRC_RPL_STATSNLINE		TEXT("214")
#define IRC_RPL_STATSILINE		TEXT("215")
#define IRC_RPL_STATSKLINE		TEXT("216")
#define IRC_RPL_STATSQLINE		TEXT("217")
#define IRC_RPL_STATSYLINE		TEXT("218")
#define IRC_RPL_STATSVLINE		TEXT("240")
#define IRC_RPL_STATSLLINE		TEXT("241")
#define IRC_RPL_STATSHLINE		TEXT("244")
#define IRC_RPL_STATSSLINE		TEXT("244")
#define IRC_RPL_STATSPING		TEXT("246")
#define IRC_RPL_STATSBLINE		TEXT("247")
#define IRC_RPL_STATSDLINE		TEXT("250")

#define IRC_ERR_NOSERVICEHOST	TEXT("492")