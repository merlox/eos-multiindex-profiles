#include <democontract.hpp>

// We'll write all the actions here
void democontract::salute(account_name account) {
    print("Hello, ", name{account});
}

void democontract::create(const account_name account,
    const string& username,
    uint64_t age,
    const string& bio) {

    // This authorization makes sure that the account is being created only by the owner of that name. Auth2 requires that it is signed by a specific permission user as the second parameter
    require_auth2(account, N(owner));
    print("Creating a new profile with the account ", name{account}, "\n");

    // We are creating a profile container with this. The first parameter is the contract code and the second parameter is the scope
    profile_table profile(_self, _self);

    // This find method is using the indexed parameters for finding that element inside the profile struct. If it finds anything, it returns an iterator, otherwise it's empty returns profile.end()
    auto iterator = profile.find(account);

    // If we haven't found an account with that name in the database, create an account otherwise don't create it because this is the create action. We are using an assert function just like in solidity. The assert reverts everything and cancel the transaction which is awesome to avoid permanent damage
    eosio_assert(iterator == profile.end(), "Account already existing with that name");

    // After passing the check, we create the profile with the method .emplace() the first parameter is who's paying for the RAM storage, in this case it's the account. The second parameter is the lambda function which looks like this [](){} the [&] ampersand means "take all the variables in this scope and pass them by reference, not by copy". The (profile_table& p) is the new profile that we want to "emplace" or create.
    profile.emplace(account, [&](auto& p) {
        p.account = account;
        p.username = username;
        p.age = age;
        p.bio = bio;
    });

    print("Profile created");
}

void democontract::update(const account_name account,
    const string& username,
    uint64_t age,
    const string& bio) {

    require_auth2(account, N(owner));
    print("Updating the profile with the account: ", name{account}, "\n");
    profile_table profile(_self, _self);
    auto iterator = profile.find(account);
    eosio_assert(iterator != profile.end(), "The account has not been found");

    // The function to modify the data in the blockchain. Receives the iterator, the payer account and the function
    profile.modify(iterator, account, [&](auto& p) {
        p.username = username;
        p.bio = bio;
        p.age = age;
    });

    print("The account ", name{account}, " has been modified");
}

void democontract::remove(const account_name account){
    require_auth2(account, N(owner));
    print("Removing the profile with the account: ", name{account}, "\n");
    profile_table profile(_self, _self);
    auto iterator = profile.find(account);
    eosio_assert(iterator != profile.end(), "The account has not been found");

    profile.erase(iterator);

    eosio_assert(iterator != profile.end(), "The account found couldn't be delete properly");
    print("The profile found has been deleted successfully");
}

void democontract::getprofile(const account_name account) {
    print("Seaching for the data of the profile for the account: ", name{account}, "\n");

    profile_table profile(_self, _self);
    auto iterator = profile.find(account);
    eosio_assert(iterator != profile.end(), "The profile for that account doesn't exist.");

    print("Account: ", name{iterator->account}, "\n");
    print("Username: ", iterator->username.c_str(), "\n");
    print("Age: ", iterator->age, "\n");
    print("Bio: ", iterator->bio.c_str());
}

void democontract::getbyage(const uint64_t age) {
    print("Getting profiles by the age: ", age, "\n");
    profile_table profile(_self, _self);
    auto age_index = profile.get_index<N(age)>();
    auto iterator = age_index.find(age);

    print("---\n");

    for(; iterator != age_index.end() && iterator->age == age;) {
        print("Account: ", name{iterator->account}, "\n");
        print("Username: ", iterator->username.c_str(), "\n");
        print("Age: ", iterator->age, "\n");
        print("Bio: ", iterator->bio.c_str(), "\n\n");
        ++iterator;
    }
}
