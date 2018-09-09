#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <string>
using namespace eosio;
using std::string;

class democontract : public contract {
public:
    /// Constructor
    democontract(account_name self) : contract(self) {}

    /// @abi action
    void salute(account_name account);

    /// @abi action
    void create(const account_name account,
        const string& username,
        uint64_t age,
        const string& bio);

    /// @abi action
    void update(const account_name account,
        const string& username,
        uint64_t age,
        const string& bio);

    /// @abi action
    void remove(const account_name account);

    /// @abi action
    void getprofile(const account_name account);

    /// @abi action
    void getbyage(const uint64_t age);

private:
    // N() is the name of the table and , profile is the struct that we'll be using for that table.
    // i64 means that the first uint64_t in that table will be the primary key. In this case we can use the typedef "account_name" because internally it is a uint64_t so "account" is our primary key because we established the i64 in the abi definition below

    // Because the "account" is the primary key and it's an indexed parameter, we need to create a getter for it which is a constant function just like in solidity with the keyword "const". Remember that the first variable is the return value of the function in c++

    // EOSLIB_SERIALIZE() takes the struct name as the first parameter and each data member of the struct as the remanining parameters

    /// @abi table profiles i64
    struct profile {
        account_name account;
        string username;
        uint64_t age;
        string bio;

        account_name primary_key() const {return account;}
        uint64_t get_age() const {return age;}
        EOSLIB_SERIALIZE(profile, (account)(username)(age)(bio))
    };

    // We are creating a new typedef here to make it easier to manipulate this table data so we can use "profile_table" to create the multi-index container for our profiles
    typedef eosio::multi_index<N(profiles), profile, indexed_by<N(age), const_mem_fun<profile, uint64_t, &profile::get_age>>> profile_table;
};

// This is required for the abi generator and it's called a "macro". Remember to add all of the abi actions here when you add them otherwise the ABI generator will fail
EOSIO_ABI(democontract, (salute)(create)(update)(remove)(getprofile)(getbyage))
