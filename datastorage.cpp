#include <utility>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/crypto.h>

using eosio::key256;
using eosio::indexed_by;
using eosio::const_mem_fun;
using eosio::asset;
using eosio::permission_level;
using eosio::action;
using eosio::print;
using eosio::name;
using std::string;

typedef int delegatedDecisionMaker;

class CHDataStorage : public eosio::contract {
  public:
    CHDataStorage(account_name self) : contract(self),
      profiles(_self, _self),
      medical_providers(_self, _self),
      approved_providers(_self, _self){}
    int DELEGATED_SELF = 4; // Requires user confirmation
    int DELEGATED_MP = 5; // Medical Provider decide when researchers get access to data
    int DELEGATED_CH = 6; // CollectiveHealth decides when researchers get access to data

    //@abi action
    void createProfile(bool confirmBeforeMPCanStore, bool autoQuery, delegatedDecisionMaker accessor) {
      account_name owner = _self;

      // Temporarily remove this
      // require_auth(owner);

      eosio_assert(accessor == DELEGATED_MP || accessor == DELEGATED_CH, "Accessor must be a known accessor constant");

      eosio_assert(!hasProfile(owner), "Profile already exists");

      // Store new profile
      profiles.emplace(_self, [&](auto& profile){
        profile.owner = owner;
        profile.confirmBeforeMPCanStore = confirmBeforeMPCanStore;
        profile.autoQuery = autoQuery;
        profile.accessor = accessor;
      });
    }

    //@abi action
    void createMedicalProvider(account_name mp, string mpId, string firstName, string lastName) {
      // Hash the id
      checksum256 mpIdHash;
      sha256((char *) mpId.c_str(), sizeof(mpId), mpIdHash);

      // Store new MP
      medical_providers.emplace(_self, [&](auto& medical_provider){
          medical_provider.mp = _self;
          medical_provider.mpId = mpIdHash;
          medical_provider.firstName = firstName;
          medical_provider.lastName = lastName;
      })
    }

    //@abi action
    void addTrustedMp(account_name mp) {
      require_auth(mp);

      eosio_assert(!hasTrustedMp(mp), "Trusted MP already exists");

      // Store new trusted MP
      approved_providers.emplace(_self, [&](auto& approved_provider){
          approved_provider.patient = _self;
          approved_provider.mp = mp;
      })
    }

  private:

    // BUGBUG Check this
    bool hasProfile(account_name owner) {
      auto iterator = profiles.find(owner);
      return iterator != profiles.end();
    }

    // BUGBUG Check this
    bool hasTrustedMp(account_name mp) {
      auto iterator = medical_providers.find(mp);
      return iterator != medical_providers.end();
    }

    //@abi table profiles i64
    struct profile {
        profile(
                account_name owner = account_name(),
                bool confirmBeforeStore,
                bool autoQuery,
                delegatedDecisionMaker accessor
        ):owner(owner),
          confirmBeforeMPCanStore(confirmBeforeStore),
          autoQuery(autoQuery),
          accessor(accessor){}

        // The eos owner account_name of the account in this contract
        account_name owner;
        bool confirmBeforeMPCanStore;
        bool autoQuery;
        delegatedDecisionMaker accessor;

        uint64_t primary_key()const { return owner; }
    };

    typedef eosio::multi_index< N(profile), profile> profile_index;

    //@abi table medical_providers i64
    struct medical_provider {
        medical_provider( account_name mp, checksum256 mpId ):mp(mp), mpId(mpId){}

        // The eos owner account_name of the account in this contract
        account_name mp;

        // Some stringy id to represent the medical provider
        checksum256 mpId;

        string firstName;

        string lastName;

        uint64_t primary_key()const { return mp; }

    };

    typedef eosio::multi_index< N(medical_provider), medical_provider> medical_provider_index;

    //@abi table approved_providers i64
    struct approved_provider {
        approved_provider( account_name patient, account_name mp ): patient(patient), mp(mp){}

        account_name patient;
        account_name mp;

        uint64_t primary_key()const { return patient; }
        uint64_t secondary_key()const { return mp; }
    };

    typedef eosio::multi_index< N(medical_provider), approved_provider> approved_providers_index;

    profile_index profiles;
    medical_provider_index medical_providers;
    approved_providers_index approved_providers;
};