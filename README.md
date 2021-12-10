# pawn-discord-cmd
Command processor for SA-MP Discord Connector.

**pawn-discord-cmd** is a fast command processor that supports creating discord commands in PAWN with ease. This is made to support [samp-discord-connector](https://github.com/maddinat0r/samp-discord-connector) plugin.

## Installation
### Using sampctl
```
sampctl package install AkshayMohan/pawn-discord-cmd
```

### Manual install
- Download and copy `discord-cmd.inc` to `pawno/include` directory.

## Inclusion
To include it in any script, first include `discord-connector` and then include `discord-cmd` as shown below:

```pawn
#include <a_samp>
#include <discord-connector> //discord-cmd will try to include if you don't include this yourself.
#include <discord-cmd>
```
Check out the example below to understand usage.

## Configuration

### Default settings
- Command prefix: `!`
- Case-sensitive commands: `false`
- Accept commands from bots: `false`

As of v1.0.0, there are three optional configs that can be defined before including `discord-cmd`.

```pawn
#define DCMD_PREFIX '~' //If you don't define, by default it is '!'
#define DCMD_STRICT_CASE //Defining this will make commands case-sensitive. !test and !TEST will become different
#define DCMD_ALLOW_BOTS //Defining this will not ignore commands sent on channel by bots.

#include <discord-cmd>
```

## Callbacks

There are two callbacks:
- `OnDiscordCommandAttempt(DCC_User:user, DCC_Channel:channel, cmdtext[]);`
    - This callback is fired before the entered command is processed. Returning 0 here will be ignore the command attempt.

- `OnDiscordCommandPerformed(DCC_User:user, DCC_Channel:channel, cmdtext[], success);`
    - This callback is fired after performing the command.
    - `success = 0` denotes a failure.

## Functions
- `DCC_Message:DCMD_GetCommandMessageId()` - Returns `DCC_Message:message` identifier for the last used command. This can be used under `DCMD` commands or the callbacks this include provides.

## Syntax
```pawn
DCMD:command(user, channel, params[]);
```
   - `user` is of type `DCC_User` and denotes the user that executes this command.
   - `channel` is of type `DCC_Channel` and denotes the channel from where this command is executed.
   - `params` contains optional command arguments.

## Example
```pawn
#include <a_samp>
#include <discord-connector>
#include <discord-cmd>

//!test
DCMD:test(user, channel, params[]) {

    DCC_SendChannelMessage(channel, "Hello from server!");
    return 1;
}

DCMD:delete(user, channel, params[]) {

    new
        DCC_Message:message;
    message = DCMD_GetCommandMessageId();
	//delete !delete message of user.
	DCC_DeleteMessage(message);
	DCC_SendChannelMessage(channel, "deleted!");
	return 1;
}

DCMD:say(user, channel, params[]) {

    if(isnull(params)) {
        DCC_SendChannelMessage(channel, "SYNTAX: !say [msg]");
    } else {

        new str[144], username[33];
        DCC_GetUserName(user, username, sizeof(username));
        format(str, sizeof(str), "[DISCORD] %s says: %s", username, params);
        SendClientMessageToAll(-1, str); //Broadcast message to server.
    }
    return 1;
}

public OnDiscordCommandAttempt(DCC_User:user, DCC_Channel:channel, cmdtext[]) {

    if(!strcmp(cmdtext, "secret", 6)) {
    
        //if user has no particular role or privilege:
        return 0;
    }
    return 1;
}

public OnDiscordCommandPerformed(DCC_User:user, DCC_Channel:channel, cmdtext[], success) {

    if(!success) {
    
        DCC_SendChannelMessage(channel, "This command does not exist!");
    }
    return 1;
}
```
