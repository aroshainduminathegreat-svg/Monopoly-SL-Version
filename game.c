#ifdef MONOPOLY_IMPLEMENTATION

void initialize_game(void)
{
    int group;

    initialize_board();
    initialize_players();
    initialize_event_deck();

    currentRound = 0;
    inflationRate = 0;
    currentLoanInterest = DEFAULT_LOAN_INTEREST;
    currentIncomeTaxRate = INCOME_TAX_BASE_RATE;
    currentCommunityFundRate = COMMUNITY_FUND_BASE_RATE;
    economicEvent = ECONOMIC_NONE;
    governmentRegulation = REGULATION_NONE;
    regionalEvent = REGIONAL_NONE;
    regionalRoundsRemaining = 0;
    boomGroup = NO_GROUP;
    boomRoundsRemaining = 0;
    declineGroup = NO_GROUP;
    declineRoundsRemaining = 0;
    economicConstructionPercent = NORMAL_PERCENT;
    governmentConstructionPercent = NORMAL_PERCENT;
    economicHotelRentPercent = NORMAL_PERCENT;
    economicRailwayRentPercent = NORMAL_PERCENT;
    governmentRailwayRentPercent = NORMAL_PERCENT;
    governmentUtilityRentPercent = NORMAL_PERCENT;
    economicInsurancePercent = NORMAL_PERCENT;
    governmentInsurancePercent = NORMAL_PERCENT;
    recessionActive = 0;
    luxuryTaxActive = 0;
    antiSpeculationActive = 0;

    for (group = 0; group < NUMBER_OF_GROUPS; group++)
    {
        groupLastSelectedRound[group] = -MARKET_GROUP_WAIT; //I used this to allow all groups to satisfy the 30-round waiting rule at the first review.
    }                                                        //For this initial value all values below -20 are okay.
}                                                           

int count_solvent_players(void)
{
    int i;
    int count;

    count = 0;
    for (i = 0; i < NUMBER_OF_pLAYERS; i++)
    {
        if (players[i].bankrupt == 0)
        {
            count++;
        }
    }
    return count;
}

static int is_purchasable(int squareIndex)
{
    if (board[squareIndex].type == SPACE_PROPERTY) return 1;
    if (board[squareIndex].type == SPACE_RAILWAY) return 1;
    if (board[squareIndex].type == SPACE_UTILITY) return 1;
    return 0;
}

static void resolve_purchasable_space(int playerIndex, int squareIndex, int diceValue)
{
    int price;
    int rent;

    if (board[squareIndex].owner == BANK_OWNER)
    {
        price = get_effective_purchase_price(squareIndex);

        if (should_purchase(playerIndex, squareIndex) == 1)
        {
            purchase_square(playerIndex, squareIndex, price);
        }
        else
        {
            printf("%s declined to purchase %s.\n",
                   players[playerIndex].name,
                   board[squareIndex].name);
            run_auction(squareIndex);
        }
        return;
    }

    if (board[squareIndex].owner == playerIndex)
    {
        if (board[squareIndex].type == SPACE_PROPERTY)
        {
            renovate_property(playerIndex, squareIndex);
        }
        return;
    }

    rent = calculate_rent(squareIndex, diceValue);
    printf("%s landed on %s.\n",
           players[playerIndex].name,
           board[squareIndex].name);

    if (rent <= 0)
    {
        printf("No rent is collected.\n");
        return;
    }

    printf("\nRent Paid : LKR %d.\n\n", rent);

    printf("Owner : %s.\n", players[board[squareIndex].owner].name);
    collect_payment(playerIndex, rent, board[squareIndex].owner);
}

void resolve_square(int playerIndex, int diceValue)
{
    int squareIndex;
    int taxRate;
    int taxAmount;

    squareIndex = players[playerIndex].position;
    printf("Landed on: %s\n", board[squareIndex].name);

    if (is_purchasable(squareIndex) == 1)
    {
        resolve_purchasable_space(playerIndex, squareIndex, diceValue);
        return;
    }

    if (board[squareIndex].type == SPACE_EVENT)
    {
        draw_national_event(playerIndex);
        return;
    }

    if (board[squareIndex].type == SPACE_TAX)
    {
        taxRate = get_effective_income_tax_rate();
        taxAmount = calculate_asset_tax(playerIndex, taxRate);
        printf("%s must pay income tax at %d%%: LKR %d.\n",
               players[playerIndex].name, taxRate, taxAmount);
        collect_payment(playerIndex, taxAmount, NO_PLAYER);
        return;
    }

    if (board[squareIndex].type == SPACE_COMMUNITY_FUND)
    {
        taxRate = currentCommunityFundRate;
        taxAmount = calculate_asset_tax(playerIndex, taxRate);
        printf("%s must pay community fund levy at %d%%: LKR %d.\n",players[playerIndex].name, taxRate, taxAmount);
        collect_payment(playerIndex, taxAmount, NO_PLAYER);
        return;
    }

    if (board[squareIndex].type == SPACE_INSURANCE)
    {
        purchase_insurance(playerIndex);
        return;
    }

    if (board[squareIndex].type == SPACE_BANK)
    {
        handle_bank_visit(playerIndex);
        return;
    }

    if (squareIndex == 30)
    {
        players[playerIndex].position = 10;
        players[playerIndex].jailTurns = 1;
        printf("%s was sent directly to Jail.\n", players[playerIndex].name);
        return;
    }

    if (squareIndex == 20)
    {
        printf("%s is resting at Free Parking.\n", players[playerIndex].name);
        return;
    }

    if (squareIndex == 10)
    {
        printf("%s is just visiting Jail.\n", players[playerIndex].name);
        return;
    }

    if (squareIndex == 0)
    {
        printf("%s landed on GO.\n", players[playerIndex].name);
    }
}

static int handle_jail_turn(int playerIndex)
{
    int diceValue;

    if (players[playerIndex].jailTurns <= 0)
    {
        return 0;
    }

    diceValue = roll_dice();
    printf("%s rolled %d while in Jail.\n",
           players[playerIndex].name, diceValue);

    if (lastRollWasDouble == 1)
    {
        players[playerIndex].jailTurns = 0;
        printf("%s rolled doubles and leaves Jail.\n", players[playerIndex].name);
        move_player(playerIndex, diceValue);
        resolve_square(playerIndex, diceValue);
        return 1;
    }

    if (players[playerIndex].jailTurns >= MAXIMUM_JAIL_TURNS)
    {
        printf("%s pays bail of LKR %d.\n",
               players[playerIndex].name, JAIL_BAIL);
        collect_payment(playerIndex, JAIL_BAIL, NO_PLAYER);

        if (players[playerIndex].bankrupt == 0)
        {
            players[playerIndex].jailTurns = 0;
            move_player(playerIndex, diceValue);
            resolve_square(playerIndex, diceValue);
        }
        return 1;
    }

    players[playerIndex].jailTurns++;
    printf("%s remains in Jail.\n", players[playerIndex].name);
    return 1;
}

void play_turn(int playerIndex)
{
    int diceValue;

    if (players[playerIndex].bankrupt == 1)
    {
        return;
    }

    printf("\n%s's turn\n", players[playerIndex].name);
    printf("---------------------------------------------\n");

    perform_maintenance(playerIndex);
    repair_damaged_properties(playerIndex);

    if (handle_jail_turn(playerIndex) == 1)
    {
        if (players[playerIndex].bankrupt == 0)
        {
            develop_properties(playerIndex);
        }
        return;
    }

    diceValue = roll_dice();
    printf("%s rolled %d.\n", players[playerIndex].name, diceValue);

    move_player(playerIndex, diceValue);
    resolve_square(playerIndex, diceValue);

    if (players[playerIndex].bankrupt == 0)
    {
        develop_properties(playerIndex);
    }
}

void print_round_summary(void)
{
    int i;
    int j;
    int propertyCount;
    int hotelCount;

    printf("\n=============================================\n");
    printf("Round %d Summary\n", currentRound);
    printf("=============================================\n");

    for (i = 0; i < NUMBER_OF_pLAYERS; i++)
    {
        propertyCount = 0;
        hotelCount = 0;

        for (j = 0; j < BOARD_SIZE; j++)
        {
            if (board[j].owner == i)
            {
                propertyCount++;

                if (board[j].type == SPACE_PROPERTY &&
                    board[j].buildings == HOTEL_BUILDING)
                {
                    hotelCount++;
                }
            }
        }

        printf("\n%s\n", players[i].name);

        if (players[i].bankrupt == 1)
        {
            printf("\nBANKRUPT\n");
        }

        printf("\nCash : LKR %d\n", players[i].cash);

        printf("\nNet Worth : LKR %d\n",
               calculate_net_worth(i));

        printf("\nProperties : %d\n",
               propertyCount);

        printf("\nHotels : %d\n",
               hotelCount);

        printf("\nOutstanding Loan : ");

        if (players[i].loanAmount > 0)
        {
            printf("LKR %d\n",
                   players[i].loanAmount);
        }
        else
        {
            printf("None\n");
        }

        printf("\n---------------------------------------------\n");
    }

    print_market_conditions();
}

void print_winner(void)
{
    int i;
    int winner;
    int bestWorth;
    int worth;
    int totalPropertyValue;

    winner = NO_PLAYER;
    bestWorth = 0;

    for (i = 0; i < NUMBER_OF_pLAYERS; i++)
    {
        if (players[i].bankrupt == 0)
        {
            worth = calculate_net_worth(i);

            if (winner == NO_PLAYER || worth > bestWorth)
            {
                bestWorth = worth;
                winner = i;
            }
        }
    }

    printf("\n=============================================\n");
    printf("\nGAME OVER\n");

    if (winner == NO_PLAYER)
    {
        printf("\nNo solvent player remains.\n");
        printf("\n=============================================\n");
        return;
    }

    totalPropertyValue = 0;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == winner)
        {
            totalPropertyValue +=
                get_effective_market_value(i);
        }
    }

    printf("\nWinner\n");
    printf("\n%s\n", players[winner].name);

    printf("\nTotal Cash\n");
    printf("\nLKR %d\n",
           players[winner].cash);

    printf("\nTotal Property Value\n");
    printf("\nLKR %d\n",
           totalPropertyValue);

    printf("\nOutstanding Loans\n");

    if (players[winner].loanAmount > 0)
    {
        printf("\nLKR %d\n",
               players[winner].loanAmount);
    }
    else
    {
        printf("\nNone\n");
    }

    printf("\nNet Worth\n");
    printf("\nLKR %d\n",
           calculate_net_worth(winner));

    printf("\n=============================================\n");
}
void run_game(void)
{
    int turn;
    int playerIndex;


printf("\n");
printf("===================================================================\n");
printf("||                                                               ||\n");
printf("||   M   M   OOO   N   N   OOO   PPPP    OOO   L      Y   Y      ||\n");
printf("||   MM MM  O   O  NN  N  O   O  P   P  O   O  L       Y Y       ||\n");
printf("||   M M M  O   O  N N N  O   O  PPPP   O   O  L        Y        ||\n");
printf("||   M   M  O   O  N  NN  O   O  P      O   O  L        Y        ||\n");
printf("||   M   M   OOO   N   N   OOO   P       OOO   LLLLL    Y        ||\n");
printf("||                                                               ||\n");
printf("||            S R I L A N K A N   E D I T I O N   (LK)           ||\n");
printf("||                                                               ||\n");
printf("||     *Buy         *Build          *Trade          *Conquer     ||\n");
printf("===================================================================\n");

    determine_turn_order();


    while (count_solvent_players() > 1 && currentRound < MAXIMUM_ROUNDS)
    {
        currentRound++;

        printf("\n================ ROUND %d ================\n", currentRound);

        run_periodic_events();

        for (turn = 0; turn < NUMBER_OF_pLAYERS; turn++)
        {
            playerIndex = turnOrder[turn];
            play_turn(playerIndex);

            if (count_solvent_players() <= 1)
            {
                break;
            }
        }

        update_loans();
        update_properties();
        update_temporary_effects();

        if (currentRound % 10 == 0 || count_solvent_players() <= 1)
        {
            print_round_summary();
        }
    }

    print_winner();
}
#endif
