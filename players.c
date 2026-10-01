#ifdef MONOPOLY_IMPLEMENTATION
int turnOrder[NUMBER_OF_pLAYERS];
int recessionActive;

static int orderRolls[NUMBER_OF_pLAYERS];

static void set_player(int index, int strategy)
{
    players[index].strategy = strategy;
    players[index].cash = STARTING_CASH;
    players[index].position = 0;
    players[index].bankrupt = 0;
    players[index].jailTurns = 0;
    players[index].loanAmount = 0;
    players[index].loanInterestRate = 0;
    players[index].loanRoundsRemaining = 0;
    players[index].experiencedLoss = 0;
    players[index].taxesDue = 0;
    players[index].hotelBonusRounds = 0;
    players[index].festivalBonusRounds = 0;
    players[index].railwayBonusRounds = 0;
    players[index].utilityHalfRounds = 0;
    players[index].constructionStoppedRounds = 0;
    players[index].constructionDiscountPercent = NORMAL_PERCENT;
    players[index].constructionDiscountRounds = 0;
    players[index].constructionIncreasePercent = NORMAL_PERCENT;
    players[index].constructionIncreaseRounds = 0;
    players[index].insuranceDiscountPercent = 0;
    players[index].insuranceDiscountRounds = 0;
}

void initialize_players(void)
{
    set_player(0, STRATEGY_AGGRESSIVE);
    snprintf(players[0].name, sizeof players[0].name, "Aggressive Investor");

    set_player(1, STRATEGY_CONSERVATIVE);
    snprintf(players[1].name, sizeof players[1].name, "Conservative Banker");

    set_player(2, STRATEGY_RISK_TAKER);
    snprintf(players[2].name, sizeof players[2].name, "Risk Taker");

    set_player(3, STRATEGY_OPPORTUNISTIC);
    snprintf(players[3].name, sizeof players[3].name, "Opportunistic Trader");
}

static void sort_turn_order_by_initial_rolls(void)
{
    int i;
    int j;
    int temp;
    for (i = 0; i < NUMBER_OF_pLAYERS - 1; i++)
    {
        for (j = i + 1; j < NUMBER_OF_pLAYERS; j++)
        {
            if (orderRolls[turnOrder[j]] > orderRolls[turnOrder[i]])
            {
                temp = turnOrder[i];
                turnOrder[i] = turnOrder[j];
                turnOrder[j] = temp;
            }
        }
    }
}

static void resolve_tie(int start, int end)
{
    int tieRolls[NUMBER_OF_pLAYERS];
    int i;
    int j;
    int player;
    int temp;
    int nextStart;

    printf("Tie! Only tied players roll again.\n");

    for (i = start; i <= end; i++)
    {
        player = turnOrder[i];
        tieRolls[player] = roll_dice();
        printf("%s rolls %d.\n", players[player].name, tieRolls[player]);
    }

    for (i = start; i < end; i++)
    {
        for (j = i + 1; j <= end; j++)
        {
            if (tieRolls[turnOrder[j]] > tieRolls[turnOrder[i]])
            {
                temp = turnOrder[i];
                turnOrder[i] = turnOrder[j];
                turnOrder[j] = temp;
            }
        }
    }

    i = start;
    while (i <= end)
    {
        nextStart = i;
        while (i < end && tieRolls[turnOrder[i]] == tieRolls[turnOrder[i + 1]])
        {
            i++;
        }

        if (nextStart < i)
        {
            resolve_tie(nextStart, i);
        }
        i++;
    }
}

void determine_turn_order(void)
{
    int i;
    int start;

    printf("\nDetermining the First Player\n\n");
    printf("-------------------------\n");

    for (i = 0; i < NUMBER_OF_pLAYERS; i++)
    {
        turnOrder[i] = i;
        orderRolls[i] = roll_dice();
        printf("%s rolls %d.\n", players[i].name, orderRolls[i]);
    }

    sort_turn_order_by_initial_rolls();

    i = 0;
    while (i < NUMBER_OF_pLAYERS)
    {
        start = i;
        while (i < NUMBER_OF_pLAYERS - 1 &&
               orderRolls[turnOrder[i]] == orderRolls[turnOrder[i + 1]])
        {
            i++;
        }

        if (start < i)
        {
            resolve_tie(start, i);
        }
        i++;
    }
    printf("\n%s will begin the game.\n", players[turnOrder[0]].name);

    printf("\nTurn order:\n");
    for (i = 0; i < NUMBER_OF_pLAYERS; i++)
    {
       printf("%s\n", players[turnOrder[i]].name);
    }
}

static int largest_future_rent(int playerIndex)
{
    int i;
    int rent;
    int largest;

    largest = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner != BANK_OWNER && board[i].owner != playerIndex)
        {
            rent = calculate_rent(i, 12);
            if (rent > largest)
            {
                largest = rent;
            }
        }
    }
    return largest;
}

int should_purchase(int playerIndex, int squareIndex)
{
    int price;
    int marketValue;

    price = get_effective_purchase_price(squareIndex);
    marketValue = get_effective_market_value(squareIndex);

    if (players[playerIndex].cash < price)
    {
        return 0;
    }

    if (players[playerIndex].strategy == STRATEGY_AGGRESSIVE)
    {
        return players[playerIndex].cash - price >= largest_future_rent(playerIndex);
    }

    if (players[playerIndex].strategy == STRATEGY_CONSERVATIVE)
    {
        if (recessionActive == 1)
        {
            return 0;
        }
        return price <= players[playerIndex].cash / 2;
    }

    if (players[playerIndex].strategy == STRATEGY_RISK_TAKER)
    {
        return 1;
    }

    if (board[squareIndex].type == SPACE_RAILWAY || board[squareIndex].type == SPACE_UTILITY)
    {
        if (count_owned_type(playerIndex, board[squareIndex].type) == 0)
        {
            return 1;
        }
    }

    if (marketValue > price)
    {
        return 1;
    }

    if (board[squareIndex].type == SPACE_PROPERTY &&
        board[squareIndex].group == boomGroup)
    {
        return 1;
    }

    return 0;
}

int get_auction_limit(int playerIndex, int squareIndex)
{
    int marketValue;
    int limit;

    marketValue = get_effective_market_value(squareIndex);
    limit = 0;

    if (players[playerIndex].strategy == STRATEGY_AGGRESSIVE)
    {
        limit = marketValue * AGGRESSIVE_AUCTION_PERCENT / NORMAL_PERCENT;
    }
    else if (players[playerIndex].strategy == STRATEGY_CONSERVATIVE)
    {
        limit = marketValue * CONSERVATIVE_AUCTION_PERCENT / NORMAL_PERCENT;
        if (limit > players[playerIndex].cash / 2)
        {
            limit = players[playerIndex].cash / 2;
        }
        if (recessionActive == 1)
        {
            limit = limit * 80 / NORMAL_PERCENT;
        }
    }
    else if (players[playerIndex].strategy == STRATEGY_RISK_TAKER)
    {
        limit = players[playerIndex].cash;
    }
    else
    {
        limit = marketValue * OPPORTUNISTIC_AUCTION_PERCENT / NORMAL_PERCENT;
    }

    if (limit > players[playerIndex].cash)
    {
        limit = players[playerIndex].cash;
    }
    return limit;
}

void run_auction(int squareIndex)
{
    int i;
    int price;
    int nextPrice;
    int limit;
    int activeBidders;
    int winner;
    int bidding[NUMBER_OF_pLAYERS];

    price = get_effective_market_value(squareIndex)* AUCTION_OPENING_PERCENT/ NORMAL_PERCENT;

    if (price < AUCTION_INCREMENT)
    {
        price = AUCTION_INCREMENT;
    }

    printf("\nAuction Started.\n");

    printf("\nProperty :\n");
    printf("%s\n", board[squareIndex].name);

    printf("\nOpening Bid :\n");
    printf("LKR %d.\n", price);

    activeBidders = 0;

    // A player joins the auction only if they can afford at least the opening bid.
    
    for (i = 0; i < NUMBER_OF_pLAYERS; i++)
    {
        bidding[i] = 0;

        if (players[i].bankrupt == 0)
        {
            limit = get_auction_limit(i, squareIndex);

            if (limit >= price)
            {
                bidding[i] = 1;
                activeBidders++;
            }
            else
            {
                printf("\n%s withdraws.\n",
                       players[i].name);
            }
        }
    }

    if (activeBidders == 0)
    {
        printf("\nNo valid bid was made.\n");
        return;
    }

    // If only one player can afford to opening bid,that player wins at the opening price.
     
    if (activeBidders == 1)
    {
        winner = NO_PLAYER;

        for (i = 0; i < NUMBER_OF_pLAYERS; i++)
        {
            if (bidding[i] == 1)
            {
                winner = i;
                break;
            }
        }

        if (winner != NO_PLAYER)
        {
            printf("\n%s wins the auction.\n",
                   players[winner].name);

            purchase_square(winner, squareIndex, price);
        }

        return;
    }

    // Continue increasing the bid by LKR 250.until only one bidder remains.
    
    while (activeBidders > 1)
    {
        for (i = 0; i < NUMBER_OF_pLAYERS; i++)
        {
            if (bidding[i] == 1)
            {
                if (activeBidders <= 1)
                {
                    break;
                }

                limit = get_auction_limit(i, squareIndex);

                nextPrice = price + AUCTION_INCREMENT;

                if (nextPrice <= limit)
                {
                    price = nextPrice;

                    printf("\n%s bids LKR %d.\n",
                           players[i].name,
                           price);
                }
                else
                {
                    bidding[i] = 0;
                    activeBidders--;

                    printf("\n%s withdraws.\n",
                           players[i].name);
                }
            }
        }
    }

    winner = NO_PLAYER;

    for (i = 0; i < NUMBER_OF_pLAYERS; i++)
    {
        if (bidding[i] == 1)
        {
            winner = i;
            break;
        }
    }

    if (winner == NO_PLAYER)
    {
        printf("\nNo valid bid was made.\n");
        return;
    }

    printf("\n%s wins the auction.\n",
           players[winner].name);

    purchase_square(winner,
                    squareIndex,
                    price);
}
static int group_has_even_buildings(int playerIndex, int group, int targetSquare)
{
    int i;
    int minimum;

    minimum = HOTEL_BUILDING;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == SPACE_PROPERTY &&
            board[i].group == group &&
            board[i].owner == playerIndex)
        {
            if (board[i].buildings < minimum)
            {
                minimum = board[i].buildings;
            }
        }
    }

    return board[targetSquare].buildings == minimum;
}

void develop_properties(int playerIndex)
{
    int i;
    int cost;

    if (players[playerIndex].constructionStoppedRounds > 0)
    {
        return;
    }

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type != SPACE_PROPERTY || board[i].owner != playerIndex)
        {
            continue;
        }

        if (owns_monopoly(playerIndex, board[i].group) == 0)
        {
            continue;
        }

        if (group_has_even_buildings(playerIndex, board[i].group, i) == 0)
        {
            continue;
        }

        if (board[i].buildings < MAXIMUM_HOUSES)
        {
            cost = get_effective_house_cost(playerIndex, i);
            if (players[playerIndex].cash >= cost * 2)
            {
                players[playerIndex].cash -= cost;
                board[i].buildings++;
                printf("\n%s constructed one house on %s.\n", players[playerIndex].name,board[i].name);
                
                printf("\nConstruction Cost : LKR %d.\n", cost);
            }
        }
        else if (board[i].buildings == MAXIMUM_HOUSES)
        {
            cost = get_effective_hotel_cost(playerIndex, i);
            if (players[playerIndex].cash >= cost * 2)
            {
                players[playerIndex].cash -= cost;
                board[i].buildings = HOTEL_BUILDING;
                printf("\n%s upgraded %s to a Hotel.\n",players[playerIndex].name, board[i].name);
            }
        }
    }
}

static int maintenance_limit(int strategy)
{
    if (strategy == STRATEGY_AGGRESSIVE) return 75;
    if (strategy == STRATEGY_CONSERVATIVE) return 90;
    if (strategy == STRATEGY_RISK_TAKER) return 24;
    return 80;
}

void perform_maintenance(int playerIndex)
{
    int i;
    int limit;
    int cost;

    limit = maintenance_limit(players[playerIndex].strategy);

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex &&
            board[i].buildings > NO_BUILDINGS &&
            board[i].buildingCondition <= limit)
        {
            if (board[i].buildings == HOTEL_BUILDING)
            {
                cost = board[i].hotelCost * HOTEL_MAINTENANCE_PERCENT / NORMAL_PERCENT;
            }
            else
            {
                cost = board[i].houseCost * HOUSE_MAINTENANCE_PERCENT / NORMAL_PERCENT;
                cost = cost * board[i].buildings;
            }
            if (board[i].structuralDamage == 1)
            {
                cost = cost * 150 / 100;
            }

            if (players[playerIndex].cash >= cost)
            {
                players[playerIndex].cash -= cost;
                board[i].buildingCondition = STARTING_BUILDING_CONDITION;
                board[i].ignoredMaintenanceRounds = 0;
                printf("%s maintained %s for LKR %d.\n",
                       players[playerIndex].name, board[i].name, cost);
            }
        }
    }
}

int choose_insurance_property(int playerIndex)
{
    int i;
    int selected;
    int selectedValue;
    int value;
    int suitable;

    selected = -1;
    selectedValue = -1;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        suitable = 0;

        if (board[i].type == SPACE_PROPERTY &&
            board[i].owner == playerIndex &&
            board[i].buildings > NO_BUILDINGS &&
            board[i].insuranceRounds <= INSURANCE_WARNING_ROUNDS)
        {
            if (players[playerIndex].strategy == STRATEGY_AGGRESSIVE ||
                players[playerIndex].strategy == STRATEGY_CONSERVATIVE)
            {
                suitable = 1;
            }
            else if (players[playerIndex].strategy == STRATEGY_RISK_TAKER &&
                     players[playerIndex].experiencedLoss == 1)
            {
                suitable = 1;
            }
            else if (players[playerIndex].strategy == STRATEGY_OPPORTUNISTIC &&
                     get_effective_market_value(i) >= 8000) //I assummed high Value here cap is above 8000
            {
                suitable = 1;
            }
        }

        if (suitable == 1)
        {
            value = get_effective_market_value(i);
            if (value > selectedValue)
            {
                selected = i;
                selectedValue = value;
            }
        }
    }

    return selected;
}
#endif
