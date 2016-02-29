#!/bin/bash

### Defaults
SEARCH_USER=false
SEARCH_BOT=false
HOST=127.0.0.1
PORT=9004

### Do argument parsing
for i in "$@"
do
case $i in
  ## HELP
  -h|--help)
  HELP=true
  shift
  ;;
  
  ## CONNECTION
  -h=*|--host=*)
  HOST="${i#*=}"
  shift
  ;;
  
  -p=*|--port=*)
  PORT="${i#*=}"
  shift
  ;;
  
  ## WHAT TO DO
  -d|--delete)
  DELETE=true
  shift
  ;;

  ## SUBJECT?
  ## user XOR bot
  -u=*|--user=*)
  SEARCH_USER=true
  USER_NAME="${i#*=}"
  shift
  ;;
  -b=*|--bot=*)
  SEARCH_BOT=true
  BOT_NAME="${i#*=}"
  shift
  ;;
esac
done

# Print help text
if [ $HELP ]
then
  printf "\n\tMakielskis Bot - Database Utility 0.0.1\n"
  printf "\n"
  printf "\n  --help        | -h       \t this help text"
  printf "\n"
  printf "\n Connection Settings:"
  printf "\n  --host=[host] | -h=[host]\t host to connect to [default = %s]" "${HOST}"
  printf "\n  --port=[port] | -p=[port]\t port to connect to [default = %s]" "${PORT}"
  printf "\n"
  printf "\n Database Actions:"
  printf "\n  --user=[user] | -u=[user]\t the username of the user to show/delete"
  printf "\n  --bot=[bot]   | -b=[bot] \t the bot ID of the bot to show/delete"
  printf "\n  --delete      | -d       \t delete selected bot/user"
  printf "\n"
  printf "\n  Examples:"
  printf "\n    ./botscript-db.sh --user=John                 SHOW user John"
  printf "\n    ./botscript-db.sh --user=John --delete        DELETE user John"
  printf "\n    ./botscript-db.sh --bot=pg_b_penner           SHOW bot pg_sy_penner"
  printf "\n    ./botscript-db.sh --bot=pg_b_penner --delete  DELETE bot pg_sy_penner"
  printf "\n\n"
  exit 0
fi

### CHECK ARGUMENTS
if [[ $SEARCH_USER = false && $SEARCH_BOT = false ]]
then
  echo "You must either specify a user name or a bot name."
  exit 1
fi

if [[ $SEARCH_USER = true && $SEARCH_BOT = true ]]
then
  echo "You must either specify a user name or a bot name."
  echo "Not both at the same time."
  exit 1
fi

### BUILD SCRIPT
read -d '' SCRIPT <<"EOF"
function run(db)
  local delete = %s
  local search_user = '%s'
  local search_bot = '%s'
  local users = db:get_document("users"):children()

  local output = ''

  for user_index = 0, #users - 1 do
    print("analyzing " .. tostring(users[user_index]))
    if search_user ~= '*' and users[user_index]:index() ~= search_user then
      -- Do nothing (user is no wildcard [*] and username does not match)
      print("skip")
    else
      if search_user ~= '*' then
        -- User matches, we're searching for a user (not a bot)
        -- Print (and delete if requested)
        output = output .. tostring(users[user_index])
        if delete then
          users[user_index]:remove()
        end
        return output
      end
      local bots = users[user_index]:get('bots'):children()

      for bot_index = 0, #bots - 1 do
        local bot = bots[bot_index]
        if bot:index() == search_bot then
          output = tostring(bot)
          
          if delete then
            bots[bot_index]:remove()
          end
          
          return output
        end
      end
    end
  end

  output = output .. "...not found"
  return output
end
EOF

if [[ $DELETE = true ]]
then
  DELETE_REPLACE='true'
else
  DELETE_REPLACE='false'
fi

if [[ $SEARCH_USER = true ]]
then
  USER_REPLACE=$USER_NAME
else
  USER_REPLACE="*"
fi

if [[ $SEARCH_BOT = true ]]
then
  BOT_REPLACE=$BOT_NAME
else
  BOT_REPLACE=""
fi

### Ask for database password
stty -echo
read -p "Database Password: " passw; echo 1>&2
stty echo

### Execute
printf "${SCRIPT}" "$DELETE_REPLACE" "$USER_REPLACE" "$BOT_REPLACE" > /tmp/tmp.lua
printf "Connecting to ${HOST}:${PORT}...\n" 1>&2
curl --silent --data-urlencode @/tmp/tmp.lua -u admin:"$passw" --url http://${HOST}:${PORT}
rm /tmp/tmp.lua
echo
