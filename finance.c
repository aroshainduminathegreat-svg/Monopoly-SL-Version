#ifdef MONOPOLY_IMPLEMENTATION

int currentLoanInterest;
int currentIncomeTaxRate;
int governmentRegulation;
int economicHotelRentPercent;
int economicRailwayRentPercent;
int governmentRailwayRentPercent;
int governmentUtilityRentPercent;
int economicInsurancePercent;
int governmentInsurancePercent;

void purchase_square(int playerIndex, int squareIndex, int price)
{
    if (board[squareIndex].owner != BANK_OWNER)
    {
        return;
    }

    if (players[playerIndex].cash < price)
    {
        return;
    }

    players[playerIndex].cash -= price;
    board[squareIndex].owner = playerIndex;
    board[squareIndex].mortgaged = 0;
    board[squareIndex].loanLocked = 0;

    printf("%s purchased %s for LKR %d.\n",
           players[playerIndex].name,
           board[squareIndex].name,
           price);
    printf("Remaining balance: LKR %d.\n", players[playerIndex].cash);
}

static int condition_rent_percent(int condition)
{
    if (condition >= 90) return 100;
    if (condition >= 75) return 90;
    if (condition >= 50) return 75;
    if (condition >= 25) return 50;
    return 0;
}

static int property_rent_multiplier(int buildings)
{
    if (buildings == 1) return 2;
    if (buildings == 2) return 3;
    if (buildings == 3) return 5;
    if (buildings == 4) return 7;
    if (buildings == HOTEL_BUILDING) return 10;
    return 1;
}

static int railway_rent(int stationsOwned)
{
    if (stationsOwned == 1) return ONE_RAILWAY_RENT;
    if (stationsOwned == 2) return TWO_RAILWAY_RENT;
    if (stationsOwned == 3) return THREE_RAILWAY_RENT;
    if (stationsOwned >= 4) return FOUR_RAILWAY_RENT;
    return 0;
}

int calculate_rent(int squareIndex, int diceValue)
{
    int owner;
    int rent;
    int ownedCount;
    int multiplier;
    int percent;

    if (board[squareIndex].owner == BANK_OWNER)
    {
        return 0;
    }

    if (board[squareIndex].mortgaged == 1 ||
        board[squareIndex].damaged == 1 ||
        board[squareIndex].closedRounds > 0)
    {
        return 0;
    }

    owner = board[squareIndex].owner;
    rent = 0;

    if (board[squareIndex].type == SPACE_PROPERTY)
    {
        multiplier = property_rent_multiplier(board[squareIndex].buildings);
        rent = board[squareIndex].baseRent * multiplier;

        if (board[squareIndex].group == boomGroup && boomRoundsRemaining > 0)
        {
            rent = rent * MARKET_BOOM_RENT_PERCENT / NORMAL_PERCENT;
        }

        if (board[squareIndex].group == declineGroup && declineRoundsRemaining > 0)
        {
            rent = rent * MARKET_DECLINE_RENT_PERCENT / NORMAL_PERCENT;
        }

        rent = rent * get_regional_rent_percent(squareIndex) / NORMAL_PERCENT;

        if (board[squareIndex].buildings > NO_BUILDINGS)
        {
            percent = condition_rent_percent(board[squareIndex].buildingCondition);
            rent = rent * percent / NORMAL_PERCENT;
        }

       if (board[squareIndex].buildings == HOTEL_BUILDING)
        {
        rent = rent * economicHotelRentPercent / NORMAL_PERCENT;

        if (players[owner].hotelBonusRounds > 0)
        {
        rent *= 2;
        }

    if (players[owner].festivalBonusRounds > 0)
    {
        rent = rent * 150 / NORMAL_PERCENT;
    }
}
    }
    else if (board[squareIndex].type == SPACE_RAILWAY)
    {
        ownedCount = count_owned_type(owner, SPACE_RAILWAY);
        rent = railway_rent(ownedCount);
        rent = rent * economicRailwayRentPercent / NORMAL_PERCENT;
        rent = rent * governmentRailwayRentPercent / NORMAL_PERCENT;
        rent = rent * get_regional_rent_percent(squareIndex) / NORMAL_PERCENT;
        if (players[owner].railwayBonusRounds > 0)
        {
            rent *= 2;
        }
    }
    else if (board[squareIndex].type == SPACE_UTILITY)
    {
        ownedCount = count_owned_type(owner, SPACE_UTILITY);
        multiplier = ownedCount >= 2 ? TWO_UTILITY_MULTIPLIER : ONE_UTILITY_MULTIPLIER;
        rent = diceValue * multiplier;
        rent = rent * governmentUtilityRentPercent / NORMAL_PERCENT;
        rent = rent * get_regional_rent_percent(squareIndex) / NORMAL_PERCENT;
        if (players[owner].utilityHalfRounds > 0)
        {
            rent /= 2;
        }
    }

    return rent;
}

static void sell_buildings_to_raise_cash(int playerIndex, int requiredAmount)
{
    int i;
    int saleValue;

    for (i = 0; i < BOARD_SIZE && players[playerIndex].cash < requiredAmount; i++)
    {
        if (board[i].owner == playerIndex &&
            board[i].type == SPACE_PROPERTY &&
            board[i].buildings > NO_BUILDINGS &&
            board[i].loanLocked == 0)
        {
            if (board[i].buildings == HOTEL_BUILDING)
            {
                saleValue = board[i].hotelCost / 2;
            }
            else
            {
                saleValue = board[i].houseCost * board[i].buildings / 2;
            }

            players[playerIndex].cash += saleValue;
            board[i].buildings = NO_BUILDINGS;
            board[i].insurance = INSURANCE_NONE;
            board[i].insuranceRounds = 0;

            printf("%s sold buildings on %s for LKR %d.\n",
                   players[playerIndex].name, board[i].name, saleValue);
        }
    }
}

static void mortgage_assets_to_raise_cash(int playerIndex, int requiredAmount)
{
    int i;
    int value;

    for (i = 0; i < BOARD_SIZE && players[playerIndex].cash < requiredAmount; i++)
    {
        if (board[i].owner == playerIndex &&
            (board[i].type == SPACE_PROPERTY ||
             board[i].type == SPACE_RAILWAY ||
             board[i].type == SPACE_UTILITY) &&
            board[i].mortgaged == 0 &&
            board[i].loanLocked == 0 &&
            board[i].buildings == NO_BUILDINGS)
        {
            value = get_effective_mortgage_value(i);
            board[i].mortgaged = 1;
            players[playerIndex].cash += value;
            printf("%s mortgaged %s for LKR %d.\n",
                   players[playerIndex].name, board[i].name, value);
        }
    }
}

void declare_bankrupt(int playerIndex)
{
    int i;

    if (players[playerIndex].bankrupt == 1)
    {
        return;
    }

    players[playerIndex].bankrupt = 1;
    players[playerIndex].cash = 0;
    players[playerIndex].loanAmount = 0;
    players[playerIndex].loanInterestRate = 0;
    players[playerIndex].loanRoundsRemaining = 0;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex)
        {
            board[i].owner = BANK_OWNER;
            board[i].mortgaged = 0;
            board[i].loanLocked = 0;
            board[i].insurance = INSURANCE_NONE;
            board[i].insuranceRounds = 0;
            board[i].buildings = NO_BUILDINGS;
            board[i].damaged = 0;
            board[i].repairCost = 0;
        }
    }

    printf("%s has been declared bankrupt.\n", players[playerIndex].name);
    printf("\nRemaining assets transferred to the Bank.\n");
}

void collect_payment(int payerIndex, int amount, int receiverIndex)
{
    int paid;

    if (amount <= 0 || players[payerIndex].bankrupt == 1)
    {
        return;
    }

    if (players[payerIndex].cash < amount)
    {
        sell_buildings_to_raise_cash(payerIndex, amount);
    }

    if (players[payerIndex].cash < amount)
    {
        mortgage_assets_to_raise_cash(payerIndex, amount);
    }

    if (players[payerIndex].cash >= amount)
    {
        players[payerIndex].cash -= amount;
        paid = amount;
    }
    else
    {
        paid = players[payerIndex].cash;
        players[payerIndex].cash = 0;
    }

    if (receiverIndex != NO_PLAYER && players[receiverIndex].bankrupt == 0)
    {
        players[receiverIndex].cash += paid;
    }

    if (paid < amount)
    {
        declare_bankrupt(payerIndex);
    }
}

int calculate_net_worth(int playerIndex)
{
    int i;
    int total;

    total = players[playerIndex].cash - players[playerIndex].loanAmount;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex)
        {
            total += get_effective_market_value(i);
            if (board[i].buildings == HOTEL_BUILDING)
            {
                total += board[i].hotelCost;
            }
            else
            {
                total += board[i].houseCost * board[i].buildings;
            }
        }
    }
    return total;
}

int calculate_taxable_assets(int playerIndex)
{
    int i;
    int total;

    total = players[playerIndex].cash;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex)
        {
            total += get_effective_market_value(i);
        }
    }
    return total;
}

int calculate_asset_tax(int playerIndex, int ratePercent)
{
    long long total;

    total = calculate_taxable_assets(playerIndex);
    return (int)(total * ratePercent / NORMAL_PERCENT);
}

int get_effective_income_tax_rate(void)
{
    int rate;

    rate = currentIncomeTaxRate;
    if (governmentRegulation == REGULATION_PROPERTY_TAX)
    {
        rate = rate * PROPERTY_TAX_REGULATION_PERCENT / NORMAL_PERCENT;
    }
    if (rate > MAXIMUM_INCOME_TAX_RATE)
    {
        rate = MAXIMUM_INCOME_TAX_RATE;
    }
    return rate;
}

static int calculate_maximum_loan(int playerIndex)
{
    int i;
    int collateral;

    collateral = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex &&
            board[i].mortgaged == 0 &&
            board[i].buildings == NO_BUILDINGS)
        {
            collateral += get_effective_mortgage_value(i);
        }
    }
    return collateral * MAXIMUM_LOAN_PERCENT / NORMAL_PERCENT;
}

static void lock_collateral(int playerIndex, int loanAmount)
{
    int i;
    int lockedValue;

    lockedValue = 0;
    for (i = 0; i < BOARD_SIZE && lockedValue < loanAmount; i++)
    {
        if (board[i].owner == playerIndex &&
            board[i].mortgaged == 0 &&
            board[i].buildings == NO_BUILDINGS)
        {
            board[i].loanLocked = 1;
            lockedValue += get_effective_mortgage_value(i);
        }
    }
}

static void unlock_collateral(int playerIndex)
{
    int i;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex)
        {
            board[i].loanLocked = 0;
        }
    }
}

static void take_loan(int playerIndex, int amount)
{
    if (amount <= 0 || players[playerIndex].loanAmount > 0)
    {
        return;
    }

    players[playerIndex].loanAmount = amount;
    players[playerIndex].loanInterestRate = currentLoanInterest;
    players[playerIndex].loanRoundsRemaining = LOAN_DURATION_ROUNDS;
    players[playerIndex].cash += amount;
    lock_collateral(playerIndex, amount);

    printf("\n%s obtained a secured loan.\n",
       players[playerIndex].name);

    printf("\nLoan Amount : LKR %d.\n", amount);

    printf("\nCollateral :\n");
    int i;
    for (i = 0; i < BOARD_SIZE; i++)
    {
    if (board[i].owner == playerIndex &&
        board[i].loanLocked == 1)
    {
        printf("%s\n", board[i].name);
    }
}

printf("\nInterest Rate : %d%%\n",
       players[playerIndex].loanInterestRate);

printf("Duration : %d Rounds\n",
       LOAN_DURATION_ROUNDS);
}

static void repay_loan(int playerIndex, int amount)
{
    if (amount <= 0 || players[playerIndex].loanAmount <= 0)
    {
        return;
    }

    if (amount > players[playerIndex].loanAmount)
    {
        amount = players[playerIndex].loanAmount;
    }
    if (amount > players[playerIndex].cash)
    {
        amount = players[playerIndex].cash;
    }

    players[playerIndex].cash -= amount;
    players[playerIndex].loanAmount -= amount;

    printf("\n%s repaid LKR %d.\n",
       players[playerIndex].name,
       amount);

    printf("\nOutstanding Balance :\n");
    printf("LKR %d.\n", players[playerIndex].loanAmount);

    if (players[playerIndex].loanAmount == 0)
    {
        players[playerIndex].loanInterestRate = 0;
        players[playerIndex].loanRoundsRemaining = 0;
        unlock_collateral(playerIndex);
    }
}

void handle_bank_visit(int playerIndex)
{
    int maximumLoan;
    int amount;

    if (players[playerIndex].loanAmount > 0)
    {
        if (players[playerIndex].cash > players[playerIndex].loanAmount * 2)
        {
            repay_loan(playerIndex, players[playerIndex].loanAmount);
        }
        else if (players[playerIndex].cash > 5000)
        {
            repay_loan(playerIndex, players[playerIndex].cash / 4);
        }
        return;
    }

    maximumLoan = calculate_maximum_loan(playerIndex);
    if (maximumLoan <= 0)
    {
        printf("%s has no available collateral.\n", players[playerIndex].name);
        return;
    }

    amount = 0;
    if (players[playerIndex].strategy == STRATEGY_AGGRESSIVE && players[playerIndex].cash < 12000)
    {
        amount = maximumLoan;
    }
    else if (players[playerIndex].strategy == STRATEGY_CONSERVATIVE && players[playerIndex].cash < 5000)
    {
        amount = maximumLoan / 2;
    }
    else if (players[playerIndex].strategy == STRATEGY_RISK_TAKER && players[playerIndex].cash < 10000)
    {
        amount = maximumLoan;
    }
    else if (players[playerIndex].strategy == STRATEGY_OPPORTUNISTIC && boomRoundsRemaining > 0)
    {
        amount = maximumLoan / 2;
    }

    take_loan(playerIndex, amount);
}

static int count_player_assets(int playerIndex)
{
    int i;
    int count;

    count = 0;
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex)
        {
            count++;
        }
    }
    return count;
}

static void foreclose_loan(int playerIndex)
{
    int i;

    printf("\n%s has defaulted.\n",
       players[playerIndex].name);

    printf("\nCollateral has been foreclosed.\n");
    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex && board[i].loanLocked == 1)
        {
            board[i].owner = BANK_OWNER;
            board[i].loanLocked = 0;
            board[i].mortgaged = 0;
            board[i].buildings = NO_BUILDINGS;
        }
    }

    players[playerIndex].loanAmount = 0;
    players[playerIndex].loanInterestRate = 0;
    players[playerIndex].loanRoundsRemaining = 0;
    printf("\nOutstanding debt cleared.\n");

    if (count_player_assets(playerIndex) == 0 && players[playerIndex].cash == 0)
    {
        declare_bankrupt(playerIndex);
    }
}

void update_loans(void)
{
    int i;
    int interest;

    for (i = 0; i < NUMBER_OF_pLAYERS; i++)
    {
        if (players[i].bankrupt == 0 && players[i].loanAmount > 0)
        {
            interest = players[i].loanAmount * players[i].loanInterestRate / NORMAL_PERCENT;
            players[i].loanAmount += interest;
            players[i].loanRoundsRemaining--;

            if (players[i].loanRoundsRemaining <= 0)
            {
                if (players[i].cash >= players[i].loanAmount)
                {
                    repay_loan(i, players[i].loanAmount);
                }
                else
                {
                    foreclose_loan(i);
                }
            }
        }
    }
}

static void print_insurance_name(int insurance)
{
    if (insurance == INSURANCE_BASIC) printf("Basic Insurance");
    else if (insurance == INSURANCE_COMPREHENSIVE) printf("Comprehensive Insurance");
    else if (insurance == INSURANCE_BUSINESS) printf("Business Insurance");
    else printf("No Insurance");
}

void purchase_insurance(int playerIndex)
{
    int squareIndex;
    int premiumPercent;
    int premium;
    int insurance;

    squareIndex = choose_insurance_property(playerIndex);
    if (squareIndex < 0)
    {
        printf("%s has no suitable property for insurance.\n", players[playerIndex].name);
        return;
    }

    if (players[playerIndex].strategy == STRATEGY_CONSERVATIVE)
    {
        insurance = INSURANCE_COMPREHENSIVE;
        premiumPercent = COMPREHENSIVE_INSURANCE_PERCENT;
    }
    else if (players[playerIndex].strategy == STRATEGY_OPPORTUNISTIC)
    {
        insurance = INSURANCE_BUSINESS;
        premiumPercent = BUSINESS_INSURANCE_PERCENT;
    }
    else
    {
        insurance = INSURANCE_BASIC;
        premiumPercent = BASIC_INSURANCE_PERCENT;
    }

    premium = get_effective_market_value(squareIndex) * premiumPercent / NORMAL_PERCENT;
    premium = premium * economicInsurancePercent / NORMAL_PERCENT;
    premium = premium * governmentInsurancePercent / NORMAL_PERCENT;

    if (players[playerIndex].insuranceDiscountRounds > 0)
    {
        premium = premium * (NORMAL_PERCENT - players[playerIndex].insuranceDiscountPercent) / NORMAL_PERCENT;
    }

    if (players[playerIndex].cash < premium)
    {
        return;
    }

    players[playerIndex].cash -= premium;
    board[squareIndex].insurance = insurance;
    board[squareIndex].insuranceRounds = INSURANCE_DURATION_ROUNDS;

    print_insurance_name(insurance);
    printf(" purchased.\n");

    printf("\nProperty : %s\n",
       board[squareIndex].name);

    printf("\nPremium : LKR %d.\n", premium);
}

void update_properties(void)
{
    int i;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].closedRounds > 0)
        {
            board[i].closedRounds--;
        }

        if (board[i].type == SPACE_PROPERTY)
        {
            board[i].propertyAge++;

            if (board[i].propertyAge > DEPRECIATION_START_AGE 
                && board[i].propertyAge % DEPRECIATION_INTERVAL == 0
                 && board[i].depreciationPercent < MAXIMUM_DEPRECIATION)
            {
                board[i].depreciationPercent += DEPRECIATION_PERCENT;
                board[i].marketValue = board[i].marketValue * 99 / NORMAL_PERCENT;
            }

            if (board[i].owner != BANK_OWNER)
            {
                if (board[i].insuranceRounds > 0)
                {
                    board[i].insuranceRounds--;
                    if (board[i].insuranceRounds == INSURANCE_WARNING_ROUNDS)
                    {
                     printf("Insurance policy on %s expires in %d rounds.\n", board[i].name,board[i].insuranceRounds);
                    }

                    if (board[i].insuranceRounds == 0)
                    {
                        board[i].insurance = INSURANCE_NONE;
                    }
                }

                if (board[i].buildings > NO_BUILDINGS)
                {
                    board[i].buildingCondition -= CONDITION_LOSS_PER_ROUND;
                    board[i].ignoredMaintenanceRounds++;
                    if (board[i].buildingCondition < 0)
                    {
                        board[i].buildingCondition = 0;
                    }

                    if (board[i].ignoredMaintenanceRounds > STRUCTURAL_DAMAGE_ROUNDS &&
                        board[i].structuralDamage == 0)
                    {
                        board[i].structuralDamage = 1;
                        board[i].marketValue = board[i].marketValue * 85 / NORMAL_PERCENT;
                        board[i].baseRent = board[i].baseRent * 75 / NORMAL_PERCENT;
                    }
                }
            }
        }
    }
}

static void renovate_structural_damage(int playerIndex, int squareIndex)
{
    int cost;

    if (board[squareIndex].buildings == HOTEL_BUILDING)
    {
        cost = board[squareIndex].hotelCost * DAMAGED_RENOVATION_PERCENT / NORMAL_PERCENT;
    }
    else
    {
        cost = board[squareIndex].houseCost * board[squareIndex].buildings;
        cost = cost * DAMAGED_RENOVATION_PERCENT / NORMAL_PERCENT;
    }

    if (players[playerIndex].cash >= cost)
    {
        players[playerIndex].cash -= cost;
        board[squareIndex].structuralDamage = 0;
        board[squareIndex].buildingCondition = STARTING_BUILDING_CONDITION;
        board[squareIndex].ignoredMaintenanceRounds = 0;
        board[squareIndex].marketValue = board[squareIndex].marketValue * NORMAL_PERCENT / 85;
        board[squareIndex].baseRent = board[squareIndex].baseRent * NORMAL_PERCENT / 75;
        printf("%s repaired structural damage on %s for LKR %d.\n",
               players[playerIndex].name, board[squareIndex].name, cost);
    }
}

void repair_damaged_properties(int playerIndex)
{
    int i;

    for (i = 0; i < BOARD_SIZE; i++)
    {
        if (board[i].owner == playerIndex)
        {
            if (board[i].damaged == 1 && players[playerIndex].cash >= board[i].repairCost)
            {
                players[playerIndex].cash -= board[i].repairCost;
                printf("%s repaired %s for LKR %d.\n",
                       players[playerIndex].name, board[i].name, board[i].repairCost);
                board[i].damaged = 0;
                board[i].repairCost = 0;
                board[i].buildingCondition = STARTING_BUILDING_CONDITION;
            }

            if (board[i].structuralDamage == 1 &&
                players[playerIndex].strategy != STRATEGY_RISK_TAKER)
            {
                renovate_structural_damage(playerIndex, i);
            }
        }
    }
}

void renovate_property(int playerIndex, int squareIndex)
{
    int shouldRenovate;
    int cost;
    int remainingPercent;

    if (board[squareIndex].owner != playerIndex || board[squareIndex].depreciationPercent <= 0)
    {
        return;
    }

    shouldRenovate = 0;
    if (players[playerIndex].strategy == STRATEGY_AGGRESSIVE && board[squareIndex].depreciationPercent >= 20)
    {
        shouldRenovate = 1;
    }
    else if (players[playerIndex].strategy == STRATEGY_CONSERVATIVE && board[squareIndex].depreciationPercent > 10)
    {
        shouldRenovate = 1;
    }
    else if (players[playerIndex].strategy == STRATEGY_OPPORTUNISTIC && board[squareIndex].depreciationPercent > 15)
    {
        shouldRenovate = 1;
    }

    if (shouldRenovate == 0)
    {
        return;
    }

    cost = get_effective_market_value(squareIndex) * RENOVATION_PERCENT / NORMAL_PERCENT;
    if (players[playerIndex].cash < cost)
    {
        return;
    }

    players[playerIndex].cash -= cost;
    remainingPercent = NORMAL_PERCENT - board[squareIndex].depreciationPercent;
    if (remainingPercent > 0)
    {
        board[squareIndex].marketValue = board[squareIndex].marketValue * NORMAL_PERCENT / remainingPercent;
    }
    board[squareIndex].depreciationPercent = 0;
    board[squareIndex].propertyAge = 0;

    printf("%s renovated %s for LKR %d.\n",
           players[playerIndex].name, board[squareIndex].name, cost);
}
#endif
