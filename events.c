#ifdef MONOPOLY_IMPLEMENTATION

int eventDeck[NUMBER_OF_NationalEVENT_CARDS];
int eventDeckTop;
int currentRound;
int inflationRate;
int currentCommunityFundRate;
int economicEvent;
int luxuryTaxActive;
int antiSpeculationActive;
int groupLastSelectedRound[NUMBER_OF_GROUPS];

static void print_card_name(int card)
{
    switch (card)
    {
        case CARD_TOURISM_HYPE: printf("Tourism Hype"); break;
        case CARD_FUEL_SHORTAGE: printf("Fuel Shortage"); break;
        case CARD_HEAVY_FLOODS: printf("Heavy Floods"); break;
        case CARD_POLITICAL_RALLY: printf("Political Rally"); break;
        case CARD_STOCK_MARKET_RISE: printf("Stock Market Rise"); break;
        case CARD_ECONOMIC_DOWNTURN: printf("Economic Downturn"); break;
        case CARD_HOUSING_SUBSIDY: printf("Housing Subsidy"); break;
        case CARD_INTEREST_RATE_CUT: printf("Interest Rate Cut"); break;
        case CARD_INTEREST_RATE_INCREASE: printf("Interest Rate Increase"); break;
        case CARD_TAX_AMNESTY: printf("Tax Amnesty"); break;
        case CARD_POWER_FAILURE: printf("Power Failure"); break;
        case CARD_FOREIGN_FUNDING: printf("Foreign Funding"); break;
        case CARD_PORT_EXPANSION: printf("Port Expansion"); break;
        case CARD_FESTIVAL_SEASON: printf("Festival Season"); break;
        case CARD_LABOUR_STRIKE: printf("Labour Strike"); break;
        case CARD_INSURANCE_DISCOUNT: printf("Insurance Discount"); break;
        case CARD_PROPERTY_REVALUATION: printf("Property Revaluation"); break;
        case CARD_CURRENCY_DEPRECIATION: printf("Currency Depreciation"); break;
        case CARD_GOVERNMENT_GRANT: printf("Government Grant"); break;
        case CARD_NATIONAL_DISASTER: printf("National Disaster"); break;
        default: printf("Unknown Card"); break;
    }
}
static void print_regional_event(void)
{
    switch (regionalEvent)
    {
        case REGIONAL_SOUTHERN_TOURISM:
            printf("Southern Tourism Boom");
            break;

        case REGIONAL_PORT_CITY:
            printf("Port City Expansion");
            break;

        case REGIONAL_IT_GROWTH:
            printf("IT Industry Growth");
            break;

        case REGIONAL_NORTHERN_DEVELOPMENT:
            printf("Northern Development Programme");
            break;

        case REGIONAL_TEA_EXPORT:
            printf("Tea Export Boom");
            break;

        case REGIONAL_AIRPORT_EXPANSION:
            printf("Airport Expansion");
            break;

        case REGIONAL_UNIVERSITY_GROWTH:
            printf("University City Growth");
            break;

        case REGIONAL_BEACH_POLLUTION:
            printf("Beach Pollution");
            break;

        case REGIONAL_FLOOD_DAMAGE:
            printf("Flood Damage");
            break;

        case REGIONAL_TRANSPORT_STRIKE:
            printf("Transport Strike");
            break;

        case REGIONAL_ELECTRICITY_TARIFF:
            printf("Electricity Tariff Increase");
            break;

        case REGIONAL_WATER_SHORTAGE:
            printf("Water Shortage");
            break;
    }
}

void initialize_event_deck(void)
{
    int i;
    int randomIndex;
    int temp;

    for (i = 0; i < NUMBER_OF_NationalEVENT_CARDS; i++)
    {
        eventDeck[i] = i;
    }

    for (i = 0; i < NUMBER_OF_NationalEVENT_CARDS; i++)
    {
        randomIndex = rand() % NUMBER_OF_NationalEVENT_CARDS;

        temp = eventDeck[i];
        eventDeck[i] = eventDeck[randomIndex];
        eventDeck[randomIndex] = temp;
    }

    eventDeckTop = 0;
}

static void damage_random_property(int playerIndex)
{
    int choices[BOARD_SIZE];
    int count;
    int i;
    int selected;
    int compensation;

    count = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex && board[i].type == SPACE_PROPERTY)
        {
            choices[count] = i;
            count++;
        }
    }

    if (count == 0)
    {
        return;
    }

    selected = choices[rand() % count];
    board[selected].damaged = 1;
    board[selected].repairCost = get_effective_market_value(selected) / 10;
    players[playerIndex].experiencedLoss = 1;

    printf("%s was damaged. Repair cost: LKR %d.\n",
           board[selected].name, board[selected].repairCost);

    if (board[selected].insurance == INSURANCE_BASIC ||
        board[selected].insurance == INSURANCE_COMPREHENSIVE ||
        board[selected].insurance == INSURANCE_BUSINESS)
    {
        compensation = board[selected].repairCost * BASIC_COMPENSATION_PERCENT / NORMAL_PERCENT;
        if (board[selected].insurance != INSURANCE_BASIC)
        {
            compensation = board[selected].repairCost * FULL_COMPENSATION_PERCENT / NORMAL_PERCENT;
        }
        players[playerIndex].cash += compensation;
        printf("Insurance compensation: LKR %d.\n", compensation);
    }
}

static void change_all_property_values(int percent)
{
    int i;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == SPACE_PROPERTY)
        {
            board[i].marketValue = board[i].marketValue * percent / NORMAL_PERCENT;
        }
    }
}

static void change_all_property_rents(int percent)
{
    int i;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].type == SPACE_PROPERTY)
        {
            board[i].baseRent = board[i].baseRent * percent / NORMAL_PERCENT;
        }
    }
}

void draw_national_event(int playerIndex)
{
    int card;
    int i;
    int randomPlayer;
    int randomProperty;
    int randomGroup;

    int choices[BOARD_SIZE];
    int count;

    card = eventDeck[eventDeckTop];

     eventDeckTop++;

    if (eventDeckTop >= NUMBER_OF_NationalEVENT_CARDS)
    {
        eventDeckTop = 0;
    }

    printf("National Event Card: ");
    print_card_name(card);
    printf("\n");

    switch (card)
    {
        case CARD_TOURISM_HYPE:
            players[playerIndex].hotelBonusRounds = 5;
            break;

        case CARD_FUEL_SHORTAGE:
            players[playerIndex].railwayBonusRounds = 5;
            break;

        case CARD_HEAVY_FLOODS:

            count = 0;

            for (i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == SPACE_PROPERTY && board[i].coastal == 1 &&board[i].owner != BANK_OWNER)
                {
                    choices[count] = i;
                    count++;
                }
            }

            if (count > 0)
            {
                randomProperty = choices[rand() % count];

                board[randomProperty].damaged = 1;

                board[randomProperty].repairCost =
                    get_effective_market_value(randomProperty) / 10;

                printf("%s was damaged by heavy floods.\n",
                       board[randomProperty].name);
            }

            break;

        case CARD_POLITICAL_RALLY:

            count = 0;

            for (i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == SPACE_PROPERTY &&
                    board[i].owner != BANK_OWNER)
                {
                    choices[count] = i;
                    count++;
                }
            }

            if (count > 0)
            {
                randomProperty = choices[rand() % count];

                board[randomProperty].closedRounds = 2;

                printf("%s is closed for 2 rounds.\n",
                       board[randomProperty].name);
            }

            break;

        case CARD_STOCK_MARKET_RISE:

            for (i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == SPACE_PROPERTY)
                {
                    board[i].marketValue =
                        board[i].marketValue * 110 / NORMAL_PERCENT;
                }
            }

            printf("All property values increased by 10%%.\n");

            break;

        case CARD_ECONOMIC_DOWNTURN:

            for (i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == SPACE_PROPERTY)
                {
                    board[i].marketValue =
                        board[i].marketValue * 85 / NORMAL_PERCENT;
                }
            }

            printf("All property values decreased by 15%%.\n");

            break;

        case CARD_HOUSING_SUBSIDY:

            players[playerIndex].constructionDiscountPercent = 70;
            players[playerIndex].constructionDiscountRounds =
                EVENT_CARD_EFFECT_DURATION;

            break;

        case CARD_INTEREST_RATE_CUT:

            if (currentLoanInterest >= 2) //I assumed here that the interest rate cannot go below 0.
            {
                currentLoanInterest -= 2;
            }

            break;

        case CARD_INTEREST_RATE_INCREASE:

            currentLoanInterest += 2;

            break;

        case CARD_TAX_AMNESTY:

            for (i = 0; i < NUMBER_OF_pLAYERS; i++)
            {
                if (players[i].bankrupt == 0)
                {
                    players[i].cash += 2000;

                    printf("%s received LKR 2000.\n",
                           players[i].name);
                }
            }

            break;

        case CARD_POWER_FAILURE:

            players[playerIndex].utilityHalfRounds = 3;

            break;

        case CARD_FOREIGN_FUNDING:

            for (i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == SPACE_PROPERTY &&
                    board[i].commercial == 1)
                {
                    board[i].marketValue =
                        board[i].marketValue * 115 / NORMAL_PERCENT;
                }
            }

            printf("Commercial property values increased by 15%%.\n");

            break;

        case CARD_PORT_EXPANSION:

            for (i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == SPACE_RAILWAY)
                {
                    board[i].marketValue = board[i].marketValue * 120 / NORMAL_PERCENT;
                }
            }

            printf("Railway station values increased by 20%%.\n");

            break;

        case CARD_FESTIVAL_SEASON:

            players[playerIndex].festivalBonusRounds =
                EVENT_CARD_EFFECT_DURATION;

            break;

        case CARD_LABOUR_STRIKE:

            players[playerIndex].constructionStoppedRounds = 2;

            break;

        case CARD_INSURANCE_DISCOUNT:

            players[playerIndex].insuranceDiscountPercent = 20;
            players[playerIndex].insuranceDiscountRounds =
                EVENT_CARD_EFFECT_DURATION;

            break;

        case CARD_PROPERTY_REVALUATION:

            randomGroup = rand() % NUMBER_OF_GROUPS;

            for (i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == SPACE_PROPERTY &&
                    board[i].group == randomGroup)
                {
                    board[i].marketValue =
                        board[i].marketValue * 115 / NORMAL_PERCENT;
                }
            }

            printf("Property group ");
            print_group_name(randomGroup);
            printf(" increased in value by 15%%.\n");

            break;

        case CARD_CURRENCY_DEPRECIATION:

            players[playerIndex].constructionIncreasePercent = 110;
            players[playerIndex].constructionIncreaseRounds = EVENT_CARD_EFFECT_DURATION;

            break;

        case CARD_GOVERNMENT_GRANT:

            do
            {
                randomPlayer = rand() % NUMBER_OF_pLAYERS;
            }
            while (players[randomPlayer].bankrupt == 1);

            players[randomPlayer].cash += 5000;

            printf("%s received a government grant of LKR 5000.\n",
                   players[randomPlayer].name);

            break;

        case CARD_NATIONAL_DISASTER:

            count = 0;

            for (i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].type == SPACE_PROPERTY &&
                    board[i].owner != BANK_OWNER &&
                    board[i].buildings > NO_BUILDINGS)
                {
                    choices[count] = i;
                    count++;
                }
            }

            if (count > 0)
            {
                randomProperty = choices[rand() % count];

                board[randomProperty].damaged = 1;

                board[randomProperty].repairCost =
                    get_effective_market_value(randomProperty) / 50;

                printf("%s was damaged by a national disaster.\n",
                       board[randomProperty].name);
            }
            else
            {
                printf("There are no developed properties to damage.\n");
            }

            break;

        default:
            break;
    }
}

static void apply_inflation(void)
{
    int i;
    int selectedRate;
    int multiplier;

    int inflationOptions[6] = {-3, 0, 2, 5, 8, 12};

    selectedRate = inflationOptions[rand() % 6];

    inflationRate = selectedRate;

    multiplier = 100 + selectedRate;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].purchasePrice > 0)
        {
            board[i].purchasePrice =
                board[i].purchasePrice * multiplier / NORMAL_PERCENT;

            board[i].mortgageValue =
                board[i].mortgageValue * multiplier / NORMAL_PERCENT;

            board[i].baseRent =
                board[i].baseRent * multiplier / NORMAL_PERCENT;

            board[i].houseCost =
                board[i].houseCost * multiplier / NORMAL_PERCENT;

            board[i].hotelCost =
                board[i].hotelCost * multiplier / NORMAL_PERCENT;

            board[i].marketValue =
                board[i].marketValue * multiplier / NORMAL_PERCENT;
        }
    }

    if (currentIncomeTaxRate < MAXIMUM_INCOME_TAX_RATE)
    {
        currentIncomeTaxRate++;
    }

    if (currentCommunityFundRate < MAXIMUM_COMMUNITY_FUND_RATE)
    {
        currentCommunityFundRate++;
    }

    printf("Selected inflation rate: %d%%.\n", inflationRate);
}

static void review_property_market(void)
{
    int availableGroups[NUMBER_OF_GROUPS];
    int availableCount;
    int group;
    int selectedIndex;

    availableCount = 0;

    for (group = 0; group < NUMBER_OF_GROUPS; group++)
    {
        if (currentRound - groupLastSelectedRound[group] >= MARKET_GROUP_WAIT)
        {
            availableGroups[availableCount] = group;
            availableCount++;
        }
    }

    if (availableCount < 2)
    {
        printf("Not enough property groups are available for market review.\n");
        return;
    }

    selectedIndex = rand() % availableCount;
    boomGroup = availableGroups[selectedIndex];

    availableGroups[selectedIndex] = availableGroups[availableCount - 1];
    availableCount--;

    selectedIndex = rand() % availableCount;
    declineGroup = availableGroups[selectedIndex];

    groupLastSelectedRound[boomGroup] = currentRound;
    groupLastSelectedRound[declineGroup] = currentRound;

    boomRoundsRemaining = MARKET_EFFECT_DURATION;
    declineRoundsRemaining = MARKET_EFFECT_DURATION;

    printf("Market boom group: ");
    print_group_name(boomGroup);

    printf(". Decline group: ");
    print_group_name(declineGroup);

    printf(".\n");
}

static void print_economic_event(void)
{
    switch (economicEvent)
    {
        case ECONOMIC_TOURISM_BOOM: printf("Tourism Boom"); break;
        case ECONOMIC_FUEL_CRISIS: printf("Fuel Crisis"); break;
        case ECONOMIC_HEAVY_MONSOON: printf("Heavy Monsoon"); break;
        case ECONOMIC_RECESSION: printf("Recession"); break;
        case ECONOMIC_STOCK_MARKET_BOOM: printf("Stock Market Boom"); break;
        case ECONOMIC_HOUSING_PROGRAMME: printf("Housing Programme"); break;
        case ECONOMIC_FOREIGN_INVESTMENT: printf("Foreign Investment"); break;
        case ECONOMIC_POLITICAL_UNREST: printf("Political Unrest"); break;
        default: printf("None"); break;
    }
}

static void start_economic_event(void)
{
    int i;

    economicConstructionPercent = NORMAL_PERCENT;
    economicHotelRentPercent = NORMAL_PERCENT;
    economicRailwayRentPercent = NORMAL_PERCENT;
    economicInsurancePercent = NORMAL_PERCENT;
    recessionActive = 0;

    economicEvent = rand() % 8 + 1;  //random output 1-8

    switch (economicEvent)
    {
        case ECONOMIC_TOURISM_BOOM:
            economicHotelRentPercent = 150;
            break;
        case ECONOMIC_FUEL_CRISIS:
            economicRailwayRentPercent = 150;
            break;
        case ECONOMIC_HEAVY_MONSOON:
            for (i = 0; i < NUMBER_OF_pLAYERS; i++)
            {
                if (players[i].bankrupt == 0) damage_random_property(i);
            }
            break;
        case ECONOMIC_RECESSION:
            change_all_property_values(85);
            change_all_property_rents(90);
            currentLoanInterest = 15;
            recessionActive = 1;
            break;
        case ECONOMIC_STOCK_MARKET_BOOM:
            change_all_property_values(110);
            break;
        case ECONOMIC_HOUSING_PROGRAMME:
            economicConstructionPercent = 75;
            break;
        case ECONOMIC_FOREIGN_INVESTMENT:
            for (i = 0; i < BOARD_SIZE; i++)
            {
                if (board[i].commercial == 1)
                {
                    board[i].marketValue = board[i].marketValue * 120 / NORMAL_PERCENT;
                }
            }
            break;
        case ECONOMIC_POLITICAL_UNREST:
            economicHotelRentPercent = 50;
            break;
        default:
            break;
    }

    printf("Economic Event: ");
    print_economic_event();
    printf(".\n");
}

static void start_government_regulation(void)
{
    governmentConstructionPercent = NORMAL_PERCENT;
    governmentRailwayRentPercent = NORMAL_PERCENT;
    governmentUtilityRentPercent = NORMAL_PERCENT;
    governmentInsurancePercent = NORMAL_PERCENT;
    luxuryTaxActive = 0;
    antiSpeculationActive = 0;

    governmentRegulation = rand() % 8 + 1;

    switch (governmentRegulation)
    {
        case REGULATION_REDUCE_INTEREST:
            if (currentLoanInterest > 2) currentLoanInterest -= 2;
            break;
        case REGULATION_HOUSING_SUBSIDY:
            governmentConstructionPercent = 80;
            break;
        case REGULATION_LUXURY_TAX:
            luxuryTaxActive = 1;
            break;
        case REGULATION_RAILWAY_MODERNIZATION:
            governmentRailwayRentPercent = 125;
            break;
        case REGULATION_ELECTRICITY_TARIFF:
            governmentUtilityRentPercent = 125;
            break;
        case REGULATION_INSURANCE:
            governmentInsurancePercent = 80;
            break;
        case REGULATION_ANTI_SPECULATION:
            antiSpeculationActive = 1;
            break;
        default:
            break;
    }

    printf("A new government regulation is active.\n");
}

static void start_regional_event(void)
{
    regionalEvent = rand() % 12 + 1;

    regionalRoundsRemaining = REGIONAL_EFFECT_DURATION;

    printf("Regional Development Card: ");
    print_regional_event();
    printf("\n");

    printf("This regional event will remain active for %d rounds.\n",
           REGIONAL_EFFECT_DURATION);
}

void run_periodic_events(void)
{
    if (currentRound > 0 && currentRound % INFLATION_INTERVAL == 0)
    {
        apply_inflation();
    }

    if (currentRound > 0 && currentRound % MARKET_REVIEW_INTERVAL == 0)
    {
        review_property_market();
    }

    if (currentRound > 0 && currentRound % ECONOMIC_EVENT_INTERVAL == 0)
    {
        start_economic_event();
    }

    if (currentRound > 0 && currentRound % REGIONAL_EVENT_INTERVAL == 0)
    {
        start_regional_event();
    }

    if (currentRound > 0 && currentRound % GOVERNMENT_REGULATION_INTERVAL == 0)
    {
        start_government_regulation();
    }
}

static void decrease_player_effects(int playerIndex)
{
    if (players[playerIndex].hotelBonusRounds > 0) players[playerIndex].hotelBonusRounds--;
    if (players[playerIndex].festivalBonusRounds > 0)players[playerIndex].festivalBonusRounds--;
    if (players[playerIndex].railwayBonusRounds > 0) players[playerIndex].railwayBonusRounds--;
    if (players[playerIndex].utilityHalfRounds > 0) players[playerIndex].utilityHalfRounds--;
    if (players[playerIndex].constructionStoppedRounds > 0) players[playerIndex].constructionStoppedRounds--;
    if (players[playerIndex].constructionDiscountRounds > 0) players[playerIndex].constructionDiscountRounds--;
    if (players[playerIndex].constructionIncreaseRounds > 0) players[playerIndex].constructionIncreaseRounds--;
    if (players[playerIndex].insuranceDiscountRounds > 0) players[playerIndex].insuranceDiscountRounds--;
}

void update_temporary_effects(void)
{
    int i;

    if (boomRoundsRemaining > 0) boomRoundsRemaining--;
    if (declineRoundsRemaining > 0) declineRoundsRemaining--;
    if (regionalRoundsRemaining > 0) regionalRoundsRemaining--;

    for (i = 0; i < NUMBER_OF_pLAYERS; i++)
    {
        decrease_player_effects(i);
    }
}

void print_market_conditions(void)
{
    printf("\n=============================================\n");
    printf("Current Market Conditions\n");
    printf("=============================================\n");

    printf("\nMarket Boom\n");
    printf("---------------------------------------------\n");

    if (boomRoundsRemaining > 0)
    {
        print_group_name(boomGroup);
        printf(" (+%d%%)\n",
               MARKET_BOOM_VALUE_PERCENT - NORMAL_PERCENT);

        printf("Rounds Remaining : %d\n",
               boomRoundsRemaining);
    }
    else
    {
        printf("None\n");
    }

    printf("\nMarket Decline\n");
    printf("---------------------------------------------\n");

    if (declineRoundsRemaining > 0)
    {
        print_group_name(declineGroup);
        printf(" (-%d%%)\n",
               NORMAL_PERCENT - MARKET_DECLINE_VAL_PERCENT);

        printf("Rounds Remaining : %d\n",
               declineRoundsRemaining);
    }
    else
    {
        printf("None\n");
    }

    printf("\nRegional Development\n");
    printf("---------------------------------------------\n");

    if (regionalRoundsRemaining > 0 &&
        regionalEvent != REGIONAL_NONE)
    {
        print_regional_event();
        printf("\n");

        printf("Rounds Remaining : %d\n",
               regionalRoundsRemaining);
    }
    else
    {
        printf("None\n");
    }

    printf("\nInflation\n");
    printf("---------------------------------------------\n");

    if (inflationRate > 0)
    {
        printf("+%d%%\n", inflationRate);
    }
    else
    {
        printf("%d%%\n", inflationRate);
    }

    printf("\nCurrent Loan Interest\n");
    printf("---------------------------------------------\n");
    printf("%d%%\n", currentLoanInterest);

    printf("\n=============================================\n");
}
#endif
