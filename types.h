//General constants
#define BOARD_SIZE 40
#define NUMBER_OF_pLAYERS 4
#define NUMBER_OF_GROUPS 8
#define NUMBER_OF_NationalEVENT_CARDS 20
#define MAXIMUM_ROUNDS 500
#define STARTING_CASH 30000
#define GO_REWARD 2000
#define BANK_OWNER -1
#define NO_PLAYER -1
#define NO_GROUP -1

// Money and rules related constants
#define INCOME_TAX_BASE_RATE 15
#define COMMUNITY_FUND_BASE_RATE 10
#define MINIMUM_MARKET_TAX_RATE 5
#define MAXIMUM_INCOME_TAX_RATE 35
#define MAXIMUM_COMMUNITY_FUND_RATE 25
#define PROPERTY_TAX_REGULATION_PERCENT 150
#define JAIL_BAIL 300
#define MAXIMUM_JAIL_TURNS 3
#define AUCTION_INCREMENT 250
#define AUCTION_OPENING_PERCENT 50
#define AGGRESSIVE_AUCTION_PERCENT 120
#define CONSERVATIVE_AUCTION_PERCENT 100
#define OPPORTUNISTIC_AUCTION_PERCENT 90
#define MAXIMUM_LOAN_PERCENT 75
#define LOAN_DURATION_ROUNDS 20
#define DEFAULT_LOAN_INTEREST 8
#define INSURANCE_DURATION_ROUNDS 20
#define INSURANCE_WARNING_ROUNDS 3
#define BASIC_INSURANCE_PERCENT 5
#define COMPREHENSIVE_INSURANCE_PERCENT 10
#define BUSINESS_INSURANCE_PERCENT 15
#define BASIC_COMPENSATION_PERCENT 80
#define FULL_COMPENSATION_PERCENT 100
#define HOUSE_MAINTENANCE_PERCENT 5
#define HOTEL_MAINTENANCE_PERCENT 8
#define RENOVATION_PERCENT 10
#define DAMAGED_RENOVATION_PERCENT 25

// Railway and utility constants
#define RAILWAY_PRICE 4000
#define RAILWAY_MORTGAGE 2000
#define UTILITY_PRICE 3000
#define UTILITY_MORTGAGE 1500
#define ONE_RAILWAY_RENT 250
#define TWO_RAILWAY_RENT 500
#define THREE_RAILWAY_RENT 1000
#define FOUR_RAILWAY_RENT 2000
#define ONE_UTILITY_MULTIPLIER 4
#define TWO_UTILITY_MULTIPLIER 10

// Building constants 
#define NO_BUILDINGS 0
#define MAXIMUM_HOUSES 4
#define HOTEL_BUILDING 5
#define STARTING_BUILDING_CONDITION 100
#define CONDITION_LOSS_PER_ROUND 2
#define STRUCTURAL_DAMAGE_ROUNDS 20
#define MAXIMUM_DEPRECIATION 30
#define DEPRECIATION_START_AGE 50
#define DEPRECIATION_INTERVAL 5
#define DEPRECIATION_PERCENT 1

// Event constants
#define INFLATION_INTERVAL 10
#define DISASTER_INTERVAL 10
#define MARKET_REVIEW_INTERVAL 10
#define ECONOMIC_EVENT_INTERVAL 15
#define REGIONAL_EVENT_INTERVAL 15
#define GOVERNMENT_REGULATION_INTERVAL 20
#define MARKET_EFFECT_DURATION 10
#define REGIONAL_EFFECT_DURATION 15
#define EVENT_CARD_EFFECT_DURATION 15
#define TOURISM_HYPE_EFFECT 5
#define CLOSD_PROPERTY_DURATION 2
#define MARKET_GROUP_WAIT 30

// Percentage constants
#define NORMAL_PERCENT 100
#define MARKET_BOOM_PRICE_PERCENT 115
#define MARKET_BOOM_MORTGAGE_PERCENT 115
#define MARKET_BOOM_RENT_PERCENT 125
#define MARKET_BOOM_CONSTRUCTION_PERCENT 110
#define MARKET_BOOM_VALUE_PERCENT 120
#define MARKET_DECLINE_VAL_PERCENT 85
#define MARKET_DECLINE_RENT_PERCENT 80
#define MARKET_DECLINE_MORTGAGE_PERCENT 90
#define MARKET_DECLINE_AUCTION_PERCENT 75

typedef enum
{
    SPACE_START,
    SPACE_PROPERTY,
    SPACE_EVENT,
    SPACE_TAX,
    SPACE_COMMUNITY_FUND,
    SPACE_RAILWAY,
    SPACE_SPECIAL,
    SPACE_UTILITY,
    SPACE_INSURANCE,
    SPACE_BANK
} SpaceType;

typedef enum
{
    GROUP_BROWN,
    GROUP_LIGHT_BLUE,
    GROUP_PINK,
    GROUP_ORANGE,
    GROUP_RED,
    GROUP_YELLOW,
    GROUP_GREEN,
    GROUP_DARK_BLUE
} PropertyGroup;

typedef enum
{
    STRATEGY_AGGRESSIVE,
    STRATEGY_CONSERVATIVE,
    STRATEGY_RISK_TAKER,
    STRATEGY_OPPORTUNISTIC
} PlayerStrategy;

typedef enum
{
    INSURANCE_NONE,
    INSURANCE_BASIC,
    INSURANCE_COMPREHENSIVE,
    INSURANCE_BUSINESS
} InsuranceType;

typedef enum
{
    ECONOMIC_NONE,
    ECONOMIC_TOURISM_BOOM,
    ECONOMIC_FUEL_CRISIS,
    ECONOMIC_HEAVY_MONSOON,
    ECONOMIC_RECESSION,
    ECONOMIC_STOCK_MARKET_BOOM,
    ECONOMIC_HOUSING_PROGRAMME,
    ECONOMIC_FOREIGN_INVESTMENT,
    ECONOMIC_POLITICAL_UNREST
} EconomicEvent;

typedef enum
{
    REGULATION_NONE,
    REGULATION_PROPERTY_TAX,
    REGULATION_REDUCE_INTEREST,
    REGULATION_HOUSING_SUBSIDY,
    REGULATION_LUXURY_TAX,
    REGULATION_RAILWAY_MODERNIZATION,
    REGULATION_ELECTRICITY_TARIFF,
    REGULATION_INSURANCE,
    REGULATION_ANTI_SPECULATION
} GovernmentRegulation;

typedef enum
{
    REGIONAL_NONE,
    REGIONAL_SOUTHERN_TOURISM,
    REGIONAL_PORT_CITY,
    REGIONAL_IT_GROWTH,
    REGIONAL_NORTHERN_DEVELOPMENT,
    REGIONAL_TEA_EXPORT,
    REGIONAL_AIRPORT_EXPANSION,
    REGIONAL_UNIVERSITY_GROWTH,
    REGIONAL_BEACH_POLLUTION,
    REGIONAL_FLOOD_DAMAGE,
    REGIONAL_TRANSPORT_STRIKE,
    REGIONAL_ELECTRICITY_TARIFF,
    REGIONAL_WATER_SHORTAGE
} RegionalEvent;

typedef enum
{
    CARD_TOURISM_HYPE,
    CARD_FUEL_SHORTAGE,
    CARD_HEAVY_FLOODS,
    CARD_POLITICAL_RALLY,
    CARD_STOCK_MARKET_RISE,
    CARD_ECONOMIC_DOWNTURN,
    CARD_HOUSING_SUBSIDY,
    CARD_INTEREST_RATE_CUT,
    CARD_INTEREST_RATE_INCREASE,
    CARD_TAX_AMNESTY,
    CARD_POWER_FAILURE,
    CARD_FOREIGN_FUNDING,
    CARD_PORT_EXPANSION,
    CARD_FESTIVAL_SEASON,
    CARD_LABOUR_STRIKE,
    CARD_INSURANCE_DISCOUNT,
    CARD_PROPERTY_REVALUATION,
    CARD_CURRENCY_DEPRECIATION,
    CARD_GOVERNMENT_GRANT,
    CARD_NATIONAL_DISASTER
} NationalEventCard;

// One normal structure for one board square.
typedef struct
{
    int index;
    int type;
    char name[64];
    int group;
    int purchasePrice;
    int mortgageValue;
    int baseRent;
    int houseCost;
    int hotelCost;
    int marketValue;
    int owner;
    int mortgaged;
    int loanLocked;
    int insurance;
    int insuranceRounds;
    int buildings;
    int propertyAge;
    int depreciationPercent;
    int buildingCondition;
    int ignoredMaintenanceRounds;
    int structuralDamage;
    int damaged;
    int repairCost;
    int closedRounds;
    int coastal;
    int commercial;
} BoardSpace;

// this include all informations about player
typedef struct
{
    char name[64];
    int strategy;
    int cash;
    int position;
    int bankrupt;
    int jailTurns;
    int loanAmount;
    int loanInterestRate;
    int loanRoundsRemaining;
    int experiencedLoss;
    int taxesDue;
    int hotelBonusRounds;
    int festivalBonusRounds;
    int railwayBonusRounds;
    int utilityHalfRounds;
    int constructionStoppedRounds;
    int constructionDiscountPercent;
    int constructionDiscountRounds;
    int constructionIncreasePercent;
    int constructionIncreaseRounds;
    int insuranceDiscountPercent;
    int insuranceDiscountRounds;
} Player;

// function declaraations for all c files
void initialize_board(void);
int roll_dice(void);
void move_player(int playerIndex, int amount);
void print_group_name(int group);
int get_group_size(int group);
int owns_monopoly(int playerIndex, int group);
int count_owned_type(int playerIndex, int type);
int get_effective_purchase_price(int squareIndex);
int get_effective_mortgage_value(int squareIndex);
int get_effective_market_value(int squareIndex);
int get_effective_house_cost(int playerIndex, int squareIndex);
int get_effective_hotel_cost(int playerIndex, int squareIndex);
int get_regional_rent_percent(int squareIndex);

void initialize_players(void);
void determine_turn_order(void);
int should_purchase(int playerIndex, int squareIndex);
int get_auction_limit(int playerIndex, int squareIndex);
void run_auction(int squareIndex);
void develop_properties(int playerIndex);
void perform_maintenance(int playerIndex);
int choose_insurance_property(int playerIndex);

void purchase_square(int playerIndex, int squareIndex, int price);
int calculate_rent(int squareIndex, int diceValue);
void collect_payment(int payerIndex, int amount, int receiverIndex);
int calculate_net_worth(int playerIndex);
int calculate_taxable_assets(int playerIndex);
int calculate_asset_tax(int playerIndex, int ratePercent);
int get_effective_income_tax_rate(void);
void handle_bank_visit(int playerIndex);
void update_loans(void);
void purchase_insurance(int playerIndex);
void update_properties(void);
void repair_damaged_properties(int playerIndex);
void renovate_property(int playerIndex, int squareIndex);
void declare_bankrupt(int playerIndex);

void initialize_event_deck(void);
void draw_national_event(int playerIndex);
void run_periodic_events(void);
void update_temporary_effects(void);
void print_market_conditions(void);

void initialize_game(void);
void run_game(void);
void play_turn(int playerIndex);
void resolve_square(int playerIndex, int diceValue);
void print_round_summary(void);
void print_winner(void);
int count_solvent_players(void);


