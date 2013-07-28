# Aufbau

`{ 'type': [String], 'arguments':  [Object] }`

# Anfragen (Client -> Server)

## Bot Befehl

    {
      'type': 'bot',
      'arguments':  {
        [[ 'sid': [String], ]]
        'identifier': [String],
        'execute': {
          'command': [String],
          'argument': [String]
        }
      }
    }


## Bot Verwaltung

### Bot Anlegen

    {
      'type': 'bot_management',
      'arguments': {
        [[ 'sid': [String], ]]
        'type': 'create',
        'config': [String]
      }
    }

### Bot löschen

    {
      'type': 'bot_management',
      'arguments': {
        [[ 'sid': [String], ]]
        'type': 'delete',
        'identifier': [String]
      }
    }


# Update (Server -> Client)

## Bots Update

    {
      'type': 'bots',
      'arguments': {
        [String]: [Object],
        ...
      }
    }

Sendet Map { $(identifier): $(configuration), ... }


## Update

    {
      'type': 'update',
      'arguments': {
        'identifier': [String],
        'key': [String],
        'value': [String]
      }
    }

**Beispiel:**

    {
      'identifier': $(identifier),
      'key': $(modul)_$(config_key),
      'value': $(new_value)
    }


## Event

    {
      'type': 'event',
      'arguments': {
        'identifier': [String],
        'key': [String],
        'value': [String]
      }
    }


**Beispiel**

    {
      'identifier': $(identifier),
      'key': $(event_name),
      'value': $(event_value)
    }

## Account/Bot Antwort

    {
      'type': 'account',
      'arguments': {
        'key': [String],
        'success': [Boolean] or [String]
      }
    }

**Beispiel**

    {
      'type': 'account',
      'arguments': {
        'key': "login",
        'success': false
      }
    }

**Beispiel**

    {
      'type': 'account',
      'arguments': {
        'key': "create_bot",
        'success': "error message"
      }
    }

# Generating the JNI header

In `src/net/makielski/botscript`:  

    javac BotscriptServer.java


In `src` javac BotscriptServer.java`:  

    javah -jni net.makielski.botscript.BotscriptServer


# MinGW command to compile boost
    NO_BZIP2=1 ./b2 --with-system --with-thread --with-regex --with-iostreams --with-filesystem --with-chrono --with-date_time --with-random variant=release link=static threading=multi runtime-link=static --toolset=gcc-mingw64 define=BOOST_USE_WINDOWS_H threadapi=win32 target-os=windows -s ZLIB_SOURCE=[PATH_TO_ZLIB]
