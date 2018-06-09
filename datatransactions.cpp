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

class CHDataTransaction : public eosio::contract {
	public:
    CHDataTransaction(account_name self) : contract(self),
      patients_data(_self, _self){}
    //@abi action
    void crpatdata(string data_hash, account_name mp, string data_type, uint64_t deltaToken, string timestamp) {
      account_name patient_name = _self;

      // Temporarily remove this
      // require_auth(owner);

      // Store new profile
      patients_data.emplace(_self, [&](auto& patient_data){
        patient_data.data_hash = data_hash;
	    patient_data.patient_name = patient_name;
        patient_data.mp = mp;
		patient_data.data_type = data_type;
        patient_data.deltaToken = deltaToken;
		patient_data.timestamp = timestamp;
      });
    }

  private:
    //@abi table patientdata i64
    struct patient_data {

        // The eos owner account_name of the account in this contract
        string data_hash;
        account_name patient_name;
		account_name mp;
		string data_type;
		uint64_t deltaToken;
		string timestamp;
		
        uint64_t primary_key()const { return patient_name; }
		
		EOSLIB_SERIALIZE( patient_data, (data_hash)(patient_name)(mp)(data_type)(deltaToken)(timestamp) )
    };

    typedef eosio::multi_index< N(patientdata), patient_data> patient_data_index;

    patient_data_index patients_data;
};

EOSIO_ABI( CHDataTransaction,(crpatdata) )
