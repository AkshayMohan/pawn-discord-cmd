/*
________________________________________________________________________________

						discord-command.inc
			Command processor for SA-MP Discord Connector


@Author			:		Akshay Mohan - https://github.com/AkshayMohan
@Version        :       v1.0.2
@Github         :       https://github.com/AkshayMohan/pawn-discord-cmd

Last update: 7th of January, 2021

Changelogs:

v1.0.2 - 7th of January, 2021
	- Fixed case sensitivity.

v1.0.1 - 6th of January, 2021
	- Added function DCMD_GetCommandMessageId();

v1.0.0 - 3rd of January, 2021
	- Initial release.

MIT License

Copyright (c) 2021 Akshay Mohan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


native DCC_Message:DCMD_GetCommandMessageId();
________________________________________________________________________________    */

#if defined _included_discord_command
	#endinput
#endif

#define _included_discord_command

#if !defined DCMD_PREFIX
	#define DCMD_PREFIX '!'
#endif


#define DCMD:%0(%1,%2,%3) \
	forward DCMD_%0(DCC_User:%1,DCC_Channel:%2,%3); \
	public DCMD_%0(DCC_User:%1,DCC_Channel:%2,%3)

#if !defined isnull
	#define isnull(%0) (!(%0[0]) || (%0[0] == '\1' && !(%0[1])))
#endif


#tryinclude <discord-connector>

#if defined DCMD_STRICT_CASE
	static const DCMD_MAX_CMD_LEN = 32 - 5;
#endif
const DCMD_TOTAL_CMD_SIZE = 256;

#if defined OnDiscordCommandAttempt
	forward OnDiscordCommandAttempt(DCC_User:user, DCC_Channel:channel, cmdtext[]);
#endif

#if defined OnDiscordCommandPerformed
	forward OnDiscordCommandPerformed(DCC_User:user, DCC_Channel:channel,
										cmdtext[], success);
#endif

static DCC_Message:g_DCMDMessageID = DCC_Message:0;

stock DCC_Message:DCMD_GetCommandMessageId() {

	return g_DCMDMessageID;
}

public DCC_OnMessageCreate(DCC_Message:message) {

	new
		DCC_User:user,
		DCC_Channel:channel,
		cmdtext[DCMD_TOTAL_CMD_SIZE],
		bool:hasParams = false,
		bool:isBot;

	DCC_GetMessageContent(message, cmdtext, sizeof(cmdtext));
	if(cmdtext[0] == DCMD_PREFIX) {

		DCC_GetMessageAuthor(message, user);
		DCC_GetMessageChannel(message, channel);

		DCC_IsUserBot(user, isBot);
		#if !defined DCMD_ALLOW_BOTS
			if(isBot)
				return 0;
		#endif
		g_DCMDMessageID = message;
		#if defined OnDiscordCommandAttempt
			if(!OnDiscordCommandAttempt(user, channel, cmdtext[1]))
				return 0;
		#endif

		static command[32] = "DCMD_", success = 0;
		new pos;
		#if defined DCMD_STRICT_CASE
			pos = strfind(cmdtext, " ", false, 1);
			command[5] = '\0';
			if(pos != -1) {

				strcat(command, cmdtext[1], pos + 5);
				while(cmdtext[pos] == ' ') ++pos;
				if(cmdtext[pos]) {
					hasParams = true;
				}
			} else {
				strcat(command, cmdtext[1], DCMD_MAX_CMD_LEN);
			}
		#else
			new idx = 5;
			pos = 1;
			while(idx < 31 && cmdtext[pos] > ' ') {

				command[idx++] = (
					(cmdtext[pos] >= 'A' && cmdtext[pos] <= 'Z')
					? (cmdtext[pos] | 0x20) : cmdtext[pos]
					);
				pos++;
			}
			command[idx] = '\0';
			while(cmdtext[pos] == ' ') ++pos;
			if(cmdtext[pos]) {
				hasParams = true;
			}
		#endif
		if(hasParams) {
			success = CallLocalFunction(command, "iis",
										_:user, _:channel, cmdtext[pos]);
		} else {
			success = CallLocalFunction(command, "iis",
										_:user, _:channel, "\1");
		}
		#if defined OnDiscordCommandPerformed
			return OnDiscordCommandPerformed(
									user, channel, cmdtext[1], success);
		#else
			return 1;
		#endif
		#pragma unused success
	}
	#if defined DCMD_OnMessageCreate
		return DCMD_OnMessageCreate(message);
	#else
		return 1;
	#endif
}


#if defined _ALS_OnMessageCreate
	#undef DCC_OnMessageCreate
#else
	#define _ALS_OnMessageCreate
#endif

#define DCC_OnMessageCreate DCMD_OnMessageCreate

#if defined DCMD_OnMessageCreate
	forward DCMD_OnMessageCreate(DCC_Message:message);
#endif
