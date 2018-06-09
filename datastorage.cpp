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

class CHDataStorage : public eosio::contract {
  public:
    CHDataStorage(account_name self) : contract(self),
      profiles(_self, _self),
      medical_providers(_self, _self),
      approved_providers(_self, _self){}
    uint8_t DELEGATED_SELF = 4; // Requires user confirmation
    uint8_t DELEGATED_MP = 5; // Medical Provider decide when researchers get access to data
    uint8_t DELEGATED_CH = 6; // CollectiveHealth decides when researchers get access to data

    //@abi action
    void createprofile(bool confirmBeforeMPCanStore, bool autoQuery, uint8_t accessor) {
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
    void createmp(account_name mprovider, string mpId, string firstName, string lastName) {
      // Hash the id
      checksum256 mpIdHash;
      sha256((char *) mpId.c_str(), sizeof(mpId), &mpIdHash);

      // Store new MP
      medical_providers.emplace(_self, [&](auto& mp){
          mp.mpName = _self;
          mp.mpId = mpIdHash;
          mp.firstName = firstName;
          mp.lastName = lastName;
      });
    }

    //@abi action
    void addtrustedmp(account_name mp) {
      require_auth(mp);

      eosio_assert(!hasTrustedMp(mp), "Trusted MP already exists");

      // Store new trusted MP
      approved_providers.emplace(_self, [&](auto& approved_provider){
          approved_provider.patient = _self;
          approved_provider.mprovider = mp;
      });
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

    //@abi table profile i64
    struct profile {
        profile(
                account_name owner = account_name(),
                bool confirmBeforeMPCanStore = false,
                bool autoQuery = false,
                uint8_t accessor = 4
        ):owner(owner),
          confirmBeforeMPCanStore(confirmBeforeMPCanStore),
          autoQuery(autoQuery),
          accessor(accessor){}

        // The eos owner account_name of the account in this contract
        account_name owner;
        bool confirmBeforeMPCanStore;
        bool autoQuery;
        uint8_t accessor;

        uint64_t primary_key()const { return owner; }

        EOSLIB_SERIALIZE( profile, (owner)(confirmBeforeMPCanStore)(autoQuery)(accessor) )
    };

    typedef eosio::multi_index< N(profile), profile> profile_index;

    //@abi table mprv i64
    struct mprv {
        // The eos owner account_name of the account in this contract
        account_name mpName;

        // Some stringy id to represent the medical provider
        checksum256 mpId;

        string firstName;

        string lastName;

        uint64_t primary_key()const { return mpName; }

        EOSLIB_SERIALIZE( mprv, (mpName)(mpId)(firstName)(lastName) )
    };

    typedef eosio::multi_index< N(mprv), mprv> medical_provider_index;

    //@abi table apprvMp i64
    struct apprvMp {
        account_name mprovider;
        account_name patient;

        uint64_t primary_key()const { return mprovider; }

        EOSLIB_SERIALIZE( apprvMp, (mprovider)(patient) )
    };

    typedef eosio::multi_index< N(apprvMp), apprvMp> approved_providers_index;

    profile_index profiles;
    medical_provider_index medical_providers;
    approved_providers_index approved_providers;
};

EOSIO_ABI(CHDataStorage,(createprofile) (createmp) (addtrustedmp))