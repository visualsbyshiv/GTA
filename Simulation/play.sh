#!/bin/bash

SERVER_URL="http://localhost:3001"
USERNAME="shiv"

clear
echo "========================================="
echo "    WELCOME TO GTA TERMINAL SIMULATION   "
echo "========================================="
echo "Authenticating player '$USERNAME' with server..."

# 1. Try to login to fetch User ID
RESP=$(curl -s -X POST "$SERVER_URL/auth/login" -H "Content-Type: application/json" -d "{\"username\": \"$USERNAME\", \"password\": \"gta123\"}")
USER_ID=$(echo $RESP | grep -o '"userId":[0-9]*' | cut -d: -f2)

# 2. If login fails, try to register
if [ -z "$USER_ID" ]; then
  echo "Player profile not found. Registering new profile..."
  RESP=$(curl -s -X POST "$SERVER_URL/auth/register" -H "Content-Type: application/json" -d "{\"username\": \"$USERNAME\", \"password\": \"gta123\"}")
  USER_ID=$(echo $RESP | grep -o '"id":[0-9]*' | head -n1 | cut -d: -f2)
fi

if [ -z "$USER_ID" ]; then
  echo "❌ Error: Failed to authenticate or register on server $SERVER_URL."
  exit 1
fi

echo "✅ Authenticated! Player ID: $USER_ID. Fetching state..."
echo "-----------------------------------------"

while true; do
    # Fetch current data from server using user ID
    DATA=$(curl -s -X GET "$SERVER_URL/save/$USER_ID")
    MONEY=$(echo $DATA | grep -o '"money":[0-9]*' | cut -d: -f2)
    WANTED=$(echo $DATA | grep -o '"wanted_level":[0-9]*' | cut -d: -f2)
    
    [ -z "$MONEY" ] && MONEY=0
    [ -z "$WANTED" ] && WANTED=0

    echo ""
    echo "📊 PLAYER STATUS -> Money: \$$MONEY | Wanted Level: ⭐️ $WANTED"
    echo "-----------------------------------------"
    echo "Kya karna chahte ho? Type number and press Enter:"
    echo "1) 🚗 Steal a sports car (Earn \$5000, +1 Wanted Level)"
    echo "2) 💰 Complete a Heist mission (Earn \$20000, +3 Wanted Level)"
    echo "3) 🛡️ Pay Los Santos Customs to lose Wanted Level (-\$1000, Reset Stars)"
    echo "4) 🚪 Exit Game"
    echo "-----------------------------------------"
    read -p "Apna action chuno (1-4): " CHOICE

    case $CHOICE in
        1)
            DIFF_MONEY=5000
            NEW_WANTED=$((WANTED + 1))
            echo "⚡ Aapne ek Zentorno chura li! \$5000 mile par police piche lag gayi!"
            ;;
        2)
            DIFF_MONEY=20000
            NEW_WANTED=$((WANTED + 3))
            echo "🔥 Maze Bank Heist Kamyab raha! \$20000 mile! Police pagla gayi hai!"
            ;;
        3)
            if [ $MONEY -lt 1000 ]; then
                echo "❌ Paise nahi hain! Pehle chori karo!"
                continue
            fi
            DIFF_MONEY=-1000
            NEW_WANTED=0
            echo "🔧 Los Santos Customs ne gaadi ka rang badal diya. Police hat gayi!"
            ;;
        4)
            echo "👋 Game saving... Goodbye!"
            break
            ;;
        *)
            echo "❌ Galat number dabaya!"
            continue
            ;;
    esac

    # Push new state updates to TypeScript server (sending diff_money as increment parameter)
    curl -s -X POST "$SERVER_URL/save/$USER_ID" \
    -H "Content-Type: application/json" \
    -d "{\"money\": $DIFF_MONEY, \"pos_x\": 100, \"pos_y\": 200, \"pos_z\": 50, \"wanted_level\": $NEW_WANTED}" > /dev/null
done
