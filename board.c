#ifdef MONOPOLY_IMPLEMENTATION

BoardSpace board[BOARD_SIZE];
Player players[NUMBER_OF_pLAYERS];

int regionalEvent;
int regionalRoundsRemaining;
int boomGroup;
int boomRoundsRemaining;
int declineGroup;
int declineRoundsRemaining;
int economicConstructionPercent;
int governmentConstructionPercent;
int lastRollWasDouble;




static void set_property(int index, int group, int price, int rent,int houseCost, int hotelCost, int coastal, int commercial)
{
    board[index].type = SPACE_PROPERTY;
    board[index].group = group;
    board[index].purchasePrice = price;
    board[index].mortgageValue = price / 2;
    board[index].baseRent = rent;
    board[index].houseCost = houseCost;
    board[index].hotelCost = hotelCost;
    board[index].marketValue = price;
    board[index].coastal = coastal;
    board[index].commercial = commercial;
}

static void set_other_asset(int index, int type, int price, int mortgageValue)
{
    board[index].type = type;
    board[index].purchasePrice = price;
    board[index].mortgageValue = mortgageValue;
    board[index].marketValue = price;
}

void initialize_board(void)
{
    int i;
    
    for (i = 0; i < BOARD_SIZE; i++) //this function initializing all starting values for all squres.
    {
        board[i].index = i;
        board[i].type = SPACE_SPECIAL;
        board[i].name[0] = '\0';
        board[i].group = NO_GROUP;
        board[i].purchasePrice = 0;
        board[i].mortgageValue = 0;
        board[i].baseRent = 0;
        board[i].houseCost = 0;
        board[i].hotelCost = 0;
        board[i].marketValue = 0;
        board[i].owner = BANK_OWNER;
        board[i].mortgaged = 0;
        board[i].loanLocked = 0;
        board[i].insurance = INSURANCE_NONE;
        board[i].insuranceRounds = 0;
        board[i].buildings = NO_BUILDINGS;
        board[i].propertyAge = 0;
        board[i].depreciationPercent = 0;
        board[i].buildingCondition = STARTING_BUILDING_CONDITION;
        board[i].ignoredMaintenanceRounds = 0;
        board[i].structuralDamage = 0;
        board[i].damaged = 0;
        board[i].repairCost = 0;
        board[i].closedRounds = 0;
        board[i].coastal = 0;
        board[i].commercial = 0;
    }

    //Then relevant to the squretype(property/asset) I assigned starting values 
    board[0].type = SPACE_START;
    snprintf(board[0].name, sizeof board[0].name, "GO");

    set_property(1, GROUP_BROWN, 1500, 100, 500, 2000, 0, 1);  //index,group,price,rent,housecost,hotelcost,coastal,commercial
    snprintf(board[1].name, sizeof board[1].name, "Pettah");

    board[2].type = SPACE_COMMUNITY_FUND;
    snprintf(board[2].name, sizeof board[2].name, "Community Development Fund");

    set_property(3, GROUP_BROWN, 1800, 120, 500, 2000, 0, 1);
    snprintf(board[3].name, sizeof board[3].name, "Maradana");

    board[4].type = SPACE_TAX;
    snprintf(board[4].name, sizeof board[4].name, "Income Tax");

    set_other_asset(5, SPACE_RAILWAY, RAILWAY_PRICE, RAILWAY_MORTGAGE); //index,type,price,mortgagevalue
    snprintf(board[5].name, sizeof board[5].name, "Colombo Fort Railway Station");

    set_property(6, GROUP_LIGHT_BLUE, 2500, 180, 750, 3000, 1,1);
    snprintf(board[6].name, sizeof board[6].name, "Bambalapitiya");

    board[7].type = SPACE_EVENT;
    snprintf(board[7].name, sizeof board[7].name, "National Event Card");

    set_property(8, GROUP_LIGHT_BLUE, 2700, 200, 750, 3000, 1, 1);
    snprintf(board[8].name, sizeof board[8].name, "Wellawatte");

    set_property(9, GROUP_LIGHT_BLUE, 3000, 220, 750, 3000, 1, 0);
    snprintf(board[9].name, sizeof board[9].name, "Mount Lavinia");

    board[10].type = SPACE_SPECIAL;
    snprintf(board[10].name, sizeof board[10].name, "Jail / Just Visiting");

    set_property(11, GROUP_PINK, 3500, 260, 1000, 4000, 0, 1);
    snprintf(board[11].name, sizeof board[11].name, "Nugegoda");

    set_other_asset(12, SPACE_UTILITY, UTILITY_PRICE, UTILITY_MORTGAGE);
    snprintf(board[12].name, sizeof board[12].name, "Ceylon Electricity Board");

    set_property(13, GROUP_PINK, 3800, 280, 1000, 4000, 0, 1);
    snprintf(board[13].name, sizeof board[13].name, "Maharagama");

    set_property(14, GROUP_PINK, 4000, 300, 1000, 4000, 0, 1);
    snprintf(board[14].name, sizeof board[14].name, "Kottawa");

    set_other_asset(15, SPACE_RAILWAY, RAILWAY_PRICE, RAILWAY_MORTGAGE);
    snprintf(board[15].name, sizeof board[15].name, "Kandy Railway Station");

    set_property(16, GROUP_ORANGE, 4500, 350, 1250, 5000, 1, 1);
    snprintf(board[16].name, sizeof board[16].name, "Negombo");

    board[17].type = SPACE_INSURANCE;
    snprintf(board[17].name, sizeof board[17].name, "Sri Lanka Insurance");

    set_property(18, GROUP_ORANGE, 4700, 370, 1250, 5000, 0, 1);
    snprintf(board[18].name, sizeof board[18].name, "Katunayake");

    set_property(19, GROUP_ORANGE, 5000, 400, 1250, 5000, 0, 1);
    snprintf(board[19].name, sizeof board[19].name, "Ja-Ela");

    board[20].type = SPACE_SPECIAL;
    snprintf(board[20].name, sizeof board[20].name, "Free Parking");

    set_property(21, GROUP_RED, 5500, 450, 1500, 6000, 0, 1);
    snprintf(board[21].name, sizeof board[21].name, "Kandy City");

    board[22].type = SPACE_EVENT;
    snprintf(board[22].name, sizeof board[22].name, "National Event Card");

    set_property(23, GROUP_RED, 5800, 480, 1500, 6000, 0, 0);
    snprintf(board[23].name, sizeof board[23].name, "Peradeniya");

    set_property(24, GROUP_RED, 6000, 500, 1500, 6000, 0, 0);
    snprintf(board[24].name, sizeof board[24].name, "Katugastota");

    set_other_asset(25, SPACE_RAILWAY, RAILWAY_PRICE, RAILWAY_MORTGAGE);
    snprintf(board[25].name, sizeof board[25].name, "Galle Railway Station");

    set_property(26, GROUP_YELLOW, 6500, 600, 2000, 8000, 1, 1);
    snprintf(board[26].name, sizeof board[26].name, "Galle Fort");

    set_property(27, GROUP_YELLOW, 6800, 620, 2000, 8000, 1, 0);
    snprintf(board[27].name, sizeof board[27].name, "Unawatuna");

    set_other_asset(28, SPACE_UTILITY, UTILITY_PRICE, UTILITY_MORTGAGE);
    snprintf(board[28].name, sizeof board[28].name, "National Water Supply and Drainage Board");

    set_property(29, GROUP_YELLOW, 7000, 650, 2000, 8000, 1, 1);
    snprintf(board[29].name, sizeof board[29].name, "Hikkaduwa");

    board[30].type = SPACE_SPECIAL;
    snprintf(board[30].name, sizeof board[30].name, "Go To Jail");

    set_property(31, GROUP_GREEN, 8000, 750, 2500, 10000, 0, 1);
    snprintf(board[31].name, sizeof board[31].name, "Jaffna Town");

    set_property(32, GROUP_GREEN, 8300, 780, 2500, 10000, 0, 0);
    snprintf(board[32].name, sizeof board[32].name, "Nallur");
   
    board[33].type = SPACE_INSURANCE;
    snprintf(board[33].name, sizeof board[33].name, "Ceylinco Insurance");

    set_property(34, GROUP_GREEN, 8500, 800, 2500, 10000, 1, 1);
    snprintf(board[34].name, sizeof board[34].name, "Trincomalee");

    set_other_asset(35, SPACE_RAILWAY, RAILWAY_PRICE, RAILWAY_MORTGAGE);
    snprintf(board[35].name, sizeof board[35].name, "Jaffna Railway Station");

    board[36].type = SPACE_EVENT;
    snprintf(board[36].name, sizeof board[36].name, "National Event Card");

    set_property(37, GROUP_DARK_BLUE, 10000, 1000, 3000, 12000, 0, 1);
    snprintf(board[37].name, sizeof board[37].name, "Nuwara Eliya");

    board[38].type = SPACE_BANK;
    snprintf(board[38].name, sizeof board[38].name, "Bank of Ceylon");

    set_property(39, GROUP_DARK_BLUE, 12000, 1200, 3000, 12000, 1, 1);
    snprintf(board[39].name, sizeof board[39].name, "Galle Face");
}

int roll_dice(void)
{
    int firstDie;
    int secondDie;

    firstDie = rand() % 6 + 1;
    secondDie = rand() % 6 + 1;
    lastRollWasDouble = (firstDie == secondDie);
    return firstDie + secondDie;
}

void move_player(int playerIndex, int amount)
{
    int oldPosition;
    int newPosition;

    oldPosition = players[playerIndex].position;
    newPosition = oldPosition + amount;

    if (newPosition >= BOARD_SIZE)
    {
        newPosition = newPosition - BOARD_SIZE;
        players[playerIndex].cash = players[playerIndex].cash + GO_REWARD;
        printf("\n%s passed GO.\n",
        players[playerIndex].name);

       printf("Collected LKR %d.\n", GO_REWARD);

       printf("Current Balance : LKR %d.\n",
       players[playerIndex].cash);
    }

    players[playerIndex].position = newPosition;
    printf("%s moves from Square %d to Square %d.\n", players[playerIndex].name, oldPosition, newPosition);
}

void print_group_name(int group)
{
    switch (group)
    {
        case GROUP_BROWN: printf("Brown"); break;
        case GROUP_LIGHT_BLUE: printf("Light Blue"); break;
        case GROUP_PINK: printf("Pink"); break;
        case GROUP_ORANGE: printf("Orange"); break;
        case GROUP_RED: printf("Red"); break;
        case GROUP_YELLOW: printf("Yellow"); break;
        case GROUP_GREEN: printf("Green"); break;
        case GROUP_DARK_BLUE: printf("Dark Blue"); break;
        default: printf("error hapened.group slection failure"); break;
    }
}

int get_group_size(int group)
{
    if (group == GROUP_BROWN || group == GROUP_DARK_BLUE)
    {
        return 2;
    }
    return 3;
}

int owns_monopoly(int playerIndex, int group)
{
    int i;
    int owned;

    owned = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == SPACE_PROPERTY &&
            board[i].group == group &&
            board[i].owner == playerIndex)
        {
            owned++;
        }
    }
    return owned == get_group_size(group);
}

int count_owned_type(int playerIndex, int type)
{
    int i;
    int count;

    count = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == type && board[i].owner == playerIndex)
        {
            count++;
        }
    }
    return count;
}

static int regional_value_percent(int squareIndex)//include only the events that cause property value changes.
{
    int percent;

    percent = NORMAL_PERCENT;

    if (regionalRoundsRemaining <= 0)
    {
        return percent;
    }

    
       //Pettah, Maradana, Colombo Fort
    if (regionalEvent == REGIONAL_PORT_CITY)
    {
        if (squareIndex == 1 ||squareIndex == 3 ||squareIndex == 5)
        {
            percent = 125;
        }
    }

    
    //Nugegoda, Maharagama, Kottawa 
    else if (regionalEvent == REGIONAL_IT_GROWTH)
    {
        if (squareIndex == 11 ||squareIndex == 13 ||squareIndex == 14)
        {
            percent = 120;
        }
    }

    
       //Jaffna Town, Nallur, Trincomalee 
    else if (regionalEvent == REGIONAL_NORTHERN_DEVELOPMENT)
    {
        if (squareIndex == 31 ||squareIndex == 32 ||squareIndex == 34)
        {
            percent = 130;
        }
    }

    
      // Nuwara Eliya 
    else if (regionalEvent == REGIONAL_TEA_EXPORT)
    {
        if (squareIndex == 37)
        {
            percent = 135;
        }
    }

    
      // Kandy City, Peradeniya 
    else if (regionalEvent == REGIONAL_UNIVERSITY_GROWTH)
    {
        if (squareIndex == 21 ||squareIndex == 23)
        {
            percent = 120;
        }
    }

    
      // All coastal properties
    else if (regionalEvent == REGIONAL_FLOOD_DAMAGE)
    {
        if (board[squareIndex].coastal == 1)
        {
            percent = 80;
        }
    }

    
       //Properties around Water Board affect this
    else if (regionalEvent == REGIONAL_WATER_SHORTAGE)
    {
        if (squareIndex == 27 || squareIndex == 29)
        {
            percent = 90;
        }
    }

    return percent;
}

int get_regional_rent_percent(int squareIndex) //include only events that cause rent chnages.
{
    int percent;

    percent = NORMAL_PERCENT;

    if (regionalRoundsRemaining <= 0)
    {
        return percent;
    }

  
      // Galle Fort, Unawatuna, Hikkaduwa 
    if (regionalEvent == REGIONAL_SOUTHERN_TOURISM)
    {
        if (squareIndex == 26 || squareIndex == 27 ||squareIndex == 29)
        {
            percent = 140;
        }
    }

    
     //  Negombo, Katunayake, Ja-Ela 
    else if (regionalEvent == REGIONAL_AIRPORT_EXPANSION)
    {
        if (squareIndex == 16 ||squareIndex == 18 ||squareIndex == 19)
        {
            percent = 130;
        }
    }

   
      // Southern coastal rents 
    else if (regionalEvent == REGIONAL_BEACH_POLLUTION)
    {
        if (squareIndex == 26 ||squareIndex == 27 ||squareIndex == 29)
        {
            percent = 70;
        }
    }

    
       //Railway revenue 
    else if (regionalEvent == REGIONAL_TRANSPORT_STRIKE)
    {
        if (board[squareIndex].type == SPACE_RAILWAY)
        {
            percent = 60;
        }
    }

    
     
    else if (regionalEvent == REGIONAL_ELECTRICITY_TARIFF)
    {
        if (board[squareIndex].type == SPACE_UTILITY)
        {
            percent = 125;
        }
    }

    
       //Water revenue increase
    else if (regionalEvent == REGIONAL_WATER_SHORTAGE)
    {
        if (squareIndex == 28)
        {
            percent = 120;
        }
    }

    return percent;
}

int get_effective_purchase_price(int squareIndex)
{
    int price;

    price = board[squareIndex].purchasePrice;
    if (board[squareIndex].group == boomGroup && boomRoundsRemaining > 0)
    {
        price = price * MARKET_BOOM_PRICE_PERCENT / NORMAL_PERCENT;
    }
    return price;
}

int get_effective_mortgage_value(int squareIndex)
{
    int value;

    value = board[squareIndex].mortgageValue;
    if (board[squareIndex].group == boomGroup && boomRoundsRemaining > 0)
    {
        value = value * MARKET_BOOM_MORTGAGE_PERCENT / NORMAL_PERCENT;
    }
    if (board[squareIndex].group == declineGroup && declineRoundsRemaining > 0)
    {
        value = value * MARKET_DECLINE_MORTGAGE_PERCENT / NORMAL_PERCENT;
    }
    return value;
}

int get_effective_market_value(int squareIndex)
{
    int value;

    value = board[squareIndex].marketValue;
    if (board[squareIndex].group == boomGroup && boomRoundsRemaining > 0)
    {
        value = value * MARKET_BOOM_VALUE_PERCENT / NORMAL_PERCENT;
    }
    if (board[squareIndex].group == declineGroup && declineRoundsRemaining > 0)
    {
        value = value * MARKET_DECLINE_VAL_PERCENT / NORMAL_PERCENT;
    }
    value = value * regional_value_percent(squareIndex) / NORMAL_PERCENT;
    return value;
}

static int effective_building_cost(int playerIndex, int squareIndex, int baseCost)
{
    int cost;

    cost = baseCost;
    if (board[squareIndex].group == boomGroup && boomRoundsRemaining > 0)
    {
        cost = cost * MARKET_BOOM_CONSTRUCTION_PERCENT / NORMAL_PERCENT;
    }
    cost = cost * economicConstructionPercent / NORMAL_PERCENT;
    cost = cost * governmentConstructionPercent / NORMAL_PERCENT;

    if (players[playerIndex].constructionDiscountRounds > 0)
    {
        cost = cost * players[playerIndex].constructionDiscountPercent / NORMAL_PERCENT;
    }
    if (players[playerIndex].constructionIncreaseRounds > 0)
    {
        cost = cost * players[playerIndex].constructionIncreasePercent / NORMAL_PERCENT;
    }
    return cost;
}

int get_effective_house_cost(int playerIndex, int squareIndex)
{
    return effective_building_cost(playerIndex, squareIndex, board[squareIndex].houseCost);
}

int get_effective_hotel_cost(int playerIndex, int squareIndex)
{
    return effective_building_cost(playerIndex, squareIndex, board[squareIndex].hotelCost);
}
#endif
